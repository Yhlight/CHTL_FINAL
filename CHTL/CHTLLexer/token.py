from dataclasses import dataclass
from CHTL.CHTLLexer.token_type import TokenType

@dataclass
class Token:
    type: TokenType
    lexeme: str
    line: int
    column: int

    def __repr__(self):
        return f"Token(type={self.type.name}, lexeme='{self.lexeme}')"
