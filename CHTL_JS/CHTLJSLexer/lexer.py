from CHTL_JS.CHTLJSLexer.token import CHTLJSToken
from CHTL_JS.CHTLJSLexer.token_type import CHTLJSTokenType

class CHTLJSLexer:
    def __init__(self, source: str):
        self.source = source
        self.tokens = []
        self.start = 0
        self.current = 0
        self.line = 1
        self.keywords = {"listen": CHTLJSTokenType.IDENTIFIER} # Can be expanded

    def scan_tokens(self) -> list[CHTLJSToken]:
        while not self._is_at_end():
            self.start = self.current
            self._scan_token()
        self.tokens.append(CHTLJSToken(CHTLJSTokenType.EOF, "", self.line, self.start))
        return self.tokens

    def _scan_token(self):
        char = self._advance()
        if char in ' \r\t\n': return

        if char == '{':
            if self._match('{'): self._add_token(CHTLJSTokenType.LBRACE_LBRACE)
            else: self._add_token(CHTLJSTokenType.LBRACE)
        elif char == '}':
            if self._match('}'): self._add_token(CHTLJSTokenType.RBRACE_RBRACE)
            else: self._add_token(CHTLJSTokenType.RBRACE)
        elif char == '-':
            if self._match('>'): self._add_token(CHTLJSTokenType.ARROW)
        elif char == ':': self._add_token(CHTLJSTokenType.COLON)
        elif char == ';': self._add_token(CHTLJSTokenType.SEMICOLON)
        else:
            # This simplified lexer assumes anything else is an identifier
            # or a placeholder from the Unified Scanner.
            while not self._is_at_end() and self._peek() not in ' \r\t\n{}:;':
                self._advance()
            text = self.source[self.start:self.current]
            # Check for keywords, otherwise it's an identifier/placeholder
            ttype = self.keywords.get(text, CHTLJSTokenType.IDENTIFIER)
            self._add_token(ttype)

    def _is_at_end(self) -> bool: return self.current >= len(self.source)
    def _advance(self) -> str:
        char = self.source[self.current]; self.current += 1; return char
    def _match(self, expected: str) -> bool:
        if self._is_at_end() or self.source[self.current] != expected: return False
        self.current += 1; return True
    def _peek(self) -> str: return '\0' if self._is_at_end() else self.source[self.current]
    def _add_token(self, type: CHTLJSTokenType):
        text = self.source[self.start:self.current]
        self.tokens.append(CHTLJSToken(type, text, self.line, self.start))
