from CHTL.CHTLNode import nodes

class Generator:
    """
    Traverses the AST and generates an HTML string.
    Uses the visitor pattern.
    """
    def __init__(self):
        # A list of tags that don't need a closing tag.
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}

    def generate(self, ast: nodes.ProgramNode) -> str:
        """The main entry point for the generator."""
        return self._visit(ast)

    def _visit(self, node):
        """Dispatches to the appropriate visit method."""
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node)

    def _generic_visit(self, node):
        raise Exception(f"No visit method found for node type: {type(node).__name__}")

    def _visit_programnode(self, node: nodes.ProgramNode) -> str:
        """Visits the root ProgramNode."""
        return "".join(self._visit(child) for child in node.children if child is not None)

    def _visit_elementnode(self, node: nodes.ElementNode) -> str:
        """Visits an ElementNode and generates an HTML element."""
        # Start the opening tag
        html = f"<{node.tag_name}"

        # Add attributes
        for attr in node.attributes:
            html += f' {attr.name}="{attr.value}"'

        # Handle self-closing tags
        if node.tag_name in self.self_closing_tags:
            html += " />"
            return html

        html += ">"

        # Visit children
        for child in node.children:
            if child is not None:
                html += self._visit(child)

        # Add closing tag
        html += f"</{node.tag_name}>"

        return html

    def _visit_textnode(self, node: nodes.TextNode) -> str:
        """Visits a TextNode and returns its value."""
        # In a real-world scenario, you'd want to escape HTML characters here.
        # For now, we'll return the raw value.
        return node.value
