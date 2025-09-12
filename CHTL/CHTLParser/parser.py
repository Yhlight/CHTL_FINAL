from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLNode import nodes

class ParseError(Exception): pass

class Parser:
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> nodes.ProgramNode:
        program = nodes.ProgramNode()
        while not self._is_at_end():
            node = self._declaration()
            if node: program.children.append(node)
        return program

    def _declaration(self):
        if self._peek().type == TokenType.IDENTIFIER:
            return self._element_statement()
        if self._peek().type == TokenType.EOF:
            return None
        # Basic error recovery
        self._advance()
        return None

    def _element_statement(self):
        tag_name = self._consume(TokenType.IDENTIFIER, "element tag name").lexeme
        element = nodes.ElementNode(tag_name=tag_name)

        if not self._match(TokenType.LBRACE):
            self._consume(TokenType.SEMICOLON, "Expect '{' or ';' after element name.")
            return element

        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.TEXT):
                element.children.append(self._text_statement())
            elif self._is_attribute():
                element.attributes.append(self._attribute())
            elif self._check(TokenType.IDENTIFIER):
                element.children.append(self._element_statement())
            else:
                self._error(self._peek(), "Unexpected token inside element block.")
                self._advance() # Skip unexpected token

        self._consume(TokenType.RBRACE, "}")
        return element

    def _text_statement(self):
        self._consume(TokenType.TEXT, "'text'")
        self._consume(TokenType.LBRACE, "{")
        token = self._consume(TokenType.STRING, "string literal for text content")
        value = token.lexeme
        self._consume(TokenType.RBRACE, "}")
        return nodes.TextNode(value=value)

    def _attribute(self):
        name = self._consume(TokenType.IDENTIFIER, "attribute name").lexeme
        if not self._match(TokenType.EQUALS, TokenType.COLON):
            raise self._error(self._peek(), "Expect '=' or ':' for attribute assignment.")

        if self._match(TokenType.STRING, TokenType.IDENTIFIER):
            value = self._previous().lexeme
        else:
            raise self._error(self._peek(), "Expect string or identifier for attribute value.")

        self._consume(TokenType.SEMICOLON, "';'")
        return nodes.AttributeNode(name=name, value=value)

    def _is_attribute(self):
        return self._check(TokenType.IDENTIFIER) and self._peek(1).type in [TokenType.COLON, TokenType.EQUALS]

    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype): return self._advance()
        raise self._error(self._peek(), f"Expect {message}")

    def _check(self, ttype: TokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == ttype

    def _is_at_end(self, offset=0) -> bool:
        return self.tokens[self.current + offset].type == TokenType.EOF

    def _peek(self, offset=0) -> Token:
        return self.tokens[self.current + offset]

    def _previous(self) -> Token:
        return self.tokens[self.current - 1]

    def _advance(self) -> Token:
        if not self._is_at_end(): self.current += 1
        return self._previous()

    def _match(self, *types: TokenType) -> bool:
        for ttype in types:
            if self._check(ttype):
                self._advance()
                return True
        return False

    def _error(self, token: Token, message: str) -> ParseError:
        # For now, just print the error and allow parsing to continue
        print(f"[Line {token.line}] Error at '{token.lexeme}': {message}")
        return ParseError()
