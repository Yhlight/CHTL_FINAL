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
        if self._match(CHTLJSTokenType.VIR):
            return self._parse_virtual_object_assignment()

        if self._peek().type == CHTLJSTokenType.IDENTIFIER and self._peek(1).type == CHTLJSTokenType.ARROW:
             return self._parse_expression_statement()

        if self._match(CHTLJSTokenType.ANIMATE):
            return self._parse_animate_statement()

        if self._match(CHTLJSTokenType.ROUTER):
            return self._parse_router_statement()

        if self._check(CHTLJSTokenType.LBRACE_LBRACE):
            if self._peek(4).type == CHTLJSTokenType.DELEGATE:
                return self._parse_delegate_statement()
            else:
                return self._parse_listen_statement()

        if self._check(CHTLJSTokenType.EOF): return None
        self._advance()
        return None

    def _parse_router_statement(self):
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{' after 'router'")
        properties = []
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            key_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property key.")
            self._consume(CHTLJSTokenType.COLON, "Expect ':' after property key.")

            # Handle multiple values for a single key
            while True:
                value = self._parse_simple_value()
                properties.append(nodes.PropertyNode(key=key_token.lexeme, value=value))
                if not self._match(CHTLJSTokenType.COMMA):
                    break
                # If there's a comma, check if the next token is another value for the same key
                # or a new key. If it's an identifier followed by a colon, it's a new key.
                if self._peek().type == CHTLJSTokenType.IDENTIFIER and self._peek(1).type == CHTLJSTokenType.COLON:
                    break

        self._consume(CHTLJSTokenType.RBRACE, "Expect '}' to close router block.")
        return nodes.RouterNode(properties=properties)

    def _parse_expression_statement(self):
        expr = self._parse_expression()
        self._consume(CHTLJSTokenType.SEMICOLON, "Expect ';' after expression.")
        return nodes.ExpressionStatementNode(expression=expr)

    def _parse_expression(self):
        if self._peek().type == CHTLJSTokenType.IDENTIFIER and self._peek(1).type == CHTLJSTokenType.ARROW:
            object_name = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect object name.").lexeme
            self._consume(CHTLJSTokenType.ARROW, "Expect '->'.")
            member_name = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect member name.").lexeme
            return nodes.MemberAccessNode(object_name=object_name, member_name=member_name)

        raise CHTLJSParseError(f"Unexpected token in expression: {self._peek().lexeme}")

    def _parse_virtual_object_assignment(self):
        name = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect variable name after 'vir'.").lexeme
        self._consume(CHTLJSTokenType.EQUALS, "Expect '=' after variable name.")

        if self._check(CHTLJSTokenType.LBRACE_LBRACE):
            value_node = self._parse_listen_statement()
        elif self._check(CHTLJSTokenType.ANIMATE):
            value_node = self._parse_animate_statement()
        else:
            raise CHTLJSParseError(f"Unexpected token for virtual object value: {self._peek().lexeme}")

        self._consume(CHTLJSTokenType.SEMICOLON, "Expect ';' after virtual object assignment.")
        return nodes.VirtualObjectNode(name=name, value=value_node)

    def _parse_animate_statement(self):
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{' after 'animate'")
        anim_node = nodes.AnimateNode()
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            key_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property key.")
            self._consume(CHTLJSTokenType.COLON, "Expect ':' after property key.")
            key = key_token.lexeme
            if key in ['target', 'duration', 'easing', 'loop', 'direction', 'delay', 'callback']:
                value = self._parse_simple_value()
                setattr(anim_node, key, value)
            elif key == 'begin' or key == 'end':
                props = self._parse_property_block()
                setattr(anim_node, key, props)
            elif key == 'when':
                anim_node.when = self._parse_when_block()
            else:
                raise CHTLJSParseError(f"Unknown animate property: {key}")
            if self._check(CHTLJSTokenType.COMMA): self._advance()
        self._consume(CHTLJSTokenType.RBRACE, "Expect '}' to close animate block.")
        return anim_node

    def _parse_delegate_statement(self):
        parent_selector_node = self._parse_enhanced_selector()
        self._consume(CHTLJSTokenType.ARROW, "Expect '->'")
        self._consume(CHTLJSTokenType.DELEGATE, "Expect 'delegate'")
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{'")
        target_selectors = []
        listeners = []
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            key_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property key.")
            self._consume(CHTLJSTokenType.COLON, "Expect ':' after key.")
            if key_token.lexeme == 'target':
                if self._match(CHTLJSTokenType.LBRACKET):
                    while not self._check(CHTLJSTokenType.RBRACKET):
                        target_selectors.append(self._parse_simple_value())
                        if not self._match(CHTLJSTokenType.COMMA): break
                    self._consume(CHTLJSTokenType.RBRACKET, "Expect ']' to close target list.")
                else:
                    target_selectors.append(self._parse_simple_value())
            else:
                event_name = key_token.lexeme
                callback_code = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect callback placeholder.").lexeme
                listeners.append(nodes.EventListenerNode(event_name=event_name.strip(), callback_code=callback_code.strip()))
            if self._check(CHTLJSTokenType.COMMA): self._advance()
        self._consume(CHTLJSTokenType.RBRACE, "Expect '}'")
        return nodes.DelegateNode(
            parent_selector=parent_selector_node,
            target_selectors=target_selectors,
            listeners=listeners
        )

    def _parse_simple_value(self) -> str:
        if self._check(CHTLJSTokenType.LBRACE_LBRACE):
            self._advance()
            selector_text = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect selector text.").lexeme
            self._consume(CHTLJSTokenType.RBRACE_RBRACE, "Expect '}}'.")
            return f"{{{{{selector_text}}}}}"
        else:
            value_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property value.")
            return value_token.lexeme

    def _parse_property_block(self) -> list[nodes.PropertyNode]:
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{' to open property block.")
        properties = []
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            key = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property key.").lexeme
            self._consume(CHTLJSTokenType.COLON, "Expect ':' after key.")
            value = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect property value.").lexeme
            properties.append(nodes.PropertyNode(key=key, value=value))
            if self._check(CHTLJSTokenType.COMMA): self._advance()
        self._consume(CHTLJSTokenType.RBRACE, "Expect '}' to close property block.")
        return properties

    def _parse_when_block(self) -> list[nodes.KeyframeNode]:
        self._consume(CHTLJSTokenType.LBRACKET, "Expect '[' to open 'when' block.")
        keyframes = []
        while not self._check(CHTLJSTokenType.RBRACKET) and not self._is_at_end():
            props = self._parse_property_block()
            keyframes.append(nodes.KeyframeNode(properties=props))
            if self._check(CHTLJSTokenType.COMMA): self._advance()
        self._consume(CHTLJSTokenType.RBRACKET, "Expect ']' to close 'when' block.")
        return keyframes

    def _parse_listen_statement(self):
        target_node = self._parse_enhanced_selector()
        self._consume(CHTLJSTokenType.ARROW, "Expect '->'")
        self._consume(CHTLJSTokenType.IDENTIFIER, "Expect 'listen'")
        self._consume(CHTLJSTokenType.LBRACE, "Expect '{'")
        listeners = []
        while not self._check(CHTLJSTokenType.RBRACE) and not self._is_at_end():
            event_name = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect event name").lexeme
            self._consume(CHTLJSTokenType.COLON, "Expect ':'")
            callback_code = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect callback placeholder").lexeme
            listeners.append(nodes.EventListenerNode(event_name=event_name.strip(), callback_code=callback_code.strip()))
            if self._match(CHTLJSTokenType.SEMICOLON) or self._match(CHTLJSTokenType.COMMA):
                continue
        self._consume(CHTLJSTokenType.RBRACE, "Expect '}'")
        return nodes.ListenBlockNode(target=target_node, listeners=listeners)

    def _parse_enhanced_selector(self) -> nodes.EnhancedSelectorNode:
        start_brace = self._consume(CHTLJSTokenType.LBRACE_LBRACE, "Expect '{{'")
        selector_token = self._consume(CHTLJSTokenType.IDENTIFIER, "Expect selector text")
        end_brace = self._consume(CHTLJSTokenType.RBRACE_RBRACE, "Expect '}}'")
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
    def _peek(self, offset=0) -> CHTLJSToken:
        if self.current + offset >= len(self.tokens):
            return self.tokens[-1] # EOF
        return self.tokens[self.current + offset]
    def _previous(self) -> CHTLJSToken: return self.tokens[self.current - 1]
    def _advance(self) -> CHTLJSToken:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _match(self, *types: CHTLJSTokenType) -> bool:
        for ttype in types:
            if self._check(ttype):
                self._advance(); return True
        return False
