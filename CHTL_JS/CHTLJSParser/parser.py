from CHTL_JS.CHTLJSLexer.token import CHTLJSToken
from CHTL_JS.CHTLJSLexer.token_type import CHTLJSTokenType
from CHTL_JS.CHTLJSNode import nodes

class CHTLJSParseError(Exception): pass

class CHTLJSParser:
    def __init__(self, tokens: list[CHTLJSToken]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> nodes.CHTLJS_ProgramNode:
        program = nodes.CHTLJS_ProgramNode()
        while not self._is_at_end():
            node = self._parse_statement()
            if node: program.children.append(node)
        return program

    def _parse_statement(self):
        if self._check(CHTLJSTokenType.LBRACE_LBRACE):
            return self._parse_listen_statement()
        if self._check(CHTLJSTokenType.EOF): return None
        self._advance()
        return None

    def _parse_listen_statement(self):
        target_node = self._parse_enhanced_selector()
        self._consume(CHTLJSTokenType.ARROW, "Expect '->'")
        self._consume(CHTLJSTokenType.IDENTIFIER, "Expect 'listen'")
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{'")

        listeners = []
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            event_name = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect event name").lexeme
            self._consume(CHTLJSTokenType.COLON, "Expect ':'")

            # The callback is now a single identifier (the placeholder)
            callback_code = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect callback placeholder").lexeme
            listeners.append(nodes.EventListenerNode(event_name=event_name.strip(), callback_code=callback_code.strip()))

            if self._match(CHTLJSTokenType.SEMICOLON):
                continue

        self._consume(CHTLJSTokenType.RBRACE, "Expect '}'")
        return nodes.ListenBlockNode(target=target_node, listeners=listeners)

    def _parse_enhanced_selector(self) -> nodes.EnhancedSelectorNode:
        start_brace = self._consume(CHTLJSTokenType.LBRACE_LBRACE, "Expect '{{'")
        selector_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect selector text")
        end_brace = self._consume(CHTLJSTokenType.RBRACE_RBRACE, "Expect '}}'")

        # The selector text should include the braces
        full_text = start_brace.lexeme + selector_token.lexeme + end_brace.lexeme
        return nodes.EnhancedSelectorNode(selector_text=full_text)

    # --- Helpers ---
    def _consume(self, ttype: CHTLJSTokenType, message: str) -> CHTLJSToken:
        if self._check(ttype): return self._advance()
        raise CHTLJSParseError(f"Parse Error at '{self._peek().lexeme}': {message}")
    def _check(self, ttype: CHTLJSTokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == ttype
    def _is_at_end(self) -> bool: return self._peek().type == CHTLJSTokenType.EOF
    def _peek(self) -> CHTLJSToken: return self.tokens[self.current]
    def _previous(self) -> CHTLJSToken: return self.tokens[self.current - 1]
    def _advance(self) -> CHTLJSToken:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _match(self, *types: CHTLJSTokenType) -> bool:
        for ttype in types:
            if self._check(ttype):
                self._advance(); return True
        return False
