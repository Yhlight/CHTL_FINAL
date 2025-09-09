from CHTL.CHTLLexer.lexer import Token, TokenType
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node
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
        # Strip whitespace from the raw comment token value for a clean AST node
        cleaned_value = token.value.strip()
        return CommentNode(value=cleaned_value, lineno=token.lineno)

    def parse_value_tokens(self, terminators: List[TokenType]) -> str:
        """Parses a value which can be a single string or a series of unquoted literals."""
        if self.current_token().type == TokenType.STRING:
            token = self.consume(TokenType.STRING)
            return token.value[1:-1]  # Remove quotes

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
            return element # Self-closing tag like <br/>

        self.consume(TokenType.LBRACE)

        while self.current_token() and self.current_token().type not in (TokenType.RBRACE, TokenType.EOF):
            token = self.current_token()
            if token.type == TokenType.COMMENT:
                element.children.append(self.parse_comment())
                continue

            if token.type != TokenType.IDENTIFIER:
                raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') inside element '{element.tag_name}'")

            # Lookahead to see if it's an attribute or a child element
            next_token = self.peek(1)
            if next_token and next_token.type in (TokenType.COLON, TokenType.EQUALS):
                attr_name_token = self.consume(TokenType.IDENTIFIER)

                # Special handling for 'title' and 'text' shorthands
                if attr_name_token.value in ('title', 'text'):
                    self.consume() # Consume ':' or '='
                    value = self.parse_value_tokens([TokenType.SEMICOLON])
                    self.consume(TokenType.SEMICOLON)

                    if attr_name_token.value == 'title':
                        title_element = ElementNode(tag_name='title', lineno=attr_name_token.lineno)
                        title_element.children.append(TextNode(value=value, lineno=attr_name_token.lineno))
                        element.children.append(title_element)
                    else: # 'text'
                        element.children.append(TextNode(value=value, lineno=attr_name_token.lineno))
                else:
                    # It's a regular attribute
                    self.consume() # Consume ':' or '='
                    value = self.parse_value_tokens([TokenType.SEMICOLON])
                    self.consume(TokenType.SEMICOLON)
                    element.attributes.append(AttributeNode(name=attr_name_token.value, value=value, lineno=attr_name_token.lineno))

            elif next_token and next_token.type == TokenType.LBRACE:
                if token.value == 'text':
                    self.consume(TokenType.IDENTIFIER) # consume 'text'
                    self.consume(TokenType.LBRACE)
                    value = self.parse_value_tokens([TokenType.RBRACE])
                    self.consume(TokenType.RBRACE)
                    element.children.append(TextNode(value=value, lineno=token.lineno))
                else:
                    element.children.append(self.parse_element())
            else:
                 element.children.append(self.parse_element())

        self.consume(TokenType.RBRACE)
        return element

if __name__ == '__main__':
    from CHTL.CHTLLexer.lexer import Lexer
    import json

    source = """
    html {
        head {
            title: My CHTL Page;
        }
        body {
            -- A generator comment
            div {
                id: main_content;
                class = "container";
                text { Hello World from CHTL! }
            }
            br;
        }
    }
    """

    lexer = Lexer(source)
    tokens = lexer.tokenize()
    parser = Parser(tokens)
    ast = parser.parse()

    class AstEncoder(json.JSONEncoder):
        def default(self, o):
            if isinstance(o, (BaseNode, DocumentNode, ElementNode, TextNode, AttributeNode, CommentNode)):
                return vars(o)
            return super().default(o)

    print(json.dumps(ast, cls=AstEncoder, indent=2))
