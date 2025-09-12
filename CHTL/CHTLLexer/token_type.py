from enum import Enum, auto

class TokenType(Enum):
    # Core syntax tokens
    LBRACE = auto()         # {
    RBRACE = auto()         # }
    COLON = auto()          # :
    SEMICOLON = auto()      # ;
    EQUALS = auto()         # =

    # Literals
    IDENTIFIER = auto()
    STRING = auto()

    # Keywords
    TEXT = auto()

    # Special
    EOF = auto()
