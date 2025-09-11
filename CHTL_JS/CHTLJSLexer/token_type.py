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
    COLON = auto()
    SEMICOLON = auto()

    # For now, we'll treat JS code as opaque identifiers
    JS_CODE = auto()

    EOF = auto()
