import copy
from typing import List, Union, Any
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode, Node,
    TemplateDefinitionNode, CustomDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    CustomUsageNode, DeleteNode, InsertNode, ImportNode
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext):
        self.ast = ast
        self.context = context

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
        # These are handled at the context level, so we remove them from the tree
        return None

    def visit_CustomDefinitionNode(self, node: CustomDefinitionNode) -> None:
        # These are handled at the context level, so we remove them from the tree
        return None

    def visit_ImportNode(self, node: ImportNode) -> None:
        try:
            with open(node.path, 'r', encoding='utf-8') as f:
                source_code = f.read()
        except FileNotFoundError:
            raise RuntimeError(f"Imported file not found: {node.path}")

        # Create a new context for the imported file to avoid pollution
        import_context = CompilationContext()
        lexer = Lexer(source_code)
        tokens = lexer.tokenize()
        parser = Parser(tokens, import_context)
        import_ast = parser.parse()

        # Now, we need to extract the definitions from the imported AST
        # and add them to our main context.
        for def_node in import_ast.children:
            if isinstance(def_node, TemplateDefinitionNode):
                # For now, we'll just handle element templates
                if def_node.template_type == 'Element':
                    # TODO: Handle namespaces from import aliases/file names
                    self.context.add_element_template(def_node.name, def_node.content)
        return None # Import nodes are processed and removed from the final AST

    def visit_TemplateUsageNode(self, node: TemplateUsageNode) -> List[BaseNode]:
        if node.template_type == 'Element':
            template_content = self.context.get_element_template(node.name)
        elif node.template_type == 'Style':
            template_content = self.context.get_style_template(node.name)
        else:
            return []
        if not template_content:
            raise RuntimeError(f"Template '{node.name}' of type '{node.template_type}' not found.")
        return copy.deepcopy(template_content)

    def visit_CustomUsageNode(self, node: CustomUsageNode) -> List[BaseNode]:
        expanded_nodes = self.visit_TemplateUsageNode(node)

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
