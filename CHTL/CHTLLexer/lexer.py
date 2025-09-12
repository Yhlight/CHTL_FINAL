from .token import Token, TokenType

class Lexer:
    keywords = {}

    def __init__(self, source: str):
        self.source = source
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1
        self.column = 1

    def scan_tokens(self) -> list[Token]:
        while not self._is_at_end():
            self.start = self.current
            self._scan_token()

        self.tokens.append(Token(TokenType.EOF, "", self.line, self.column))
        return self.tokens

    def _is_at_end(self) -> bool:
        return self.current >= len(self.source)

    def _advance(self) -> str:
        self.current += 1
        self.column += 1
        return self.source[self.current - 1]

    def _add_token(self, type: TokenType, value: str = None):
        if value is None:
            value = self.source[self.start:self.current]
        self.tokens.append(Token(type, value, self.line, self.start_column))

    def _peek(self) -> str:
        if self._is_at_end():
            return '\0'
        return self.source[self.current]

    def _peek_next(self) -> str:
        if self.current + 1 >= len(self.source):
            return '\0'
        return self.source[self.current + 1]

    def _match(self, expected: str) -> bool:
        if self._is_at_end():
            return False
        if self.source[self.current] != expected:
            return False
        self.current += 1
        self.column += 1
        return True

    def _scan_token(self):
        self.start_column = self.column
        char = self._advance()

        if char in ' \r\t': pass
        elif char == '\n':
            self.line += 1
            self.column = 1
        elif char == '{': self._add_token(TokenType.LBRACE)
        elif char == '}': self._add_token(TokenType.RBRACE)
        elif char == ':': self._add_token(TokenType.COLON)
        elif char == '=': self._add_token(TokenType.EQUAL_EQUAL if self._match('=') else TokenType.EQUALS)
        elif char == '!':
            if self._match('='): self._add_token(TokenType.BANG_EQUAL)
            else: self._error("Unexpected character '!'")
        elif char == '<': self._add_token(TokenType.LESS_EQUAL if self._match('=') else TokenType.LESS)
        elif char == '>': self._add_token(TokenType.GREATER_EQUAL if self._match('=') else TokenType.GREATER)
        elif char == '&':
            if self._match('&'): self._add_token(TokenType.AMPERSAND_AMPERSAND)
            else: self._error("Unexpected character '&'")
        elif char == '|':
            if self._match('|'): self._add_token(TokenType.PIPE_PIPE)
            else: self._error("Unexpected character '|'")
        elif char == '?': self._add_token(TokenType.QUESTION)
        elif char == '.': self._add_token(TokenType.DOT)
        elif char == '#': self._add_token(TokenType.HASH)
        elif char == ';': self._add_token(TokenType.SEMICOLON)
        elif char == '(': self._add_token(TokenType.LPAREN)
        elif char == ')': self._add_token(TokenType.RPAREN)
        elif char == '"' or char == "'": self._scan_string(char)
        elif char == '+': self._add_token(TokenType.PLUS)
        elif char == '%': self._add_token(TokenType.PERCENT)
        elif char == '/':
            if self._match('/'):
                while self._peek() != '\n' and not self._is_at_end(): self._advance()
            elif self._match('*'): self._scan_multiline_comment()
            else: self._add_token(TokenType.SLASH)
        elif char == '*': self._add_token(TokenType.STAR_STAR if self._match('*') else TokenType.STAR)
        elif char == '-':
            if self._match('-'): self._scan_generator_comment()
            else: self._add_token(TokenType.MINUS)
        else:
            if self._is_digit(char): self._scan_number()
            elif self._is_alpha(char): self._scan_identifier()
            else: self._scan_unquoted_literal()

    def _scan_number(self):
        while self._is_digit(self._peek()): self._advance()
        if self._peek() == '.' and self._is_digit(self._peek_next()):
            self._advance()
            while self._is_digit(self._peek()): self._advance()
        self._add_token(TokenType.NUMBER, self.source[self.start:self.current])

    def _scan_string(self, quote_char: str):
        while self._peek() != quote_char and not self._is_at_end():
            if self._peek() == '\n':
                self.line += 1
                self.column = 0
            self._advance()
        if self._is_at_end(): self._error("Unterminated string.")
        else:
            self._advance()
            value = self.source[self.start + 1:self.current - 1]
            self._add_token(TokenType.STRING, value)

    def _scan_multiline_comment(self):
        while not (self._peek() == '*' and self._peek_next() == '/') and not self._is_at_end():
            if self._peek() == '\n':
                self.line += 1
                self.column = 0
            self._advance()
        if self._is_at_end(): self._error("Unterminated multi-line comment.")
        else:
            self._advance(); self._advance()

    def _scan_generator_comment(self):
        while self._peek() != '\n' and not self._is_at_end(): self._advance()
        self._add_token(TokenType.GEN_COMMENT, self.source[self.start + 2:self.current].strip())

    def _is_alpha(self, char: str) -> bool: return 'a' <= char <= 'z' or 'A' <= char <= 'Z' or char == '_'
    def _is_digit(self, char: str) -> bool: return '0' <= char <= '9'
    def _is_alphanumeric(self, char: str) -> bool: return self._is_alpha(char) or self._is_digit(char)

    def _scan_identifier(self):
        while self._is_alphanumeric(self._peek()) or self._peek() == '-': self._advance()
        text = self.source[self.start:self.current]
        self._add_token(self.keywords.get(text, TokenType.IDENTIFIER), text)

    def _scan_unquoted_literal(self):
        stop_chars = ['{', '}', ';', ':', '=', '\n']
        while self._peek() not in stop_chars and not self._is_at_end(): self._advance()
        value = self.source[self.start:self.current].strip()
        if value: self._add_token(TokenType.UNQUOTED_LITERAL, value)

    def _error(self, message: str):
        raise Exception(f"[Line {self.line}, Column {self.start_column}] Error: {message}")
