from CHTL.CHTLLexer import Lexer, Token, TokenType
from CHTL.CHTLNode import BaseNode, ElementNode, TextNode

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

    def parse_element_or_text_statement(self) -> ElementNode | TextNode | None:
        """
        Parses an element (e.g., `div {}`) or a text node (`text {}`).
        """
        identifier = self.current_token.literal

        if self.peek_token.type != TokenType.LBRACE:
            # This could be an unquoted literal later, but for now it's an element
            return None

        self._next_token() # Consume identifier, current_token is now LBRACE

        if identifier.lower() == 'text':
            return self._parse_text_body()
        else:
            return self._parse_element_body(identifier)

    def _parse_element_body(self, tag_name: str) -> ElementNode:
        """Parses the block content of an element node."""
        node = ElementNode(tag_name=tag_name)

        self._next_token() # Consume '{'

        # Parse nested statements
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            statement = self.parse_statement()
            if statement:
                node.children.append(statement)
            self._next_token()

        if self.current_token.type != TokenType.RBRACE:
            self.errors.append("Expected '}' to close element block.")

        return node

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
