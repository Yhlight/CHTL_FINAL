from enum import Enum, auto

class TokenType(Enum):
    # Single-character tokens
    LBRACE = auto()
    RBRACE = auto()
    LPAREN = auto()
    RPAREN = auto()
    LBRACK = auto()
    RBRACK = auto()
    COLON = auto()
    SEMICOLON = auto()
    EQUALS = auto()
    COMMA = auto()
    DOT = auto()
    AT_SYMBOL = auto()
    QUESTION = auto()
    SLASH = auto()
    GT = auto()
    LT = auto()

    # Operators
    PLUS = auto()
    MINUS = auto()
    STAR = auto()
    PERCENT = auto()
    POWER = auto()
    AMPERSAND = auto()
    AND = auto()
    OR = auto()
    EQ_EQ = auto()
    NOT_EQ = auto()
    GTE = auto()
    LTE = auto()

    # Literals
    IDENTIFIER = auto()
    STRING = auto()
    UNQUOTED_LITERAL = auto()

    # Keywords
    CUSTOM = auto()
    TEMPLATE = auto()
    ORIGIN = auto()
    IMPORT = auto()
    NAMESPACE = auto()
    CONFIGURATION = auto()
    STYLE = auto()
    TEXT = auto()
    SCRIPT = auto()
    FROM = auto()
    AS = auto()
    USE = auto()
    EXCEPT = auto()
    HTML5 = auto()
    INHERIT = auto()
    DELETE = auto()
    INSERT = auto()
    AFTER = auto()
    BEFORE = auto()
    REPLACE = auto()
    AT_KEYWORD = auto()
    TOP = auto()
    BOTTOM = auto()

    # Other
    COMMENT = auto()
    EOF = auto()
    UNKNOWN = auto()

from dataclasses import dataclass

@dataclass
class Token:
    type: TokenType
    value: str
    lineno: int
    col: int
    char_offset: int

KEYWORDS = {
    "Custom": TokenType.CUSTOM,
    "Template": TokenType.TEMPLATE,
    "Origin": TokenType.ORIGIN,
    "Import": TokenType.IMPORT,
    "Namespace": TokenType.NAMESPACE,
    "Configuration": TokenType.CONFIGURATION,
    "style": TokenType.STYLE,
    "text": TokenType.TEXT,
    "script": TokenType.SCRIPT,
    "from": TokenType.FROM,
    "as": TokenType.AS,
    "use": TokenType.USE,
    "except": TokenType.EXCEPT,
    "html5": TokenType.HTML5,
    "inherit": TokenType.INHERIT,
    "delete": TokenType.DELETE,
    "insert": TokenType.INSERT,
    "after": TokenType.AFTER,
    "before": TokenType.BEFORE,
    "replace": TokenType.REPLACE,
    "at": TokenType.AT_KEYWORD,
    "top": TokenType.TOP,
    "bottom": TokenType.BOTTOM,
}
