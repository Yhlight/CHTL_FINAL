from enum import Enum, auto

class TokenType(Enum):
    # Single-character tokens
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    LBRACKET = auto()       # [
    RBRACKET = auto()       # ]
    AT = auto()             # @
    COMMA = auto()          # ,
    COLON = auto()          # :
    EQUALS = auto()         # =
    SEMICOLON = auto()      # ;
    DOT = auto()            # .
    HASH = auto()           # #
    AMPERSAND = auto()      # &
    QUESTION = auto()       # ?

    # Operators
    PLUS = auto()           # +
    MINUS = auto()          # -
    STAR = auto()           # *
    SLASH = auto()          # /
    PERCENT = auto()        # %

    # One or two character tokens
    STAR_STAR = auto()      # **
    GREATER = auto()        # >
    LESS = auto()           # <
    GREATER_EQUAL = auto()  # >=
    LESS_EQUAL = auto()     # <=
    EQUAL_EQUAL = auto()    # ==
    BANG_EQUAL = auto()     # !=
    AMPERSAND_AMPERSAND = auto() # &&
    PIPE_PIPE = auto()      # ||

    # Literals
    IDENTIFIER = auto()
    STRING = auto()
    NUMBER = auto()

    # Keywords
    TEXT = auto()
    STYLE = auto()

    # End of File
    EOF = auto()
