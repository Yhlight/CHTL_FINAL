from CHTL.CHTLLexer import Lexer, Token, TokenType
from CHTL.CHTLNode import BaseNode, ElementNode, TextNode, StyleNode

class Parser:
    """
    The Parser for the CHTL language. It takes a lexer and builds an
    Abstract Syntax Tree (AST).
    """
    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.errors = []

        self.current_token: Token = None
        self.peek_token: Token = None
        self._next_token()
        self._next_token()  # Initialize both current_token and peek_token

    def _next_token(self):
        """Advances the token stream."""
        self.current_token = self.peek_token
        self.peek_token = self.lexer.get_next_token()

    def _expect_peek(self, token_type: TokenType) -> bool:
        """
        Asserts the type of the next token and advances if it matches.
        """
        if self.peek_token.type == token_type:
            self._next_token()
            return True
        else:
            self._peek_error(token_type)
            return False

    def _peek_error(self, token_type: TokenType):
        """Adds an error message when a token mismatch occurs."""
        msg = f"Expected next token to be {token_type}, got {self.peek_token.type} instead."
        self.errors.append(msg)

    def parse_program(self) -> ElementNode:
        """
        The main entry point for parsing. A CHTL file is treated as a list
        of statements which will be wrapped in a root node.
        """
        # A program is a series of statements. We'll wrap them in a root node.
        root = ElementNode(tag_name="root")

        while self.current_token.type != TokenType.EOF:
            statement = self.parse_statement()
            if statement:
                root.children.append(statement)
            self._next_token()

        return root

    def parse_statement(self) -> BaseNode | None:
        """
        Parses a single statement. For now, a statement is an element
        declaration.
        """
        if self.current_token.type == TokenType.IDENTIFIER:
            return self.parse_element_or_text_statement()
        return None

    def parse_element_or_text_statement(self) -> ElementNode | TextNode | StyleNode | None:
        """
        Parses an element (e.g., `div {}`), a text node (`text {}`), or a style node (`style {}`).
        """
        identifier = self.current_token.literal

        if self.peek_token.type != TokenType.LBRACE:
            # This could be an attribute or a syntax error, but it's not a block statement.
            return None

        # Don't advance tokens here. Let the sub-parsers do it.
        # This allows _parse_element_body to get the tag_name correctly.

        if identifier.lower() == 'text':
            self._next_token()  # Consume the 'text' identifier
            return self._parse_text_body()
        elif identifier.lower() == 'style':
            self._next_token()  # Consume the 'style' identifier
            return self._parse_style_body()
        else:
            # It's a standard element.
            # We don't consume the identifier here because _parse_element_body needs it.
            return self._parse_element_body(identifier)

    def _parse_element_body(self, tag_name: str) -> ElementNode:
        """Parses the block content of an element node, including attributes and nested elements."""
        node = ElementNode(tag_name=tag_name)

        self._next_token() # Consume '{'

        # Loop until we find the closing brace for the element
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            # Inside a block, we can have attributes or nested elements.
            if self.current_token.type == TokenType.IDENTIFIER:
                # If the next token is a COLON or EQUALS, it's an attribute.
                if self.peek_token.type in (TokenType.COLON, TokenType.EQUALS):
                    self._parse_attribute_statement(node)
                # If the next token is a LBRACE, it's a nested element.
                elif self.peek_token.type == TokenType.LBRACE:
                    child_node = self.parse_element_or_text_statement()
                    if child_node:
                        node.children.append(child_node)
                else:
                    self.errors.append(f"Unexpected token after identifier '{self.current_token.literal}': {self.peek_token.type}")

            # Advance to the next statement/token.
            # If we parsed a statement that consumes tokens, this moves to the next one.
            self._next_token()

        if self.current_token.type != TokenType.RBRACE:
            self.errors.append("Expected '}' to close element block.")

        return node

    def _parse_attribute_statement(self, node: ElementNode):
        """
        Parses a full attribute statement like `key: value;` and adds it to the node.
        Assumes the current token is the attribute's name (IDENTIFIER).
        """
        attr_name = self.current_token.literal

        # Advance past the name and the colon/equals
        self._next_token() # Consume name (current_token is now ':' or '=')
        self._next_token() # Consume ':' or '=' (current_token is now the value)

        # The next token is the value. It can be a STRING or an unquoted literal (IDENTIFIER).
        if self.current_token.type not in (TokenType.STRING, TokenType.IDENTIFIER):
             self.errors.append(f"Expected attribute value for '{attr_name}', got {self.current_token.type}")
             # Attempt to recover by skipping to the next semicolon
             while self.current_token.type not in (TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF):
                 self._next_token()
             return

        attr_value = self.current_token.literal
        node.attributes[attr_name] = attr_value

        # Expect a semicolon to terminate the attribute statement
        if self.peek_token.type != TokenType.SEMICOLON:
            self.errors.append(f"Missing semicolon ';' after attribute value for '{attr_name}'")
        else:
            self._next_token() # Consume the value token, leaving the semicolon as current

    def _skip_to_next_statement(self):
        """Helper for error recovery, skips tokens until a sensible recovery point."""
        while self.current_token.type not in (TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF):
            self._next_token()
        # Consume the recovery token itself to start fresh on the next one
        if self.current_token.type == TokenType.SEMICOLON:
            self._next_token()

    def _parse_style_body(self) -> StyleNode:
        """Parses the block content of a style node, e.g., `{ width: 100px; }`."""
        # Assumes current_token is '{'
        self._next_token() # Consume '{'

        properties = {}
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type != TokenType.IDENTIFIER:
                self.errors.append(f"Expected CSS property name, got {self.current_token.type}")
                self._skip_to_next_statement()
                continue

            prop_name = self.current_token.literal

            if not self._expect_peek(TokenType.COLON):
                self._skip_to_next_statement()
                continue

            self._next_token() # consume colon, current_token is now the start of the value

            value_parts = []
            while self.current_token.type not in (TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF):
                value_parts.append(self.current_token.literal)
                self._next_token()

            if not value_parts:
                self.errors.append(f"Missing value for CSS property '{prop_name}'")

            properties[prop_name] = " ".join(value_parts)

            if self.current_token.type == TokenType.SEMICOLON:
                self._next_token() # Consume semicolon to start the next property
            else:
                self.errors.append(f"Missing semicolon for CSS property '{prop_name}'")

        # The loop terminates when current_token is RBRACE. The calling loop will consume it.
        return StyleNode(properties=properties)

    def _parse_text_body(self) -> TextNode | None:
        """
        Parses the block content of a text node. It can contain either a single
        quoted string or a multi-word unquoted literal.
        """
        self._next_token() # Consume '{'

        value = ""
        # If the content is a single quoted string, just take its value.
        if self.current_token.type == TokenType.STRING:
            value = self.current_token.literal
            self._next_token() # Consume the string token
        else:
            # Otherwise, it's an unquoted literal. Consume tokens and join them
            # with spaces until we hit the closing brace.
            value_parts = []
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                value_parts.append(self.current_token.literal)
                self._next_token()
            value = " ".join(value_parts)

        # After parsing the content, we must be at a closing brace.
        if self.current_token.type != TokenType.RBRACE:
            # This error is now more robust.
            self.errors.append("Expected '}' to close text block.")

        return TextNode(value=value)
