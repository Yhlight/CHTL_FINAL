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

    def _generate_element_node(self, node: ElementNode) -> str:
        """Generates an HTML element from an ElementNode."""
        tag_name = node.tag_name.lower()

        if tag_name in self.self_closing_tags:
            # Self-closing tags have no children content.
            # Attributes will be added here in a future step.
            return f"<{tag_name}>"

        # Generate children's HTML recursively.
        children_html = "".join(self._generate_node(child) for child in node.children)

        # Attributes will be added to the opening tag in a future step.
        return f"<{tag_name}>{children_html}</{tag_name}>"

    def _generate_text_node(self, node: TextNode) -> str:
        """Generates text content from a TextNode."""
        # Escape the text to prevent HTML injection vulnerabilities.
        return html.escape(node.value)
