import html
from CHTL.CHTLNode import nodes

class Generator:
    def __init__(self):
        self.self_closing_tags = {'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input', 'link', 'meta', 'param', 'source', 'track', 'wbr'}

    def generate(self, ast: nodes.ProgramNode) -> str:
        return self._visit(ast)

    def _visit(self, node):
        method_name = f'_visit_{type(node).__name__.lower()}'
        visitor = getattr(self, method_name, lambda n: "")
        return visitor(node)

    def _visit_programnode(self, node: nodes.ProgramNode) -> str:
        return "".join(self._visit(child) for child in node.children)

    def _visit_elementnode(self, node: nodes.ElementNode) -> str:
        html_str = f"<{node.tag_name}"
        # Sort attributes for deterministic output
        for attr in sorted(node.attributes, key=lambda x: x.name):
            value = attr.value.strip("'\"")
            html_str += f' {attr.name}="{html.escape(value)}"'

        if node.tag_name in self.self_closing_tags and not node.children:
            return html_str + " />"

        html_str += ">"
        html_str += "".join(self._visit(child) for child in node.children)
        html_str += f"</{node.tag_name}>"
        return html_str

    def _visit_textnode(self, node: nodes.TextNode) -> str:
        return html.escape(node.value)
