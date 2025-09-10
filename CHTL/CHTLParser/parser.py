from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLNode import nodes

class ParseError(Exception):
    pass

class Parser:
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> nodes.ProgramNode:
        """Parses the token stream and returns the root of the AST."""
        program = nodes.ProgramNode()
        while not self._is_at_end():
            program.children.append(self._declaration())
        return program

    def _declaration(self):
        """Parses a top-level declaration, which is typically an element."""
        # This can be expanded later for imports, templates, etc.
        if self._check(TokenType.IDENTIFIER) or self._check(TokenType.TEXT):
             return self._statement()

        # Skip any semicolons that might be at the top level
        if self._match(TokenType.SEMICOLON):
            return None # Or a special Node type if we need to preserve them

        raise self._error(self._peek(), "Expected a statement.")

    def _statement(self):
        """Parses a statement, which can be an element or a text node."""
        if self._peek().type == TokenType.TEXT:
            return self._text_node_statement()
        return self._element_statement()

    def _text_node_statement(self):
        """Parses a text node: text { "value" } or text { unquoted value }"""
        self._consume(TokenType.TEXT, "Expect 'text' keyword.")
        self._consume(TokenType.LBRACE, "Expect '{' after 'text'.")

        # Handle a single quoted string literal
        if self._match(TokenType.STRING):
            value = self._previous().lexeme
        # Handle unquoted literal, which can be multiple tokens
        else:
            value_parts = []
            while not self._check(TokenType.RBRACE) and not self._is_at_end():
                value_parts.append(self._advance().lexeme)
            value = " ".join(value_parts)

        self._consume(TokenType.RBRACE, "Expect '}' after text node value.")
        return nodes.TextNode(value=value)

    def _element_statement(self):
        """Parses an element: tag { attributes... children... }"""
        tag_name = self._consume(TokenType.IDENTIFIER, "Expect element tag name.").lexeme
        self._consume(TokenType.LBRACE, "Expect '{' after element tag name.")

        attributes = []
        children = []

        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            # Is it an attribute or a child element?
            # Attribute: IDENTIFIER : or =
            # Child element: IDENTIFIER { or text {
            if self._is_attribute():
                attributes.append(self._attribute())
            else:
                children.append(self._declaration())

        self._consume(TokenType.RBRACE, "Expect '}' after element block.")
        return nodes.ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def _attribute(self):
        """Parses an attribute: name: value;"""
        name = self._consume(TokenType.IDENTIFIER, "Expect attribute name.").lexeme

        if not self._match(TokenType.COLON, TokenType.EQUALS):
            raise self._error(self._peek(), "Expect ':' or '=' after attribute name.")

        if self._match(TokenType.STRING, TokenType.IDENTIFIER):
            value = self._previous().lexeme
        else:
            raise self._error(self._peek(), "Expect attribute value.")

        self._consume(TokenType.SEMICOLON, "Expect ';' after attribute value.")
        return nodes.AttributeNode(name=name, value=value)

    def _is_attribute(self):
        """
        Checks if the current token sequence looks like an attribute `IDENTIFIER :`
        without consuming the tokens.
        """
        return self._check(TokenType.IDENTIFIER) and self._peek_at(1).type in [TokenType.COLON, TokenType.EQUALS]

    # Helper methods for token navigation
    def _match(self, *types: TokenType) -> bool:
        for ttype in types:
            if self._check(ttype):
                self._advance()
                return True
        return False

    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype):
            return self._advance()
        raise self._error(self._peek(), message)

    def _check(self, ttype: TokenType) -> bool:
        if self._is_at_end():
            return False
        return self._peek().type == ttype

    def _advance(self) -> Token:
        if not self._is_at_end():
            self.current += 1
        return self._previous()

    def _is_at_end(self) -> bool:
        return self._peek().type == TokenType.EOF

    def _peek(self) -> Token:
        return self.tokens[self.current]

    def _peek_at(self, offset: int) -> Token:
        """Looks ahead a certain offset from the current token."""
        if self.current + offset >= len(self.tokens):
            return self.tokens[-1] # Return EOF token
        return self.tokens[self.current + offset]

    def _previous(self) -> Token:
        return self.tokens[self.current - 1]

    def _error(self, token: Token, message: str) -> ParseError:
        # In a real compiler, you'd have a more robust error reporting system.
        err_msg = f"[Line {token.line}, Col {token.col}] Error"
        if token.type == TokenType.EOF:
            err_msg += " at end"
        else:
            err_msg += f" at '{token.lexeme}'"

        err_msg += f": {message}"
        return ParseError(err_msg)
