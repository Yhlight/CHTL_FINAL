from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node
)
import html

class HTMLGenerator:
    def __init__(self, ast: DocumentNode):
        self.ast = ast
        # From https://developer.mozilla.org/en-US/docs/Glossary/Empty_element
        self.self_closing_tags = {
            'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
            'link', 'meta', 'param', 'source', 'track', 'wbr'
        }

    def generate(self) -> str:
        """Generates the final HTML string from the AST."""
        return self.visit(self.ast)

    def visit(self, node: Node, indent_level: int = 0) -> str:
        """Dispatches to the appropriate visitor method for the given node type."""
        method_name = f'visit_{type(node).__name__}'
        visitor = getattr(self, method_name, self.generic_visit)
        return visitor(node, indent_level)

    def generic_visit(self, node: Node, indent_level: int):
        raise Exception(f'No visit_{type(node).__name__} method')

    def visit_DocumentNode(self, node: DocumentNode, indent_level: int) -> str:
        html_parts = [self.visit(child, indent_level) for child in node.children]
        return "\n".join(html_parts)

    def visit_ElementNode(self, node: ElementNode, indent_level: int) -> str:
        indent = "  " * indent_level

        # Build attributes string
        attrs = " ".join([self.visit(attr) for attr in node.attributes])
        if attrs:
            attrs = " " + attrs

        # Opening tag
        result = f"{indent}<{node.tag_name}{attrs}>"

        if node.tag_name in self.self_closing_tags:
            return result

        # Children
        if node.children:
            # If the only child is a text node, don't add newlines
            if len(node.children) == 1 and isinstance(node.children[0], TextNode):
                 result += self.visit(node.children[0], 0) # no indent for inline text
            else:
                result += "\n"
                children_html = [self.visit(child, indent_level + 1) for child in node.children]
                result += "\n".join(filter(None, children_html)) # Filter out empty strings from ignored comments
                result += f"\n{indent}"

        # Closing tag
        result += f"</{node.tag_name}>"
        return result

    def visit_AttributeNode(self, node: AttributeNode, indent_level: int = 0) -> str:
        # Attribute values should be escaped
        return f'{node.name}="{html.escape(node.value)}"'

    def visit_TextNode(self, node: TextNode, indent_level: int) -> str:
        indent = "  " * indent_level
        # Text content should also be escaped
        return f"{indent}{html.escape(node.value)}"

    def visit_CommentNode(self, node: CommentNode, indent_level: int) -> str:
        if node.value.startswith('--'):
            indent = "  " * indent_level
            # Remove '--' and trim whitespace
            comment_content = node.value[2:].strip()
            return f"{indent}<!-- {comment_content} -->"
        # Ignore '//' and '/*' comments
        return ""

if __name__ == '__main__':
    from CHTL.CHTLLexer.lexer import Lexer
    from CHTL.CHTLParser.parser import Parser

    source = """
    html {
        head {
            title: My CHTL Page with < & >;
        }
        body {
            -- A generator comment
            div {
                id: main_content;
                class = "container";
                text { Hello "World" from CHTL! }
            }
            br;
            // This comment will be ignored
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
