from dataclasses import dataclass
from CHTL.CHTLLexer.token_type import TokenType

@dataclass
class Token:
    type: TokenType
    lexeme: str
    line: int
    col: int
