from enum import Enum, auto
from dataclasses import dataclass

class TokenType(Enum):
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACK, RBRACK = auto(), auto(), auto(), auto(), auto(), auto()
    COLON, SEMICOLON, EQUALS, COMMA, DOT, AT = auto(), auto(), auto(), auto(), auto(), auto()
    QUESTION, SLASH, GT, LT = auto(), auto(), auto(), auto()

    # Operators
    PLUS, MINUS, STAR, PERCENT, POWER = auto(), auto(), auto(), auto(), auto()
    AMPERSAND, AND, OR = auto(), auto(), auto()
    EQ_EQ, NOT_EQ, GTE, LTE = auto(), auto(), auto(), auto()

    # Literals
    IDENTIFIER, STRING, UNQUOTED_LITERAL, RAW_CONTENT = auto(), auto(), auto(), auto()

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
            ':': TokenType.COLON, ';': TokenType.SEMICOLON, ',': TokenType.COMMA,
            '.': TokenType.DOT, '@': TokenType.AT, '?': TokenType.QUESTION,
            '+': TokenType.PLUS, '%': TokenType.PERCENT,
        }

    def tokenize(self) -> list[Token]:
        while self.pos < len(self.source_code):
            start_pos = self.pos
            char = self.current_char()

            if char.isspace(): self.advance(); continue

            # Lookahead for [Origin] blocks. This is a special case.
            if char == '[' and self.peek_ahead("Origin]"):
                # Manually tokenize the [Origin] block parts
                self.add_token(TokenType.LBRACK, '['); self.advance()
                self.add_token(TokenType.IDENTIFIER, 'Origin'); self.advance(len('Origin'))
                self.add_token(TokenType.RBRACK, ']'); self.advance()
                self.consume_whitespace()

                # Type, e.g., @Html
                if self.current_char() == '@':
                    self.consume_identifier() # This will add the @Type token
                self.consume_whitespace()

                # Optional name
                if self.current_char().isalpha():
                    self.consume_identifier() # This will add the name token
                self.consume_whitespace()

                if self.current_char() == '{':
                    self.add_token(TokenType.LBRACE, '{'); self.advance()
                    self.consume_raw_content() # This adds the RAW_CONTENT token
                    self.add_token(TokenType.RBRACE, '}'); self.advance()
                continue

            if char == '&':
                if self.peek() == '&': self.add_token(TokenType.AND, '&&'); self.advance(2); continue
                else: self.add_token(TokenType.AMPERSAND, '&'); self.advance(); continue
            if char == '|':
                if self.peek() == '|': self.add_token(TokenType.OR, '||'); self.advance(2); continue
            if char == '*':
                if self.peek() == '*': self.add_token(TokenType.POWER, '**'); self.advance(2); continue
                else: self.add_token(TokenType.STAR, '*'); self.advance(); continue
            if char == '!':
                if self.peek() == '=': self.add_token(TokenType.NOT_EQ, '!='); self.advance(2); continue
            if char == '=':
                if self.peek() == '=': self.add_token(TokenType.EQ_EQ, '=='); self.advance(2); continue
                else: self.add_token(TokenType.EQUALS, '='); self.advance(); continue
            if char == '>':
                if self.peek() == '=': self.add_token(TokenType.GTE, '>='); self.advance(2); continue
                else: self.add_token(TokenType.GT, '>'); self.advance(); continue
            if char == '<':
                if self.peek() == '=': self.add_token(TokenType.LTE, '<='); self.advance(2); continue
                else: self.add_token(TokenType.LT, '<'); self.advance(); continue

            if char == '/':
                if self.peek() == '/': self.consume_line_comment('//'); continue
                elif self.peek() == '*': self.consume_multi_line_comment(); continue
                else: self.add_token(TokenType.SLASH, '/'); self.advance(); continue
            if char == '-':
                if self.peek() == '-': self.consume_line_comment('--'); continue
                else: self.add_token(TokenType.MINUS, '-'); self.advance(); continue

            if char in self.simple_tokens:
                self.add_token(self.simple_tokens[char], char); self.advance(); continue

            if char == '"' or char == "'": self.consume_string(char); continue
            if char.isalpha() or char == '_': self.consume_identifier(); continue
            if char.isdigit(): self.consume_unquoted_literal(); continue
            if char == '#': self.consume_identifier(); continue

            self.consume_unquoted_literal()

            if self.pos == start_pos:
                self.add_token(TokenType.UNKNOWN, self.current_char()); self.advance()

        self.add_token(TokenType.EOF, ''); return self.tokens

    def consume_raw_content(self):
        brace_level = 1
        start_pos = self.pos
        while self.pos < len(self.source_code):
            char = self.current_char()
            if char == '{': brace_level += 1
            elif char == '}':
                brace_level -= 1
                if brace_level == 0: break
            self.advance()
        content = self.source_code[start_pos:self.pos]
        self.add_token(TokenType.RAW_CONTENT, content)

    def consume_line_comment(self, prefix):
        start_pos = self.pos; self.advance(len(prefix))
        while self.pos < len(self.source_code) and self.current_char() not in '\n\r': self.advance()
        self.add_token(TokenType.COMMENT, self.source_code[start_pos:self.pos])

    def consume_multi_line_comment(self):
        start_pos = self.pos; self.advance(2)
        while self.pos < len(self.source_code) - 1:
            if self.current_char() == '*' and self.peek() == '/':
                self.advance(2); self.add_token(TokenType.COMMENT, self.source_code[start_pos:self.pos]); return
            self.advance()
        self.add_token(TokenType.UNKNOWN, self.source_code[start_pos:]); self.pos = len(self.source_code)

    def consume_string(self, quote_char):
        start_pos = self.pos; self.advance()
        while self.pos < len(self.source_code) and self.current_char() != quote_char:
            if self.current_char() == '\\': self.advance()
            self.advance()
        self.advance(); self.add_token(TokenType.STRING, self.source_code[start_pos:self.pos])

    def consume_identifier(self):
        start_pos = self.pos
        while self.pos < len(self.source_code):
            if not (self.current_char().isalnum() or self.current_char() in '_-@#'): break
            self.advance()
        self.add_token(TokenType.IDENTIFIER, self.source_code[start_pos:self.pos])

    def consume_unquoted_literal(self):
        start_pos = self.pos
        terminators = list(self.simple_tokens.keys()) + ['/', '"', "'", '|', '&', '*', '-', '=', '>', '<', '!', '(', ')']
        while self.pos < len(self.source_code):
            if self.current_char().isspace() or self.current_char() in terminators: break
            self.advance()
        value = self.source_code[start_pos:self.pos]
        if value: self.add_token(TokenType.UNQUOTED_LITERAL, value)

    def consume_whitespace(self):
        while self.pos < len(self.source_code) and self.current_char().isspace(): self.advance()

    def add_token(self, type, value):
        col = self.pos - self.line_start + 1
        self.tokens.append(Token(type, value, self.lineno, col))

    def advance(self, count=1):
        for _ in range(count):
            if self.pos < len(self.source_code):
                if self.current_char() == '\n': self.lineno += 1; self.line_start = self.pos + 1
                self.pos += 1

    def current_char(self):
        return self.source_code[self.pos] if self.pos < len(self.source_code) else '\0'

    def peek(self):
        return self.source_code[self.pos + 1] if self.pos + 1 < len(self.source_code) else '\0'

    def peek_ahead(self, text: str) -> bool:
        return self.source_code.startswith(text, self.pos + 1)
