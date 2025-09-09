from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode
)
import html

class HTMLGenerator:
    def __init__(self, ast: DocumentNode):
        self.ast = ast
        self.global_styles = []
        self.self_closing_tags = {
            'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
            'link', 'meta', 'param', 'source', 'track', 'wbr'
        }

    def generate(self) -> str:
        self.global_styles = [] # Reset for multiple calls
        self.find_and_process_styles(self.ast)
        return self.visit(self.ast)

    def visit(self, node: Node, indent_level: int = 0) -> str:
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node, indent_level)

    def generic_visit(self, node: Node, indent_level: int):
        raise Exception(f'No visit_{type(node).__name__} method')

    def find_and_process_styles(self, node: Node):
        if isinstance(node, StyleNode):
            for style_child in node.children:
                if isinstance(style_child, CssRuleNode):
                    self.global_styles.append(self.visit(style_child, 2))
        elif hasattr(node, 'children'):
            for child in node.children:
                self.find_and_process_styles(child)

    def visit_DocumentNode(self, node: DocumentNode, indent_level: int) -> str:
        html_parts = [self.visit(child, indent_level) for child in node.children]
        return "\n".join(filter(None, html_parts))

    def visit_ElementNode(self, node: ElementNode, indent_level: int) -> str:
        indent = "  " * indent_level

        inline_styles = []
        children_nodes = []
        for child in node.children:
            if isinstance(child, StyleNode):
                for style_child in child.children:
                    if isinstance(style_child, CssPropertyNode):
                        inline_styles.append(self.visit(style_child))
            else:
                children_nodes.append(child)

        attrs = " ".join([self.visit(attr) for attr in node.attributes])
        if inline_styles:
            style_attr = f'style="{" ".join(inline_styles)}"'
            attrs = f"{attrs} {style_attr}".strip()

        if attrs:
            attrs = " " + attrs

        result = f"{indent}<{node.tag_name}{attrs}>"

        if node.tag_name in self.self_closing_tags:
            return result

        content_html = ""
        # Special handling for head to inject styles
        if node.tag_name == 'head' and self.global_styles:
            style_block = "\n".join(self.global_styles)
            style_tag = f"    <style>\n{style_block}\n    </style>"
            content_html += "\n" + style_tag

        if len(children_nodes) == 1 and isinstance(children_nodes[0], TextNode):
            content_html += self.visit(children_nodes[0])
            result += content_html + f"</{node.tag_name}>"
            return result
        elif children_nodes:
            children_html_parts = [self.visit(child, indent_level + 1) for child in children_nodes]
            content_html += "\n" + "\n".join(filter(None, children_html_parts))

        result += content_html + "\n" + indent + f"</{node.tag_name}>"
        return result

    def visit_AttributeNode(self, node: AttributeNode, indent_level: int = 0) -> str:
        return f'{node.name}="{html.escape(node.value)}"'

    def visit_TextNode(self, node: TextNode, indent_level: int = 0) -> str:
        indent = "  " * indent_level if indent_level > 0 else ""
        return f"{indent}{html.escape(node.value)}"

    def visit_CommentNode(self, node: CommentNode, indent_level: int) -> str:
        if node.value.startswith('--'):
            indent = "  " * indent_level
            comment_content = node.value[2:].strip()
            return f"{indent}<!-- {comment_content} -->"
        return ""

    def visit_StyleNode(self, node: StyleNode, indent_level: int = 0):
        pass

    def visit_CssRuleNode(self, node: CssRuleNode, indent_level: int) -> str:
        indent = "  " * indent_level
        props = "\n".join([self.visit(prop, indent_level + 1) for prop in node.properties])
        return f"{indent}{node.selector} {{\n{props}\n{indent}}}"

    def visit_CssPropertyNode(self, node: CssPropertyNode, indent_level: int = 0) -> str:
        indent = "  " * indent_level
        if indent_level == 0:
            return f"{node.name}: {node.value};"
        return f"{indent}{node.name}: {node.value};"

if __name__ == '__main__':
    from CHTL.CHTLLexer.lexer import Lexer
    from CHTL.CHTLParser.parser import Parser

    source = """
    html {
        head {}
        body {
            p { text: "hello"; }
        }
    }
    """

    lexer = Lexer(source)
    tokens = lexer.tokenize()
    parser = Parser(tokens)
    ast = parser.parse()

    generator = HTMLGenerator(ast)
    generated_html = generator.generate()

    print(generated_html)
