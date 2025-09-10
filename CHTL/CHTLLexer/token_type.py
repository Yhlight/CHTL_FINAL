from enum import Enum, auto

class TokenType(Enum):
    # Single-character tokens
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    COLON = auto()          # :
    EQUALS = auto()         # =
    SEMICOLON = auto()      # ;

    # Literals
    IDENTIFIER = auto()     # div, id, width, .box, &:hover
    STRING = auto()         # "a string"
    NUMBER = auto()         # 100, 0.8

    # Keywords
    TEXT = auto()
    STYLE = auto()

    # End of File
    EOF = auto()
