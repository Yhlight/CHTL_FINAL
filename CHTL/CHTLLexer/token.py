from enum import Enum
from typing import NamedTuple

class TokenType(Enum):
    # Single-character tokens
    LBRACE = '{'
    RBRACE = '}'
    COLON = ':'
    EQUALS = '='
    SEMICOLON = ';'
    LPAREN = '('
    RPAREN = ')'
    DOT = '.'
    HASH = '#'
    AT = '@'

    # Keywords
    TEMPLATE_KEYWORD = '[Template]'
    INHERIT = 'inherit'

    # Operators
    QUESTION = '?'
    EQUAL_EQUAL = '=='
    BANG_EQUAL = '!='
    LESS = '<'
    LESS_EQUAL = '<='
    GREATER = '>'
    GREATER_EQUAL = '>='
    AMPERSAND_AMPERSAND = '&&'
    PIPE_PIPE = '||'
    PLUS = '+'
    MINUS = '-'
    STAR = '*'
    SLASH = '/'
    PERCENT = '%'
    STAR_STAR = '**'

    # Literals
    IDENTIFIER = 'IDENTIFIER'
    NUMBER = 'NUMBER'
    STRING = 'STRING'
    UNQUOTED_LITERAL = 'UNQUOTED_LITERAL'

    # Comments
    GEN_COMMENT = 'GEN_COMMENT' # --

    # End of file
    EOF = 'EOF'

class Token(NamedTuple):
    type: TokenType
    value: str
    line: int
    column: int

    def __repr__(self):
        return f"Token(type={self.type.name}, value='{self.value}', line={self.line}, column={self.column})"
