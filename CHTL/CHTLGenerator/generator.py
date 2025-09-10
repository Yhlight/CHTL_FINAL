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
        self.global_styles = [] # Reset for multiple calls
        self._collect_styles(ast)
        body_html = self._visit(ast)

        if self.global_styles:
            style_block = "<style>\n" + "\n".join(self.global_styles) + "\n</style>"
            if "<head>" in body_html.lower():
                 body_html = body_html.replace("<head>", f"<head>\n{style_block}", 1)
            else:
                body_html = style_block + body_html

        return body_html

    def _collect_styles(self, node, parent_element=None):
        """Recursively traverses the AST to find and collect global styles."""
        if isinstance(node, nodes.StyleNode) and parent_element:
            for child in node.children:
                if isinstance(child, nodes.StyleSelectorRuleNode):
                    selector = child.selector
                    # Resolve '&' selectors
                    if selector.startswith('&'):
                        class_attr = next((attr for attr in parent_element.attributes if attr.name == 'class'), None)
                        id_attr = next((attr for attr in parent_element.attributes if attr.name == 'id'), None)

                        if class_attr:
                            parent_selector = "." + class_attr.value.split()[0]
                            selector = selector.replace('&', parent_selector, 1)
                        elif id_attr:
                            parent_selector = "#" + id_attr.value
                            selector = selector.replace('&', parent_selector, 1)
                        else:
                            selector = selector[1:].lstrip() # Remove '&' and leading space

                    properties_str = "".join([f"  {p.name}: {p.value};\n" for p in child.properties])
                    self.global_styles.append(f"{selector} {{\n{properties_str}}}")

        if hasattr(node, 'children'):
            new_parent = node if isinstance(node, nodes.ElementNode) else parent_element
            for child in node.children:
                if child: self._collect_styles(child, new_parent)

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
