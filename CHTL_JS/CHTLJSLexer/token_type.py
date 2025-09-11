from enum import Enum, auto

class CHTLJSTokenType(Enum):
    # CHTL JS specific tokens
    LBRACE_LBRACE = auto()  # {{
    RBRACE_RBRACE = auto()  # }}
    ARROW = auto()          # ->

    # Re-using some standard token types
    IDENTIFIER = auto()
    LBRACE = auto()
    RBRACE = auto()
    LBRACKET = auto()
    RBRACKET = auto()
    COLON = auto()
    SEMICOLON = auto()
    COMMA = auto()
    EQUALS = auto()

    # Keywords
    ANIMATE = auto()
    DELEGATE = auto()
    VIR = auto()
    ROUTER = auto()

    # For now, we'll treat JS code as opaque identifiers
    JS_CODE = auto()

    EOF = auto()
