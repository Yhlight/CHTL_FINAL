import copy
from typing import List, Union, Any
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode,
    TemplateDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    CustomUsageNode, DeleteNode, InsertNode
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext):
        self.ast = ast
        self.context = context

    def transform(self) -> DocumentNode:
        transformed_children = self._visit_list(self.ast.children)
        self.ast.children = transformed_children
        return self.ast

    def _visit_list(self, node_list: List[BaseNode]) -> List[BaseNode]:
        new_list = []
        for item in node_list:
            result = self.visit(item)
            if isinstance(result, list):
                new_list.extend(result)
            elif result is not None:
                new_list.append(result)
        return new_list

    def visit(self, node: BaseNode) -> Any:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)

    def generic_visit(self, node: BaseNode) -> BaseNode:
        return node

    def visit_DocumentNode(self, node: DocumentNode) -> DocumentNode:
        node.children = self._visit_list(node.children)
        return node

    def visit_ElementNode(self, node: ElementNode) -> ElementNode:
        if node.children:
            node.children = self._visit_list(node.children)
        return node

    def visit_StyleNode(self, node: StyleNode) -> StyleNode:
        if node.children:
            node.children = self._visit_list(node.children)
        return node

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
        # 1. Expand the base template
        expanded_nodes = self.visit_TemplateUsageNode(node)

        # 2. Apply specialization rules from the body
        for rule in node.body:
            if isinstance(rule, DeleteNode):
                self._apply_delete(expanded_nodes, rule)
            # Future rules like InsertNode would be handled here
            # elif isinstance(rule, InsertNode):
            #     self._apply_insert(expanded_nodes, rule)

        return expanded_nodes

    def _apply_delete(self, nodes: List[ElementNode], rule: DeleteNode):
        for target_name in rule.targets:
            # For now, assume we are deleting attributes from the first element
            # This is a simplification and will need to be made more robust
            if not nodes or not isinstance(nodes[0], ElementNode):
                continue

            target_element = nodes[0]
            target_element.attributes = [
                attr for attr in target_element.attributes if attr.name != target_name
            ]

    def visit_TemplateDefinitionNode(self, node: TemplateDefinitionNode) -> None:
        # Template and Custom definitions are removed from the final AST
        return None

if __name__ == '__main__':
    # This will be tested via the main compiler script and unit tests.
    pass
