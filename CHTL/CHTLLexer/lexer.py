from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType

class Lexer:
    """A robust, state-machine-based lexer for the CHTL language."""

    def __init__(self, source: str):
        self.source = source
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1

        self.keywords = {
            "text": TokenType.TEXT,
            "style": TokenType.STYLE,
        }

    def scan_tokens(self) -> list[Token]:
        """Scans the source code and returns a list of tokens."""
        while not self._is_at_end():
            self.start = self.current
            self._scan_token()

        self.tokens.append(Token(TokenType.EOF, "", self.line, self.start))
        return self.tokens

    def _scan_token(self):
        """The main entry point for the state machine.
        This is a simpler implementation of a state machine, where the state is
        implicit in the method called. The key is that each method has its own
        set of rules for what terminates the token."""
        char = self._advance()

        if char in ' \r\t': return
        if char == '\n': self.line += 1; return

        if char == '{': self._add_token(TokenType.LBRACE); return
        if char == '}': self._add_token(TokenType.RBRACE); return
        if char == ';': self._add_token(TokenType.SEMICOLON); return
        if char == '=': self._add_token(TokenType.EQUALS); return
        if char == ':': self._add_token(TokenType.COLON); return
        if char == '"' or char == "'": self._string(char); return

        if char == '/':
            if self._match('/'):
                while self._peek() != '\n' and not self._is_at_end(): self._advance()
            elif self._match('*'):
                self._block_comment()
            else:
                # If not a comment, it's part of a path or other identifier.
                # We back up and let the generic identifier scanner handle it.
                self.current -= 1
                self._scan_generic_identifier()
            return

        # Back up one character so the scanning methods can see the first char
        self.current -= 1

        # This is the core logic: decide which type of "word" it is based on the first char.
        first_char = self._peek()
        if self._is_digit(first_char):
            self._scan_number()
        elif first_char in '.#&' or (first_char == ':' and self.tokens[-1].type == TokenType.IDENTIFIER):
             # A ':' following an identifier is likely a pseudo-selector
            self._scan_selector_or_pseudo()
        elif self._is_alpha(first_char):
            self._scan_keyword_or_property()
        else:
            # Fallback for anything else (e.g. %)
             self._scan_generic_identifier()

    def _string(self, quote_type: str):
        while self._peek() != quote_type and not self._is_at_end():
            if self._peek() == '\n': self.line += 1
            self._advance()
        if self._is_at_end(): return
        self._advance()
        self._add_token(TokenType.STRING)

    def _block_comment(self):
        while not (self._peek() == '*' and self._peek_next() == '/') and not self._is_at_end():
            if self._peek() == '\n': self.line += 1
            self._advance()
        if not self._is_at_end():
            self._advance(); self._advance()

    def _scan_number(self):
        self._advance() # consume first digit
        while self._is_digit(self._peek()): self._advance()
        if self._peek() == '.' and self._is_digit(self._peek_next()):
            self._advance()
            while self._is_digit(self._peek()): self._advance()
        self._add_token(TokenType.NUMBER)

    def _scan_selector_or_pseudo(self):
        self._advance() # consume first char
        # A selector can contain almost anything until a space or opening brace
        while self._peek() not in ' \r\t\n{' and not self._is_at_end():
            self._advance()
        self._add_token(TokenType.IDENTIFIER)

    def _scan_keyword_or_property(self):
        self._advance() # consume first char
        # A property name is typically alphanumeric with hyphens
        while self._is_alpha(self._peek()) or self._is_digit(self._peek()) or self._peek() == '-':
            self._advance()
        text = self.source[self.start:self.current]
        token_type = self.keywords.get(text, TokenType.IDENTIFIER)
        self._add_token(token_type)

    def _scan_generic_identifier(self):
        self._advance() # consume first char
        # e.g., for units like 'px', '%', or paths
        while self._peek() not in ' \r\t\n{;:' and not self._is_at_end():
            self._advance()
        self._add_token(TokenType.IDENTIFIER)

    # --- Helpers ---
    def _is_digit(self, char: str) -> bool: return '0' <= char <= '9'
    def _is_alpha(self, char: str) -> bool: return ('a' <= char <= 'z') or ('A' <= char <= 'Z') or char == '_'
    def _is_at_end(self) -> bool: return self.current >= len(self.source)
    def _advance(self) -> str:
        char = self.source[self.current]; self.current += 1; return char
    def _match(self, expected: str) -> bool:
        if self._is_at_end() or self.source[self.current] != expected: return False
        self.current += 1; return True
    def _peek(self) -> str: return '\0' if self._is_at_end() else self.source[self.current]
    def _peek_next(self) -> str: return '\0' if self.current + 1 >= len(self.source) else self.source[self.current + 1]
    def _add_token(self, type: TokenType, literal: object = None):
        text = self.source[self.start:self.current]
        self.tokens.append(Token(type, text if literal is None else literal, self.line, self.start))
