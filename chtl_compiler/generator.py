from . import ast

class HtmlGenerator:
    def __init__(self, doc: ast.Document):
        self.doc = doc

    def generate(self) -> str:
        return "".join(self._generate_node(child, indent_level=0) for child in self.doc.children)

    def _generate_node(self, node: ast.Node, indent_level: int) -> str:
        if isinstance(node, ast.ElementNode):
            return self._generate_element(node, indent_level)
        elif isinstance(node, ast.TextNode):
            indent = "  " * indent_level
            return f"{indent}{node.content}\n"
        return ""

    def _generate_element(self, node: ast.ElementNode, indent_level: int) -> str:
        indent = "  " * indent_level

        attrs_list = [f'{attr.name}="{attr.value}"' for attr in node.attributes]

        if node.styles:
            style_str = ";".join(f"{style.prop}:{style.value}" for style in node.styles)
            attrs_list.append(f'style="{style_str}"')

        attrs_str = (" " + " ".join(attrs_list)) if attrs_list else ""

        self_closing_tags = {"br", "img", "hr", "meta", "link", "input", "col", "base", "area", "embed", "param", "source", "track", "wbr"}
        if node.tag in self_closing_tags:
            return f"{indent}<{node.tag}{attrs_str} />\n"

        children_str = "".join(self._generate_node(child, indent_level + 1) for child in node.children)

        if not children_str.strip(): # If no children or only whitespace children
            return f"{indent}<{node.tag}{attrs_str}></{node.tag}>\n"

        return f"{indent}<{node.tag}{attrs_str}>\n{children_str}{indent}</{node.tag}>\n"
