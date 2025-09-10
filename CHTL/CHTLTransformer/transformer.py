import os
import copy
import zipfile
import tempfile
from typing import List, Union, Any, Optional, Set
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLUtils.module_resolver import ModuleResolver
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode, Node,
    TemplateDefinitionNode, CustomDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    CustomUsageNode, DeleteNode, InsertNode, ImportNode, OriginNode, InfoNode, ExportNode
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext, current_file_path: str, official_module_dir: str = './modules', processing_stack: Optional[Set[str]] = None):
        self.ast = ast
        self.context = context
        self.current_file_path = os.path.abspath(current_file_path)
        self.official_module_dir = official_module_dir
        self.processing_stack = processing_stack if processing_stack is not None else set()

    def transform(self) -> DocumentNode:
        if self.current_file_path in self.processing_stack:
            raise RuntimeError(f"Circular import detected: '{self.current_file_path}' is already being processed.")

        self.processing_stack.add(self.current_file_path)
        transformed_ast = self.visit(self.ast)
        self.processing_stack.remove(self.current_file_path)

        return transformed_ast

    def visit(self, node: BaseNode) -> Any:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)

    def _visit_list(self, node_list: List[BaseNode]) -> List[BaseNode]:
        new_list = []
        i = 0
        while i < len(node_list):
            item = node_list[i]
            result = self.visit(item)
            if isinstance(result, list):
                new_list.extend(result)
            elif result is not None:
                new_list.append(result)
            i += 1
        return new_list

    def generic_visit(self, node: BaseNode) -> BaseNode:
        if hasattr(node, 'children') and node.children:
            node.children = self._visit_list(node.children)
        return node

    def visit_TemplateDefinitionNode(self, node: TemplateDefinitionNode) -> None:
        self.context.add_element_template(node.name, node.content)
        return None

    def visit_CustomDefinitionNode(self, node: CustomDefinitionNode) -> None:
        self.context.add_element_template(node.name, node.content)
        return None

    def visit_OriginNode(self, node: OriginNode) -> Optional[OriginNode]:
        if node.name and node.content:
            self.context.add_origin_block(node.name, node.content)
            return node
        elif node.name and not node.content:
            content = self.context.get_origin_block(node.name)
            if content is None:
                raise NameError(f"Named origin block '{node.name}' not found.")
            return OriginNode(origin_type=node.origin_type, name=node.name, content=content, lineno=node.lineno)
        elif not node.name and node.content:
            return node
        return None

    def visit_InfoNode(self, node: InfoNode) -> None:
        return None

    def visit_ExportNode(self, node: ExportNode) -> None:
        return None

    def visit_ImportNode(self, node: ImportNode) -> None:
        resolver = ModuleResolver(official_module_dir=self.official_module_dir, current_file_path=self.current_file_path)
        resolved_path = resolver.resolve(node.path)

        if not resolved_path:
            raise RuntimeError(f"Could not resolve import '{node.path}' from '{self.current_file_path}'")

        if node.import_type in ('Html', 'Style', 'JavaScript'):
            if not node.alias:
                raise ValueError(f"Import of type '{node.import_type}' requires an 'as' alias.")
            try:
                with open(resolved_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                self.context.add_origin_block(name=node.alias, content=content)
            except FileNotFoundError:
                raise RuntimeError(f"Imported file not found at resolved path: {resolved_path}")
            return None

        namespace = node.alias or os.path.splitext(os.path.basename(resolved_path))[0]
        source_code = ""
        info_node = None
        export_node = None

        if resolved_path.endswith(".cmod"):
            with tempfile.TemporaryDirectory() as temp_dir:
                with zipfile.ZipFile(resolved_path, 'r') as zip_ref:
                    zip_ref.extractall(temp_dir)
                module_name = os.path.splitext(os.path.basename(resolved_path))[0]

                info_file_path = os.path.join(temp_dir, module_name, 'info', f"{module_name}.chtl")
                if os.path.exists(info_file_path):
                    with open(info_file_path, 'r', encoding='utf-8') as f:
                        info_code = f.read()
                    info_lexer = Lexer(info_code)
                    info_tokens = info_lexer.tokenize()
                    info_parser = Parser(info_tokens, info_code, CompilationContext())
                    info_ast = info_parser.parse()
                    for child in info_ast.children:
                        if isinstance(child, InfoNode): info_node = child
                        if isinstance(child, ExportNode): export_node = child

                if info_node:
                    self.context.add_module_info(namespace, {"info": info_node.metadata, "exports": export_node.exported_items if export_node else None})

                main_chtl_file = os.path.join(temp_dir, module_name, 'src', f"{module_name}.chtl")
                if not os.path.exists(main_chtl_file):
                    raise RuntimeError(f"Main CHTL file not found in CMOD '{resolved_path}'.")
                with open(main_chtl_file, 'r', encoding='utf-8') as f:
                    source_code = f.read()
        else:
            try:
                with open(resolved_path, 'r', encoding='utf-8') as f:
                    source_code = f.read()
            except FileNotFoundError:
                raise RuntimeError(f"Imported file not found: {resolved_path}")

        imported_context = CompilationContext()
        lexer = Lexer(source_code, imported_context)
        tokens = lexer.tokenize()
        parser = Parser(tokens, source_code, imported_context)
        import_ast = parser.parse()

        import_transformer = ASTTransformer(
            ast=import_ast,
            context=imported_context,
            current_file_path=resolved_path,
            official_module_dir=self.official_module_dir,
            processing_stack=self.processing_stack.copy()
        )
        import_transformer.transform()

        export_list = export_node.exported_items if export_node else None
        self.context.merge_context(imported_context, namespace, export_list)

        return None

    def visit_TemplateUsageNode(self, node: TemplateUsageNode) -> List[BaseNode]:
        if node.template_type == 'Element':
            template_content = self.context.get_element_template(node.name, namespace=node.from_namespace)
        elif node.template_type == 'Style':
            template_content = self.context.get_style_template(node.name, namespace=node.from_namespace)
        else:
            return []
        if not template_content:
            raise RuntimeError(f"Template '{node.name}' of type '{node.template_type}' not found.")
        return copy.deepcopy(template_content)

    def visit_CustomUsageNode(self, node: CustomUsageNode) -> List[BaseNode]:
        temp_usage_node = TemplateUsageNode(template_type=node.template_type, name=node.name, from_namespace=node.from_namespace, lineno=node.lineno)
        expanded_nodes = self.visit_TemplateUsageNode(temp_usage_node)

        for rule in node.body:
            if isinstance(rule, DeleteNode):
                self._apply_delete(expanded_nodes, rule)
            elif isinstance(rule, InsertNode):
                self._apply_insert_recursive(expanded_nodes, rule)
            elif isinstance(rule, ElementNode):
                self._apply_style_override_recursive(expanded_nodes, [rule])

        self.visit(expanded_nodes)
        return expanded_nodes

    def _apply_delete(self, nodes: List[ElementNode], rule: DeleteNode):
        for target_name in rule.targets:
            for node in nodes:
                if isinstance(node, ElementNode):
                    node.attributes = [attr for attr in node.attributes if attr.name != target_name]

    def _apply_insert_recursive(self, nodes: List[Node], rule: InsertNode) -> bool:
        target_tag = rule.target_selector
        for i, node in enumerate(nodes):
            if isinstance(node, ElementNode):
                if node.tag_name == target_tag:
                    if rule.position == 'after':
                        nodes[i+1:i+1] = rule.content; return True
                    elif rule.position == 'before':
                        nodes[i:i] = rule.content; return True
                    elif rule.position == 'replace':
                        nodes[i:i+1] = rule.content; return True
                    elif rule.position == 'at top':
                        node.children[0:0] = rule.content; return True
                    elif rule.position == 'at bottom':
                        node.children.extend(rule.content); return True
                if node.children and self._apply_insert_recursive(node.children, rule):
                    return True
        return False

    def _apply_style_override_recursive(self, target_nodes: List[Node], override_nodes: List[Node]):
        for override_node in override_nodes:
            if not isinstance(override_node, ElementNode): continue
            for target_node in target_nodes:
                if isinstance(target_node, ElementNode) and target_node.tag_name == override_node.tag_name:
                    override_style_node = next((c for c in override_node.children if isinstance(c, StyleNode)), None)
                    if override_style_node:
                        target_style_node = next((c for c in target_node.children if isinstance(c, StyleNode)), None)
                        if not target_style_node:
                            target_style_node = StyleNode(lineno=override_style_node.lineno)
                            target_node.add_child(target_style_node)
                        target_style_node.children.extend(override_style_node.children)
                    if override_node.children and target_node.children:
                        self._apply_style_override_recursive(target_node.children, override_node.children)
                    break
