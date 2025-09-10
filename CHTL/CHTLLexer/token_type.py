from enum import Enum, auto

class TokenType(Enum):
    # Single-character tokens
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    COLON = auto()          # :
    EQUALS = auto()         # =
    SEMICOLON = auto()      # ;
    SLASH = auto()          # /
    STAR = auto()           # *
    QUOTE = auto()          # "
    SINGLE_QUOTE = auto()   # '

    # Literals
    IDENTIFIER = auto()     # div, id, text, unquoted_literal
    STRING = auto()         # "a string"

    # Special CHTL keywords
    TEXT = auto()           # text

    # End of File
    EOF = auto()
