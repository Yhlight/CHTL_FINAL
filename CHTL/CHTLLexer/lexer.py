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

        if char == '{': self._add_token(TokenType.LBRACE); return
        if char == '}': self._add_token(TokenType.RBRACE); return
        if char == ':': self._add_token(TokenType.COLON); return
        if char == ';': self._add_token(TokenType.SEMICOLON); return
        if char == '=': self._add_token(TokenType.EQUALS); return

        if char == '"' or char == "'": self._string(char); return
        if self._is_alpha(char): self._identifier(); return

        print(f"Lexer Error: Unexpected character '{char}' on line {self.line}")

    def _string(self, quote_type: str):
        while self._peek() != quote_type and not self._is_at_end():
            if self._peek() == '\n': self.line += 1
            self._advance()
        if self._is_at_end():
            print(f"Lexer Error: Unterminated string on line {self.line}")
            return
        self._advance() # The closing quote
        value = self.source[self.start + 1:self.current - 1]
        self._add_token(TokenType.STRING, value)

    def _identifier(self):
        while self._is_alpha_numeric(self._peek()): self._advance()
        text = self.source[self.start:self.current]
        token_type = self.keywords.get(text, TokenType.IDENTIFIER)
        # The literal value of an identifier is its text content
        self._add_token(token_type, text)

    def _is_digit(self, char: str) -> bool: return '0' <= char <= '9'
    def _is_alpha(self, char: str) -> bool: return ('a' <= char <= 'z') or ('A' <= char <= 'Z') or char == '_'
    def _is_alpha_numeric(self, char: str) -> bool: return self._is_alpha(char) or self._is_digit(char)
    def _is_at_end(self) -> bool: return self.current >= len(self.source)
    def _advance(self) -> str: char = self.source[self.current]; self.current += 1; return char
    def _peek(self) -> str: return '\0' if self._is_at_end() else self.source[self.current]

    def _add_token(self, type: TokenType, literal: object = None):
        text = self.source[self.start:self.current]
        # The lexeme is the literal value for identifiers and strings
        if literal is None:
             literal = text
        self.tokens.append(Token(type, literal, self.line, self.start))
