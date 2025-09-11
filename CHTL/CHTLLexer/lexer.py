from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType

class Lexer:
    def __init__(self, source: str):
        self.source = source
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1

        self.keywords = {
            "text": TokenType.TEXT,
            "style": TokenType.STYLE,
            "from": TokenType.IDENTIFIER,
            "as": TokenType.IDENTIFIER,
            "Template": TokenType.IDENTIFIER,
            "Import": TokenType.IDENTIFIER,
            "Custom": TokenType.IDENTIFIER,
            "Origin": TokenType.IDENTIFIER,
            "delete": TokenType.IDENTIFIER,
            "insert": TokenType.IDENTIFIER,
        }

    def scan_tokens(self) -> list[Token]:
        while not self._is_at_end():
            self.start = self.current
            self._scan_token()
        self.tokens.append(Token(TokenType.EOF, "", self.line, self.start))
        return self.tokens

    def _scan_token(self):
        char = self._advance()

        if char in ' \r\t': return
        if char == '\n': self.line += 1; return

        # Single-character tokens
        if char == '{': self._add_token(TokenType.LBRACE); return
        if char == '}': self._add_token(TokenType.RBRACE); return
        if char == '[': self._add_token(TokenType.LBRACKET); return
        if char == ']': self._add_token(TokenType.RBRACKET); return
        if char == '@': self._add_token(TokenType.AT); return
        if char == ',': self._add_token(TokenType.COMMA); return
        if char == ';': self._add_token(TokenType.SEMICOLON); return
        if char == '.': self._add_token(TokenType.DOT); return
        if char == '#': self._add_token(TokenType.HASH); return
        if char == '&': self._add_token(TokenType.AMPERSAND); return
        if char == '?': self._add_token(TokenType.QUESTION); return
        if char == '+': self._add_token(TokenType.PLUS); return
        if char == '-': self._add_token(TokenType.MINUS); return
        if char == '%': self._add_token(TokenType.PERCENT); return

        # One or two character tokens
        if char == '=': self._add_token(TokenType.EQUAL_EQUAL if self._match('=') else TokenType.EQUALS); return
        if char == '!':
            if self._match('='): self._add_token(TokenType.BANG_EQUAL)
            return
        if char == '>': self._add_token(TokenType.GREATER_EQUAL if self._match('=') else TokenType.GREATER); return
        if char == '<': self._add_token(TokenType.LESS_EQUAL if self._match('=') else TokenType.LESS); return
        if char == '*':
            self._add_token(TokenType.STAR_STAR if self._match('*') else TokenType.STAR)
            return
        if char == '|':
            if self._match('|'): self._add_token(TokenType.PIPE_PIPE)
            return

        # Handle comments and slash operator
        if char == '/':
            if self._match('/'): # Line comment
                while self._peek() != '\n' and not self._is_at_end(): self._advance()
            elif self._match('*'): # Block comment
                 while not (self._peek() == '*' and self._peek_next() == '/') and not self._is_at_end():
                    if self._peek() == '\n': self.line += 1
                    self._advance()
                 if not self._is_at_end(): self._advance(); self._advance()
            else: # Slash operator
                self._add_token(TokenType.SLASH)
            return

        if char == ':': self._add_token(TokenType.COLON); return

        # Literals
        if char == '"' or char == "'": self._string(char); return
        if self._is_digit(char): self._number(); return
        if self._is_alpha(char): self._identifier(); return

        # Any other character is unexpected in CHTL, but might be valid in embedded JS.
        # Preserve it so the parser can reconstruct the script content.
        self._add_token(TokenType.UNEXPECTED)

    def _string(self, quote_type: str):
        while self._peek() != quote_type and not self._is_at_end():
            if self._peek() == '\n': self.line += 1
            self._advance()
        if self._is_at_end(): return
        self._advance()
        self._add_token(TokenType.STRING)

    def _number(self):
        while self._is_digit(self._peek()): self._advance()
        if self._peek() == '.' and self._is_digit(self._peek_next()):
            self._advance()
            while self._is_digit(self._peek()): self._advance()
        self._add_token(TokenType.NUMBER)

    def _identifier(self):
        while self._is_alpha_numeric(self._peek()) or self._peek() == '-': self._advance()
        text = self.source[self.start:self.current]
        token_type = self.keywords.get(text, TokenType.IDENTIFIER)
        self._add_token(token_type)

    # --- Helpers ---
    def _is_digit(self, char: str) -> bool: return '0' <= char <= '9'
    def _is_alpha(self, char: str) -> bool: return ('a' <= char <= 'z') or ('A' <= char <= 'Z') or char == '_'
    def _is_alpha_numeric(self, char: str) -> bool: return self._is_alpha(char) or self._is_digit(char)
    def _is_at_end(self) -> bool: return self.current >= len(self.source)
    def _advance(self) -> str:
        char = self.source[self.current]; self.current += 1; return char
    def _match(self, expected: str) -> bool:
        if self._is_at_end() or self.source[self.current] != expected: return False
        self.current += 1; return True
    def _peek(self) -> str: return '\0' if self._is_at_end() else self.source[self.current]
    def _peek_next(self) -> str: return '\0' if self.current + 1 >= len(self.source) else self.source[self.current + 1]
    def _add_token(self, type: TokenType):
        text = self.source[self.start:self.current]
        self.tokens.append(Token(type, text, self.line, self.start))
