from enum import Enum, auto
from dataclasses import dataclass

class TokenType(Enum):
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACK, RBRACK = auto(), auto(), auto(), auto(), auto(), auto()
    COLON, SEMICOLON, EQUALS, COMMA, DOT, AT = auto(), auto(), auto(), auto(), auto(), auto()
    QUESTION, SLASH, GT, LT = auto(), auto(), auto(), auto()

    # Operators
    PLUS, MINUS, STAR, PERCENT, POWER = auto(), auto(), auto(), auto(), auto()
    AMPERSAND, AND, OR = auto(), auto(), auto()

    # Literals
    IDENTIFIER, STRING, UNQUOTED_LITERAL = auto(), auto(), auto()

    # Other
    COMMENT, EOF, UNKNOWN = auto(), auto(), auto()

@dataclass
class Token:
    type: TokenType
    value: str
    lineno: int
    col: int

class Lexer:
    def __init__(self, source_code: str):
        self.source_code = source_code
        self.pos = 0
        self.lineno = 1
        self.line_start = 0
        self.tokens: list[Token] = []
        self.simple_tokens = {
            '{': TokenType.LBRACE, '}': TokenType.RBRACE, '(': TokenType.LPAREN,
            ')': TokenType.RPAREN, '[': TokenType.LBRACK, ']': TokenType.RBRACK,
            ':': TokenType.COLON, ';': TokenType.SEMICOLON, '=': TokenType.EQUALS,
            ',': TokenType.COMMA, '.': TokenType.DOT, '@': TokenType.AT,
            '?': TokenType.QUESTION, '+': TokenType.PLUS, '%': TokenType.PERCENT,
            '>': TokenType.GT, '<': TokenType.LT
        }

    def tokenize(self) -> list[Token]:
        while self.pos < len(self.source_code):
            start_pos = self.pos
            char = self.current_char()

            if char.isspace(): self.advance(); continue

            if char == '&':
                if self.peek() == '&': self.add_token(TokenType.AND, '&&'); self.advance(2); continue
                else: self.add_token(TokenType.AMPERSAND, '&'); self.advance(); continue
            if char == '|':
                if self.peek() == '|': self.add_token(TokenType.OR, '||'); self.advance(2); continue
            if char == '*':
                if self.peek() == '*': self.add_token(TokenType.POWER, '**'); self.advance(2); continue
                else: self.add_token(TokenType.STAR, '*'); self.advance(); continue
            if char == '/':
                if self.peek() == '/': self.consume_line_comment('//'); continue
                elif self.peek() == '*': self.consume_multi_line_comment(); continue
                else: self.add_token(TokenType.SLASH, '/'); self.advance(); continue
            if char == '-':
                if self.peek() == '-': self.consume_line_comment('--'); continue
                else: self.add_token(TokenType.MINUS, '-'); self.advance(); continue

            if char in self.simple_tokens:
                self.add_token(self.simple_tokens[char], char)
                self.advance()
                continue

            if char == '"' or char == "'": self.consume_string(char); continue
            if char.isalpha() or char in '_#': self.consume_identifier(); continue

            self.consume_unquoted_literal()

            if self.pos == start_pos:
                self.add_token(TokenType.UNKNOWN, self.current_char())
                self.advance()

        self.add_token(TokenType.EOF, '')
        return self.tokens

    def consume_line_comment(self, prefix):
        start_pos = self.pos
        self.advance(len(prefix))
        while self.pos < len(self.source_code) and self.current_char() not in '\n\r':
            self.advance()
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.COMMENT, value)

    def consume_multi_line_comment(self):
        start_pos = self.pos
        self.advance(2)
        while self.pos < len(self.source_code) - 1:
            if self.current_char() == '*' and self.peek() == '/':
                self.advance(2)
                value = self.source_code[start_pos:self.pos]
                self.add_token(TokenType.COMMENT, value)
                return
            self.advance()
        # Unterminated comment
        self.add_token(TokenType.UNKNOWN, self.source_code[start_pos:])
        self.pos = len(self.source_code)

    def consume_string(self, quote_char):
        start_pos = self.pos
        self.advance()
        while self.pos < len(self.source_code) and self.current_char() != quote_char:
            if self.current_char() == '\\': self.advance()
            self.advance()
        self.advance()
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.STRING, value)

    def consume_identifier(self):
        start_pos = self.pos
        while self.pos < len(self.source_code):
            char = self.current_char()
            if char.isspace() or not (char.isalnum() or char in '_-.' or char == '#'): break
            self.advance()
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.IDENTIFIER, value)

    def consume_unquoted_literal(self):
        start_pos = self.pos
        terminators = list(self.simple_tokens.keys()) + ['/', '"', "'", '|', '&', '*', '-']
        while self.pos < len(self.source_code):
            char = self.current_char()
            if char.isspace() or char in terminators: break
            self.advance()
        value = self.source_code[start_pos:self.pos]
        if value: self.add_token(TokenType.UNQUOTED_LITERAL, value)

    def add_token(self, type, value):
        col = self.pos - self.line_start + 1
        self.tokens.append(Token(type, value, self.lineno, col))

    def advance(self, count=1):
        for _ in range(count):
            if self.pos < len(self.source_code):
                if self.current_char() == '\n':
                    self.lineno += 1
                    self.line_start = self.pos + 1
                self.pos += 1

    def current_char(self):
        if self.pos < len(self.source_code): return self.source_code[self.pos]
        return '\0'

    def peek(self):
        if self.pos + 1 < len(self.source_code): return self.source_code[self.pos + 1]
        return '\0'
