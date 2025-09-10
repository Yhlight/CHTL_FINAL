import os
import copy
import zipfile
import tempfile
import shutil
from typing import List, Union, Any, Optional
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLUtils.module_resolver import ModuleResolver
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode, Node,
    TemplateDefinitionNode, CustomDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    CustomUsageNode, DeleteNode, InsertNode, ImportNode, OriginNode
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext, current_file_path: str, official_module_dir: str = './modules'):
        self.ast = ast
        self.context = context
        self.current_file_path = current_file_path
        self.official_module_dir = official_module_dir

    def transform(self) -> DocumentNode:
        return self.visit(self.ast)

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
        # Add the template to the context, assuming the default namespace for now.
        # A more advanced implementation would handle finding the current namespace.
        if node.template_type == 'Element':
            self.context.add_element_template(node.name, node.content)
        elif node.template_type == 'Style':
            self.context.add_style_template(node.name, node.content)
        elif node.template_type == 'Var':
            self.context.add_var_template(node.name, node.content)

        # Remove the definition node from the AST after it's been processed.
        return None

    def visit_CustomDefinitionNode(self, node: CustomDefinitionNode) -> None:
        # This is an alias for TemplateDefinitionNode, so the logic is the same.
        # We can treat Custom definitions as templates in the context.
        if node.template_type == 'Element':
            self.context.add_element_template(node.name, node.content)
        elif node.template_type == 'Style':
            self.context.add_style_template(node.name, node.content)
        elif node.template_type == 'Var':
            self.context.add_var_template(node.name, node.content)
        return None

    def visit_OriginNode(self, node: OriginNode) -> Optional[OriginNode]:
        # If the node has a name and content, it's a definition.
        if node.name and node.content:
            self.context.add_origin_block(node.name, node.content)
            # The definition itself can also be rendered.
            return node

        # If it has a name but no content, it's a usage of a named block.
        elif node.name and not node.content:
            content = self.context.get_origin_block(node.name)
            if content is None:
                raise NameError(f"Named origin block '{node.name}' not found.")
            # Return a new node with the content filled in.
            return OriginNode(
                origin_type=node.origin_type,
                name=node.name,
                content=content,
                lineno=node.lineno
            )

        # If it has no name but content, it's an anonymous block.
        elif not node.name and node.content:
            return node

        return None # Should not happen

    def visit_ImportNode(self, node: ImportNode) -> None:
        resolver = ModuleResolver(
            official_module_dir=self.official_module_dir,
            current_file_path=self.current_file_path
        )
        resolved_path = resolver.resolve(node.path)

        if not resolved_path:
            raise RuntimeError(f"Could not resolve import '{node.path}' from '{self.current_file_path}'")

        # Handle raw file imports
        if node.import_type in ('Html', 'Style', 'JavaScript'):
            if not node.alias:
                raise ValueError(f"Import of type '{node.import_type}' requires an 'as' alias.")
            try:
                with open(resolved_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                # These are added to the default namespace, as they are not CHTL modules
                self.context.add_origin_block(name=node.alias, content=content)
            except FileNotFoundError:
                raise RuntimeError(f"Imported file not found at resolved path: {resolved_path}")
            return None # The import node is processed and removed

        # Handle CHTL module imports (.chtl, .cmod)
        namespace = node.alias or os.path.splitext(os.path.basename(resolved_path))[0]
        source_code = ""
        source_path_for_nested_imports = resolved_path

        if resolved_path.endswith(".cmod"):
            with tempfile.TemporaryDirectory() as temp_dir:
                with zipfile.ZipFile(resolved_path, 'r') as zip_ref:
                    zip_ref.extractall(temp_dir)
                module_name = os.path.splitext(os.path.basename(resolved_path))[0]
                main_chtl_file = os.path.join(temp_dir, module_name, 'src', f"{module_name}.chtl")
                if not os.path.exists(main_chtl_file):
                    raise RuntimeError(f"Main CHTL file '{main_chtl_file}' not found in CMOD archive '{resolved_path}'.")
                source_path_for_nested_imports = main_chtl_file
                with open(main_chtl_file, 'r', encoding='utf-8') as f:
                    source_code = f.read()
        else:
            try:
                with open(resolved_path, 'r', encoding='utf-8') as f:
                    source_code = f.read()
            except FileNotFoundError:
                raise RuntimeError(f"Imported file not found at resolved path: {resolved_path}")

        lexer = Lexer(source_code, self.context)
        tokens = lexer.tokenize()
        parser = Parser(tokens, self.context)
        import_ast = parser.parse()

        for child_node in import_ast.children:
            if isinstance(child_node, TemplateDefinitionNode):
                if child_node.template_type == 'Element':
                    self.context.add_element_template(child_node.name, child_node.content, namespace=namespace)
                elif child_node.template_type == 'Style':
                    self.context.add_style_template(child_node.name, child_node.content, namespace=namespace)
                elif child_node.template_type == 'Var':
                    self.context.add_var_template(child_node.name, child_node.content, namespace=namespace)
            elif isinstance(child_node, ImportNode):
                self.visit(child_node)

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
        # Create a temporary TemplateUsageNode to expand the base template,
        # respecting the namespace if one was provided.
        temp_usage_node = TemplateUsageNode(
            template_type=node.template_type,
            name=node.name,
            from_namespace=node.from_namespace,
            lineno=node.lineno
        )
        expanded_nodes = self.visit_TemplateUsageNode(temp_usage_node)

        for rule in node.body:
            if isinstance(rule, DeleteNode):
                self._apply_delete(expanded_nodes, rule)
            elif isinstance(rule, InsertNode):
                self._apply_insert_recursive(expanded_nodes, rule)
            elif isinstance(rule, ElementNode):
                self._apply_style_override_recursive(expanded_nodes, [rule])

        # After transformations, visit the children of the expanded nodes
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
                        for content_node in reversed(rule.content):
                            nodes.insert(i + 1, content_node)
                        return True
                    elif rule.position == 'before':
                        for content_node in reversed(rule.content):
                            nodes.insert(i, content_node)
                        return True
                    elif rule.position == 'replace':
                        nodes[i:i+1] = rule.content
                        return True
                    elif rule.position == 'at top':
                        node.children[0:0] = rule.content
                        return True
                    elif rule.position == 'at bottom':
                        node.children.extend(rule.content)
                        return True

                if node.children:
                    if self._apply_insert_recursive(node.children, rule):
                        return True
        return False

    def _apply_style_override_recursive(self, target_nodes: List[Node], override_nodes: List[Node]):
        for override_node in override_nodes:
            if not isinstance(override_node, ElementNode):
                continue

            for target_node in target_nodes:
                if isinstance(target_node, ElementNode) and target_node.tag_name == override_node.tag_name:
                    override_style_node = next((c for c in override_node.children if isinstance(c, StyleNode)), None)
                    if override_style_node:
                        target_style_node = next((c for c in target_node.children if isinstance(c, StyleNode)), None)
                        if not target_style_node:
                            target_style_node = StyleNode(lineno=override_style_node.lineno)
                            target_node.add_child(target_style_node)

                        for child in override_style_node.children:
                            target_style_node.add_child(child)

                    if override_node.children and target_node.children:
                        self._apply_style_override_recursive(target_node.children, override_node.children)

                    break
