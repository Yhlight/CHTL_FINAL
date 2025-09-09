import copy
from typing import List, Union, Any
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, TextNode, CommentNode, StyleNode,
    TemplateDefinitionNode, TemplateUsageNode, CssRuleNode, CssPropertyNode,
    DocumentContent, StyleContent, Node
)

class ASTTransformer:
    def __init__(self, ast: DocumentNode, context: CompilationContext):
        self.ast = ast
        self.context = context

    def transform(self) -> DocumentNode:
        """
        Transforms the AST by expanding template usages.
        Returns a new, transformed AST.
        """
        # We start visiting from the document node.
        # The visit methods will return either a single node or a list of nodes.
        transformed_children = self.visit(self.ast.children)
        self.ast.children = transformed_children
        return self.ast

    def visit(self, node: Union[Node, List[Node]]) -> Any:
        if isinstance(node, list):
            new_list = []
            for item in node:
                result = self.visit(item)
                if isinstance(result, list):
                    new_list.extend(result)
                else:
                    new_list.append(result)
            return new_list

        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node)

    def generic_visit(self, node: BaseNode) -> BaseNode:
        # Default behavior: do nothing, just return the node.
        return node

    def visit_DocumentNode(self, node: DocumentNode) -> DocumentNode:
        node.children = self.visit(node.children)
        return node

    def visit_ElementNode(self, node: ElementNode) -> ElementNode:
        if node.children:
            node.children = self.visit(node.children)
        return node

    def visit_StyleNode(self, node: StyleNode) -> StyleNode:
        if node.children:
            node.children = self.visit(node.children)
        return node

    def visit_TemplateUsageNode(self, node: TemplateUsageNode) -> List[Node]:
        """
        This is the core of the transformer. It replaces a usage node
        with the actual content from the template definition.
        """
        if node.template_type == 'Element':
            template_content = self.context.get_element_template(node.name)
        elif node.template_type == 'Style':
            template_content = self.context.get_style_template(node.name)
        else:
            # Handle other template types like @Var later if needed
            return []

        if not template_content:
            raise RuntimeError(f"Template '{node.name}' of type '{node.template_type}' not found.")

        # Return a deep copy to ensure each template usage is a unique instance
        return copy.deepcopy(template_content)

    def visit_TemplateDefinitionNode(self, node: TemplateDefinitionNode) -> List[Node]:
        # Template definitions are removed from the final AST as they are not rendered.
        return []

if __name__ == '__main__':
    # This will be tested via the main compiler script later.
    pass
