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
        program = nodes.ProgramNode()
        while not self._is_at_end():
            declaration = self._declaration()
            if declaration:
                program.children.append(declaration)
        return program

    def _declaration(self):
        # In this simplified version, all top-level constructs start with an IDENTIFIER
        # or a keyword that is tokenized as such (e.g. 'text', 'style').
        if self._check(TokenType.IDENTIFIER) or self._check(TokenType.TEXT) or self._check(TokenType.STYLE):
             return self._statement()

        if self._match(TokenType.SEMICOLON):
            return None

        if self._check(TokenType.EOF):
            return None

        raise self._error(self._peek(), "Expected a statement, element, or component definition.")

    def _statement(self):
        if self._peek().type == TokenType.TEXT:
            return self._text_node_statement()
        # The parser no longer needs to distinguish style statements here,
        # as they only appear inside element blocks.
        return self._element_statement()

    def _text_node_statement(self):
        self._consume(TokenType.TEXT, "Expect 'text' keyword.")
        self._consume(TokenType.LBRACE, "Expect '{' after 'text'.")

        if self._match(TokenType.STRING):
            value = self._previous().lexeme
        else:
            # Unquoted text can be made of multiple tokens (e.g. numbers and identifiers)
            value_parts = []
            while not self._check(TokenType.RBRACE) and not self._is_at_end():
                value_parts.append(self._advance().lexeme)
            value = " ".join(value_parts)

        self._consume(TokenType.RBRACE, "Expect '}' after text node value.")
        return nodes.TextNode(value=value)

    def _element_statement(self):
        tag_name = self._consume(TokenType.IDENTIFIER, "Expect element tag name.").lexeme
        self._consume(TokenType.LBRACE, "Expect '{' after element tag name.")

        attributes = []
        children = []

        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.STYLE):
                children.append(self._style_statement())
            elif self._is_attribute():
                attributes.append(self._attribute())
            elif self._check(TokenType.IDENTIFIER) or self._check(TokenType.TEXT):
                children.append(self._statement())
            else:
                raise self._error(self._peek(), "Unexpected token inside element block.")

        self._consume(TokenType.RBRACE, "Expect '}' after element block.")
        return nodes.ElementNode(tag_name=tag_name, attributes=attributes, children=children)

    def _style_statement(self):
        self._consume(TokenType.STYLE, "Expect 'style' keyword.")
        self._consume(TokenType.LBRACE, "Expect '{' after 'style'.")
        style_node = nodes.StyleNode()
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            # A selector is just an IDENTIFIER now, thanks to the new lexer.
            # A property also starts with an IDENTIFIER. We look ahead for a colon.
            if self._peek_at(1).type == TokenType.COLON:
                style_node.children.append(self._style_property())
            else:
                style_node.children.append(self._style_selector_rule())
        self._consume(TokenType.RBRACE, "Expect '}' after style block.")
        return style_node

    def _style_property(self):
        name = self._consume(TokenType.IDENTIFIER, "Expect property name.").lexeme
        self._consume(TokenType.COLON, "Expect ':' after property name.")
        value_parts = []
        while not self._check(TokenType.SEMICOLON) and not self._is_at_end():
            value_parts.append(self._advance().lexeme)
        value = " ".join(value_parts).strip()
        self._consume(TokenType.SEMICOLON, "Expect ';' after property value.")
        return nodes.StylePropertyNode(name=name, value=value)

    def _style_selector_rule(self):
        selector = self._consume(TokenType.IDENTIFIER, "Expect selector.").lexeme
        self._consume(TokenType.LBRACE, "Expect '{' after selector.")
        properties = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            properties.append(self._style_property())
        self._consume(TokenType.RBRACE, "Expect '}' after selector rule block.")
        return nodes.StyleSelectorRuleNode(selector=selector, properties=properties)

    def _attribute(self):
        name = self._consume(TokenType.IDENTIFIER, "Expect attribute name.").lexeme
        self._consume(TokenType.EQUALS, "Expect '=' or ':' after attribute name.")
        value = self._consume(TokenType.STRING, "Expect string literal for attribute value.").lexeme
        self._consume(TokenType.SEMICOLON, "Expect ';' after attribute value.")
        return nodes.AttributeNode(name=name, value=value)

    def _is_attribute(self):
        # A simple attribute is IDENTIFIER = STRING;
        # The CHTL spec also allows IDENTIFIER : IDENTIFIER/STRING
        # The new lexer makes this distinction harder, so we simplify for now.
        # Let's assume attribute is IDENTIFIER = or :
        return self._check(TokenType.IDENTIFIER) and self._peek_at(1).type in [TokenType.COLON, TokenType.EQUALS]

    # --- Helpers ---
    def _match(self, *types: TokenType) -> bool:
        for ttype in types:
            if self._check(ttype): self._advance(); return True
        return False
    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype): return self._advance()
        raise self._error(self._peek(), message)
    def _check(self, ttype: TokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == ttype
    def _advance(self) -> Token:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _is_at_end(self) -> bool: return self._peek().type == TokenType.EOF
    def _peek(self) -> Token: return self.tokens[self.current]
    def _peek_at(self, offset: int) -> Token:
        if self.current + offset >= len(self.tokens): return self.tokens[-1]
        return self.tokens[self.current + offset]
    def _previous(self) -> Token: return self.tokens[self.current - 1]
    def _error(self, token: Token, message: str) -> ParseError:
        err_msg = f"[Line {token.line}, Col {token.col}] Error at '{token.lexeme}': {message}"
        return ParseError(err_msg)
