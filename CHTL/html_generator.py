from CHTL.ast.nodes import DocumentNode, ElementNode, AttributeNode, TextNode

class HtmlGenerator:
    def visit(self, node):
        """The main entry point for the visitor."""
        return node.accept(self)

    def visit_documentnode(self, node: DocumentNode):
        """Visits the root document node."""
        return "".join(self.visit(child) for child in node.children)

    def visit_elementnode(self, node: ElementNode):
        """Visits an element node and generates an HTML element."""
        attrs = " ".join(self.visit(attr) for attr in node.attributes)
        attrs_str = f" {attrs}" if attrs else ""

        # For now, all tags are treated as paired tags.
        # Self-closing tags (e.g., <img>, <br>) will be handled later.
        children_html = "".join(self.visit(child) for child in node.children)

        return f"<{node.tag_name}{attrs_str}>{children_html}</{node.tag_name}>"

    def visit_attributenode(self, node: AttributeNode):
        """Visits an attribute node and generates an HTML attribute string."""
        # Basic HTML escaping for attribute values can be added here.
        # For now, we assume values are safe.
        return f'{node.name}="{node.value}"'

    def visit_textnode(self, node: TextNode):
        """Visits a text node."""
        # Basic HTML escaping for text content (e.g., <, >) can be added here.
        return node.value

    def visit_default(self, node):
        """Default visitor method for unhandled node types."""
        raise TypeError(f"No visit_{type(node).__name__.lower()} method for node type {type(node).__name__}")
