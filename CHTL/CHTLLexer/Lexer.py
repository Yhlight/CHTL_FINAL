from .Token import Token, TokenType

class Lexer:
    """
    The Lexer for the CHTL language. It takes source text and turns it into a stream of tokens.
    """
    def __init__(self, text: str):
        self.text = text
        self.pos = 0
        self.current_char = self.text[self.pos] if self.pos < len(self.text) else None

    def _advance(self):
        """Moves the read position forward by one character."""
        self.pos += 1
        if self.pos >= len(self.text):
            self.current_char = None
        else:
            self.current_char = self.text[self.pos]

    def _peek(self) -> str | None:
        """Looks at the next character without consuming it."""
        peek_pos = self.pos + 1
        if peek_pos >= len(self.text):
            return None
        return self.text[peek_pos]

    def _skip_whitespace(self):
        """Skips over any whitespace characters."""
        while self.current_char is not None and self.current_char.isspace():
            self._advance()

    def _skip_comment(self):
        """Skips over comments. Supports // and /* */."""
        if self.current_char == '/' and self._peek() == '/':
            # Single-line comment
            while self.current_char is not None and self.current_char != '\n':
                self._advance()
            return True
        if self.current_char == '/' and self._peek() == '*':
            # Multi-line comment
            self._advance()  # /
            self._advance()  # *
            while self.current_char is not None:
                if self.current_char == '*' and self._peek() == '/':
                    self._advance()  # *
                    self._advance()  # /
                    break
                self._advance()
            return True
        # Note: '--' comments are not skipped, they need to be tokenized.
        return False

    def _identifier(self) -> Token:
        """
        Parses an identifier or an unquoted literal. Allows for hyphens,
        which are common in CSS class names and attributes.
        """
        start_pos = self.pos
        result = ''
        # This will also tokenize unquoted literals like `red` or `content-box`.
        while self.current_char is not None and (self.current_char.isalnum() or self.current_char == '-'):
            result += self.current_char
            self._advance()
        return Token(TokenType.IDENTIFIER, result, start_pos, self.pos)

    def _hex_literal(self) -> Token:
        """Parses a hex color code literal, e.g., #ff0000."""
        start_pos = self.pos
        self._advance() # Consume '#'
        while self.current_char is not None and self.current_char.isalnum():
            self._advance()
        literal = self.text[start_pos:self.pos]
        return Token(TokenType.HEX_LITERAL, literal, start_pos, self.pos)

    def _string(self, quote_type: str) -> Token:
        """Parses a quoted string literal."""
        start_pos = self.pos
        self._advance()  # Consume the opening quote
        str_start_pos = self.pos
        while self.current_char is not None and self.current_char != quote_type:
            self._advance()

        if self.current_char is None:
            return Token(TokenType.ILLEGAL, self.text[start_pos:], start_pos, self.pos)

        literal = self.text[str_start_pos:self.pos]
        self._advance()  # Consume the closing quote
        return Token(TokenType.STRING, literal, start_pos, self.pos)

    def get_next_token(self) -> Token:
        """
        Get the next token from the input stream. This is the main entry point.
        """
        while self.current_char is not None:
            self._skip_whitespace()
            if self._skip_comment():
                continue

            if self.current_char is None:
                break

            start_pos = self.pos

            if self.current_char.isalpha() or self.current_char.isdigit():
                return self._identifier()

            if self.current_char in ('"', "'"):
                return self._string(self.current_char)

            if self.current_char == '#':
                # Check if it's a hex color code or just a HASH token
                if self._peek() and self._peek().isalnum():
                    return self._hex_literal()
                # Fallthrough to the token_map if it's just a standalone '#'

            if self.current_char == '*':
                if self._peek() == '*':
                    self._advance()
                    self._advance()
                    return Token(TokenType.DOUBLE_ASTERISK, "**", start_pos, self.pos)
                # Fallthrough to the token_map for a single '*'

            token_map = {
                '{': TokenType.LBRACE,
                '}': TokenType.RBRACE,
                '[': TokenType.LBRACKET,
                ']': TokenType.RBRACKET,
                '(': TokenType.LPAREN,
                ')': TokenType.RPAREN,
                ':': TokenType.COLON,
                '=': TokenType.EQUALS,
                ';': TokenType.SEMICOLON,
                '@': TokenType.AT,
                '.': TokenType.DOT,
                '#': TokenType.HASH,
                '&': TokenType.AMPERSAND,
                '>': TokenType.GT,
                '+': TokenType.PLUS,
                '-': TokenType.MINUS,
                '*': TokenType.ASTERISK,
                '/': TokenType.SLASH,
                '%': TokenType.PERCENT,
            }
            if self.current_char in token_map:
                token_type = token_map[self.current_char]
                char = self.current_char
                self._advance()
                return Token(token_type, char, start_pos, self.pos)

            # If we haven't matched anything, it's an illegal character.
            char = self.current_char
            self._advance()
            return Token(TokenType.ILLEGAL, char, start_pos, self.pos)

        return Token(TokenType.EOF, "", self.pos, self.pos)
