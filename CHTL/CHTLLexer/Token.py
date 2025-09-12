from enum import Enum
import collections

class TokenType(Enum):
    """
    Enum representing the type of a token in the CHTL language.
    """
    # Special Tokens
    EOF = "EOF"
    ILLEGAL = "ILLEGAL"

    # Literals
    IDENTIFIER = "IDENTIFIER"  # e.g., div, my_variable, color, red
    STRING = "STRING"        # "anything in quotes"
    HEX_LITERAL = "HEX_LITERAL" # e.g., #ff0000

    # Delimiters
    LBRACE = "{"
    RBRACE = "}"
    LBRACKET = "["
    RBRACKET = "]"
    LPAREN = "("
    RPAREN = ")"

    # Punctuation
    COLON = ":"
    EQUALS = "="
    SEMICOLON = ";"
    COMMA = ","
    AT = "@"
    DOT = "."
    HASH = "#"
    AMPERSAND = "&"
    GT = ">"

    # Operators
    PLUS = "+"
    MINUS = "-"
    ASTERISK = "*"
    SLASH = "/"
    PERCENT = "%"
    DOUBLE_ASTERISK = "**"

    # Comments - The lexer will likely skip these, but the types are here for completeness
    COMMENT_LINE = "//"
    COMMENT_BLOCK = "/**/"
    COMMENT_GENERATOR = "--"


# A Token is a named tuple that includes its type, literal value, and position in the source.
# The type is from the TokenType enum.
# The literal is the original string from the source code.
# start_pos and end_pos are the character indices in the source text.
Token = collections.namedtuple('Token', ['type', 'literal', 'start_pos', 'end_pos'])
