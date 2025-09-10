from CHTL.CHTLNode import nodes
import html

class Generator:
    """
    Traverses the AST and generates a final HTML string, including handling
    inline and global styles.
    """
    def __init__(self):
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}
        self.global_styles = []

    def generate(self, ast: nodes.ProgramNode) -> str:
        """The main entry point for the generator."""
        # First pass: collect all global styles from selector rules
        self._collect_styles(ast)

        # Second pass: build the HTML string
        body_html = self._visit(ast, is_root=True)

        # Inject the collected global styles into the <head>
        if self.global_styles:
            style_block = "<style>\n" + "\n".join(self.global_styles) + "\n</style>"
            # A more robust solution would be to find the head node and inject there,
            # but for now, simple string replacement is sufficient if a head tag exists.
            if "<head>" in body_html.lower():
                 body_html = body_html.replace("<head>", f"<head>\n{style_block}", 1)
            else:
                # If no head, inject it at the start (less ideal, but a fallback)
                body_html = style_block + body_html

        return body_html

    def _collect_styles(self, node):
        """Recursively traverses the AST to find and collect global styles."""
        if isinstance(node, nodes.StyleNode):
            for child in node.children:
                if isinstance(child, nodes.StyleSelectorRuleNode):
                    properties_str = ""
                    for prop in child.properties:
                        properties_str += f"  {prop.name}: {prop.value};\n"
                    self.global_styles.append(f"{child.selector} {{\n{properties_str}}}")

        if hasattr(node, 'children'):
            for child in node.children:
                self._collect_styles(child)

    def _visit(self, node, is_root=False):
        """Dispatches to the appropriate visit method."""
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, self._generic_visit)
        return visitor(node)

    def _generic_visit(self, node):
        raise Exception(f"No visit method found for node type: {type(node).__name__}")

    def _visit_programnode(self, node: nodes.ProgramNode) -> str:
        return "".join(self._visit(child) for child in node.children if child)

    def _visit_elementnode(self, node: nodes.ElementNode) -> str:
        # Find the style node among children to process for inline styles
        style_node = next((child for child in node.children if isinstance(child, nodes.StyleNode)), None)
        inline_styles = ""
        if style_node:
            props = [child for child in style_node.children if isinstance(child, nodes.StylePropertyNode)]
            inline_styles = "; ".join(f"{p.name}: {p.value}" for p in props)

        # Build the tag
        html_str = f"<{node.tag_name}"
        for attr in node.attributes:
            html_str += f' {attr.name}="{html.escape(attr.value)}"'

        if inline_styles:
            html_str += f' style="{html.escape(inline_styles)}"'

        if node.tag_name in self.self_closing_tags and not node.children:
            html_str += " />"
            return html_str

        html_str += ">"

        # Visit children, excluding the style node which has already been processed
        for child in node.children:
            if child and not isinstance(child, nodes.StyleNode):
                html_str += self._visit(child)

        html_str += f"</{node.tag_name}>"
        return html_str

    def _visit_textnode(self, node: nodes.TextNode) -> str:
        return html.escape(node.value)

    # Style nodes themselves don't generate output in the second pass
    def _visit_stylenode(self, node: nodes.StyleNode) -> str:
        return ""

    def _visit_stylepropertynode(self, node: nodes.StylePropertyNode) -> str:
        return "" # Handled by _visit_elementnode

    def _visit_styleselectorrulenode(self, node: nodes.StyleSelectorRuleNode) -> str:
        return "" # Handled by _collect_styles
