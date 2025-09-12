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

    # Punctuation
    COLON = ":"
    EQUALS = "="
    SEMICOLON = ";"
    AT = "@"
    DOT = "."
    HASH = "#"
    AMPERSAND = "&"
    GT = ">"

    # Comments - The lexer will likely skip these, but the types are here for completeness
    COMMENT_LINE = "//"
    COMMENT_BLOCK = "/**/"
    COMMENT_GENERATOR = "--"


# A Token is a named tuple with a type and a literal value.
# The type is from the TokenType enum, and the literal is the original string.
Token = collections.namedtuple('Token', ['type', 'literal'])
