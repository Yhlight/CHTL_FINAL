from CHTL.CHTLNode import BaseNode, ElementNode, TextNode
import html

class Generator:
    """
    The HTML Generator for the CHTL language. It takes an AST and produces
    an HTML string.
    """
    def __init__(self):
        # A set of HTML tags that are self-closing.
        self.self_closing_tags = {"br", "hr", "img", "input", "link", "meta"}

    def generate(self, ast_root: ElementNode) -> str:
        """
        The main entry point for generating HTML from the AST.
        The parser wraps everything in a "root" node, so we generate its children.
        """
        if not isinstance(ast_root, ElementNode) or ast_root.tag_name != "root":
            raise ValueError("AST root must be a root ElementNode.")

        return "".join(self._generate_node(child) for child in ast_root.children)

    def _generate_node(self, node: BaseNode) -> str:
        """Dispatches to the correct generation method based on node type."""
        if isinstance(node, ElementNode):
            return self._generate_element_node(node)
        elif isinstance(node, TextNode):
            return self._generate_text_node(node)
        else:
            # In a more robust system, this would log an error.
            return ""

    def _generate_attributes(self, node: ElementNode) -> str:
        """Generates the HTML attribute string from a node's attributes dict."""
        if not node.attributes:
            return ""

        parts = []
        for key, value in node.attributes.items():
            # Escape the attribute value to handle special characters like " and >
            escaped_value = html.escape(str(value), quote=True)
            parts.append(f'{key}="{escaped_value}"')

        # Return the parts joined by spaces, with a leading space
        return " " + " ".join(parts)

    def _generate_element_node(self, node: ElementNode) -> str:
        """Generates an HTML element from an ElementNode, including its attributes."""
        tag_name = node.tag_name.lower()
        attr_string = self._generate_attributes(node)

        if tag_name in self.self_closing_tags:
            # Self-closing tags have no children content.
            return f"<{tag_name}{attr_string}>"

        # Generate children's HTML recursively.
        children_html = "".join(self._generate_node(child) for child in node.children)

        return f"<{tag_name}{attr_string}>{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        """Generates text content from a TextNode."""
        # Escape the text to prevent HTML injection vulnerabilities.
        return html.escape(node.value)
