from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType

class Lexer:
    def __init__(self, source: str):
        self.source = source
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1
        self.col = 1

    def scan_tokens(self) -> list[Token]:
        while not self._is_at_end():
            self.start = self.current
            self._scan_token()

        self.tokens.append(Token(TokenType.EOF, "", self.line, self.col))
        return self.tokens

    def _scan_token(self):
        char = self._advance()

        if char in ' \r\t': return
        if char == '\n':
            self.line += 1
            self.col = 1
            return

        if char == '{': self._add_token(TokenType.LBRACE)
        elif char == '}': self._add_token(TokenType.RBRACE)
        elif char == ':': self._add_token(TokenType.COLON)
        elif char == '=': self._add_token(TokenType.EQUALS)
        elif char == ';': self._add_token(TokenType.SEMICOLON)
        elif char == '"' or char == "'":
            self._string(char)
        else:
            # Handle comments and unquoted literals/identifiers
            if char == '/':
                if self._match('/'):  # Single-line comment
                    while self._peek() != '\n' and not self._is_at_end():
                        self._advance()
                    return
                elif self._match('*'):  # Block comment
                    while not (self._peek() == '*' and self._peek_next() == '/') and not self._is_at_end():
                        if self._peek() == '\n':
                            self.line += 1
                        self._advance()
                    if not self._is_at_end():
                        self._advance()  # *
                        self._advance()  # /
                    return

            # If we're here, it's an identifier or unquoted literal.
            while not self._is_at_end() and self._peek() not in ' \r\t\n{}:=;"\'':
                if self._peek() == '/' and self._peek_next() in '/*':
                    break
                self._advance()

            text = self.source[self.start:self.current]
            if text == 'text':
                self._add_token(TokenType.TEXT)
            elif text:
                self._add_token(TokenType.IDENTIFIER)

    def _string(self, quote_type: str):
        while self._peek() != quote_type and not self._is_at_end():
            if self._peek() == '\n':
                self.line += 1
            self._advance()

        if self._is_at_end():
            # Unterminated string.
            # For now, we'll just report it. In the future, this should raise a proper error.
            print(f"Unterminated string on line {self.line}")
            return

        # The closing quote.
        self._advance()

        # Trim the surrounding quotes.
        value = self.source[self.start + 1:self.current - 1]
        self._add_token(TokenType.STRING, value)

    def _match(self, expected: str) -> bool:
        if self._is_at_end():
            return False
        if self.source[self.current] != expected:
            return False

        self.current += 1
        return True

    def _peek(self) -> str:
        if self._is_at_end():
            return '\0'
        return self.source[self.current]

    def _peek_next(self) -> str:
        if self.current + 1 >= len(self.source):
            return '\0'
        return self.source[self.current + 1]

    def _is_at_end(self) -> bool:
        return self.current >= len(self.source)

    def _advance(self) -> str:
        char = self.source[self.current]
        self.current += 1
        self.col += 1
        return char

    def _add_token(self, type: TokenType, literal: object = None):
        text = self.source[self.start:self.current]
        self.tokens.append(Token(type, text if literal is None else literal, self.line, self.start))
