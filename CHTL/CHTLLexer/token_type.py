from enum import Enum, auto

class TokenType(Enum):
    # Single-character tokens
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    LBRACKET = auto()       # [
    RBRACKET = auto()       # ]
    COLON = auto()          # :
    SEMICOLON = auto()      # ;
    DOT = auto()            # .
    PLUS = auto()           # +
    MINUS = auto()          # -
    STAR = auto()           # *
    SLASH = auto()          # /
    PERCENT = auto()        # %
    HASH = auto()           # #
    AT = auto()             # @
    COMMA = auto()          # ,
    AMPERSAND = auto()      # &
    QUESTION = auto()       # ?

    # One or two character tokens
    EQUALS = auto()
    EQUAL_EQUAL = auto()
    BANG_EQUAL = auto()
    GREATER = auto()
    GREATER_EQUAL = auto()
    LESS = auto()
    LESS_EQUAL = auto()
    STAR_STAR = auto()
    PIPE_PIPE = auto()
    AMPERSAND_AMPERSAND = auto()

    # Literals
    IDENTIFIER = auto()
    STRING = auto()
    NUMBER = auto()

    # Keywords
    TEXT = auto()
    STYLE = auto()
    # Keywords for features to be added later
    TEMPLATE = auto()
    IMPORT = auto()
    CUSTOM = auto()
    ORIGIN = auto()
    DELETE = auto()
    INSERT = auto()

    # Special
    UNEXPECTED = auto()
    EOF = auto()
