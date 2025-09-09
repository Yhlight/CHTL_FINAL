import re
from enum import Enum, auto
from dataclasses import dataclass

class TokenType(Enum):
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    LPAREN = auto()         # (
    RPAREN = auto()         # )
    LBRACK = auto()         # [
    RBRACK = auto()         # ]
    COLON = auto()          # :
    SEMICOLON = auto()      # ;
    EQUALS = auto()         # =
    COMMA = auto()          # ,
    DOT = auto()            # .
    AT = auto()             # @
    AMPERSAND = auto()      # &
    QUESTION = auto()       # ?
    STAR = auto()           # *
    SLASH = auto()          # /
    GT = auto()             # >
    LT = auto()             # <
    IDENTIFIER = auto()
    STRING = auto()
    UNQUOTED_LITERAL = auto()
    COMMENT = auto()
    EOF = auto()
    UNKNOWN = auto()

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
            '&': TokenType.AMPERSAND, '?': TokenType.QUESTION, '*': TokenType.STAR,
            '>': TokenType.GT, '<': TokenType.LT
        }

    def tokenize(self) -> list[Token]:
        while self.pos < len(self.source_code):
            start_pos = self.pos
            char = self.current_char()

            if char.isspace():
                self.advance()
                continue

            if char == '/':
                if self.peek() == '/':
                    self.consume_line_comment('//')
                    continue
                elif self.peek() == '*':
                    self.consume_multi_line_comment()
                    continue

            if char == '-':
                if self.peek() == '-':
                    self.consume_line_comment('--')
                    continue

            if char in self.simple_tokens:
                self.add_token(self.simple_tokens[char], char)
                self.advance()
                continue

            if char == '"' or char == "'":
                self.consume_string(char)
                continue

            if char.isalpha() or char in '_#':
                self.consume_identifier()
                continue

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
        while self.pos < len(self.source_code) - 1 and not (self.current_char() == '*' and self.peek() == '/'):
            self.advance()
        if self.pos < len(self.source_code):
            self.advance(2)
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.COMMENT, value)

    def consume_string(self, quote_char):
        start_pos = self.pos
        self.advance()
        while self.pos < len(self.source_code) and self.current_char() != quote_char:
            if self.current_char() == '\\':
                self.advance()
            self.advance()
        self.advance()
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.STRING, value)

    def consume_identifier(self):
        start_pos = self.pos
        while self.pos < len(self.source_code):
            char = self.current_char()
            if char.isspace() or not (char.isalnum() or char in '_-.' or char == '#'):
                break
            self.advance()
        value = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.IDENTIFIER, value)

    def consume_unquoted_literal(self):
        start_pos = self.pos
        terminators = list(self.simple_tokens.keys()) + ['/', '"', "'"]
        while self.pos < len(self.source_code):
            char = self.current_char()
            if char.isspace() or char in terminators:
                break
            if char == '-' and self.peek() == '-':
                break
            self.advance()
        value = self.source_code[start_pos:self.pos]
        if value:
            self.add_token(TokenType.UNQUOTED_LITERAL, value)

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
        if self.pos < len(self.source_code):
            return self.source_code[self.pos]
        return '\0'

    def peek(self):
        if self.pos + 1 < len(self.source_code):
            return self.source_code[self.pos + 1]
        return '\0'
