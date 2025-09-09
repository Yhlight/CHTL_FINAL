from CHTL.CHTLLexer.lexer import Token, TokenType
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode
)
from typing import List, Optional

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def peek(self, offset: int = 0) -> Optional[Token]:
        if self.pos + offset < len(self.tokens):
            return self.tokens[self.pos + offset]
        return None

    def current_token(self) -> Optional[Token]:
        return self.peek(0)

    def consume(self, expected_type: Optional[TokenType] = None) -> Token:
        token = self.current_token()
        if not token:
            raise RuntimeError("Unexpected end of token stream.")

        if expected_type and token.type != expected_type:
            raise RuntimeError(f"Line {token.lineno}: Expected token {expected_type}, but got {token.type} ('{token.value}')")

        self.pos += 1
        return token

    def parse(self) -> DocumentNode:
        doc = DocumentNode(lineno=1)
        while self.current_token() and self.current_token().type != TokenType.EOF:
            if self.current_token().type == TokenType.COMMENT:
                doc.children.append(self.parse_comment())
            elif self.current_token().type == TokenType.IDENTIFIER:
                doc.children.append(self.parse_element())
            else:
                token = self.current_token()
                raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} at top level.")
        return doc

    def parse_comment(self) -> CommentNode:
        token = self.consume(TokenType.COMMENT)
        cleaned_value = token.value.strip()
        return CommentNode(value=cleaned_value, lineno=token.lineno)

    def parse_value_tokens(self, terminators: List[TokenType]) -> str:
        if self.current_token().type == TokenType.STRING:
            token = self.consume(TokenType.STRING)
            return token.value[1:-1]

        value_parts = []
        while self.current_token() and self.current_token().type not in terminators:
            token = self.consume()
            value_parts.append(token.value)
        return " ".join(value_parts).strip()

    def parse_element(self) -> ElementNode:
        tag_token = self.consume(TokenType.IDENTIFIER)
        element = ElementNode(tag_name=tag_token.value, lineno=tag_token.lineno)

        if self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
            return element

        self.consume(TokenType.LBRACE)

        while self.current_token() and self.current_token().type not in (TokenType.RBRACE, TokenType.EOF):
            token = self.current_token()
            next_token = self.peek(1)

            if token.type == TokenType.COMMENT:
                element.children.append(self.parse_comment())
            elif token.type == TokenType.IDENTIFIER and token.value == 'style':
                element.children.append(self.parse_style_block(element))
            elif token.type == TokenType.IDENTIFIER and next_token and next_token.type == TokenType.LBRACE:
                if token.value == 'text':
                    self.consume() # consume 'text'
                    self.consume() # consume '{'
                    value = self.parse_value_tokens([TokenType.RBRACE])
                    self.consume(TokenType.RBRACE)
                    element.children.append(TextNode(value=value, lineno=token.lineno))
                else:
                    element.children.append(self.parse_element())
            elif token.type == TokenType.IDENTIFIER and next_token and next_token.type in (TokenType.COLON, TokenType.EQUALS):
                self.parse_attribute(element)
            elif token.type == TokenType.IDENTIFIER:
                element.children.append(self.parse_element())
            else:
                raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') inside element '{element.tag_name}'")

        self.consume(TokenType.RBRACE)
        return element

    def parse_attribute(self, element: ElementNode):
        attr_name_token = self.consume(TokenType.IDENTIFIER)
        self.consume() # Consume ':' or '='
        value = self.parse_value_tokens([TokenType.SEMICOLON])
        self.consume(TokenType.SEMICOLON)

        if attr_name_token.value == 'text':
            element.children.append(TextNode(value=value, lineno=attr_name_token.lineno))
        elif attr_name_token.value == 'title':
            title_element = ElementNode(tag_name='title', lineno=attr_name_token.lineno)
            title_element.children.append(TextNode(value=value, lineno=attr_name_token.lineno))
            element.children.append(title_element)
        else:
            element.attributes.append(AttributeNode(name=attr_name_token.value, value=value, lineno=attr_name_token.lineno))

    def parse_style_block(self, parent_element: ElementNode) -> StyleNode:
        style_token = self.consume(TokenType.IDENTIFIER)
        self.consume(TokenType.LBRACE)
        style_node = StyleNode(lineno=style_token.lineno)

        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            is_rule = False
            i = 0
            while True:
                peek_token = self.peek(i)
                if not peek_token or peek_token.type in (TokenType.SEMICOLON, TokenType.RBRACE):
                    is_rule = False
                    break
                if peek_token.type == TokenType.LBRACE:
                    is_rule = True
                    break
                i += 1

            if is_rule:
                rule = self.parse_css_rule(parent_element)
                style_node.children.append(rule)
            else:
                prop = self.parse_css_property()
                style_node.children.append(prop)

        self.consume(TokenType.RBRACE)
        return style_node

    def parse_css_property(self) -> CssPropertyNode:
        name_token = self.consume(TokenType.IDENTIFIER)
        self.consume(TokenType.COLON)
        value = self.parse_value_tokens([TokenType.SEMICOLON])
        self.consume(TokenType.SEMICOLON)
        return CssPropertyNode(name=name_token.value, value=value, lineno=name_token.lineno)

    def parse_css_rule(self, parent_element: ElementNode) -> CssRuleNode:
        selector_parts = []
        rule_lineno = self.current_token().lineno
        while self.current_token() and self.current_token().type != TokenType.LBRACE:
            selector_parts.append(self.consume().value)

        selector = "".join(selector_parts).strip()

        if selector.startswith('.'):
            class_name = selector.split(':')[0].split(' ')[0][1:]
            found = False
            for attr in parent_element.attributes:
                if attr.name == 'class':
                    if class_name not in attr.value.split():
                        attr.value += f" {class_name}"
                    found = True
                    break
            if not found:
                parent_element.attributes.append(AttributeNode(name='class', value=class_name))
        elif selector.startswith('#'):
            id_name = selector.split(':')[0].split(' ')[0][1:]
            found = False
            for attr in parent_element.attributes:
                if attr.name == 'id':
                    attr.value = id_name
                    found = True
                    break
            if not found:
                parent_element.attributes.append(AttributeNode(name='id', value=id_name))

        rule_node = CssRuleNode(selector=selector, lineno=rule_lineno)
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            rule_node.properties.append(self.parse_css_property())
        self.consume(TokenType.RBRACE)
        return rule_node

if __name__ == '__main__':
    from CHTL.CHTLLexer.lexer import Lexer
    import json

    source = """
    div {
        class: "old-class";
        style {
            width: 100px;
            .box {
                color: blue;
            }
            #my-id {
                font-weight: bold;
            }
        }
    }
    """

    lexer = Lexer(source)
    tokens = lexer.tokenize()
    parser = Parser(tokens)
    ast = parser.parse()

    class AstEncoder(json.JSONEncoder):
        def default(self, o):
            if isinstance(o, (BaseNode, DocumentNode, ElementNode, TextNode, AttributeNode, CommentNode, StyleNode, CssRuleNode, CssPropertyNode)):
                return vars(o)
            return super().default(o)

    print(json.dumps(ast, cls=AstEncoder, indent=2))
