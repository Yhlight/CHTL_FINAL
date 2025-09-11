from dataclasses import dataclass
from CHTL_JS.CHTLJSLexer.token_type import CHTLJSTokenType

@dataclass
class CHTLJSToken:
    type: CHTLJSTokenType
    lexeme: str
    line: int
    col: int
