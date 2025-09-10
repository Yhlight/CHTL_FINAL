from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLNode import nodes

class ParseError(Exception): pass

class ExpressionParser:
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> nodes.ExpressionNode:
        if not self.tokens:
            return nodes.LiteralNode(value="")
        return self._conditional()

    def _conditional(self):
        expr = self._logical_or()
        if self._match(TokenType.QUESTION):
            true_branch = self._conditional()
            self._consume(TokenType.COLON, "Expect ':' after true branch of conditional expression.")
            false_branch = self._conditional()
            return nodes.ConditionalNode(condition=expr, true_branch=true_branch, false_branch=false_branch)
        return expr

    def _logical_or(self):
        expr = self._logical_and()
        while self._match(TokenType.PIPE_PIPE):
            op = self._previous()
            right = self._logical_and()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _logical_and(self):
        expr = self._equality()
        while self._match(TokenType.AMPERSAND_AMPERSAND):
            op = self._previous()
            right = self._equality()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _equality(self):
        expr = self._comparison()
        while self._match(TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL):
            op = self._previous()
            right = self._comparison()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _comparison(self):
        expr = self._addition()
        while self._match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL):
            op = self._previous()
            right = self._addition()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _addition(self):
        expr = self._multiplication()
        while self._match(TokenType.PLUS, TokenType.MINUS):
            op = self._previous()
            right = self._multiplication()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _multiplication(self):
        expr = self._primary()
        while self._match(TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.STAR_STAR):
            op = self._previous()
            right = self._primary()
            expr = nodes.BinaryOpNode(left=expr, op=op, right=right)
        return expr

    def _primary(self):
        if self._match(TokenType.NUMBER):
            number_token = self._previous()
            if not self._is_at_end() and self._peek().type == TokenType.IDENTIFIER:
                unit_token = self._advance()
                return nodes.ValueWithUnitNode(value=float(number_token.lexeme), unit=unit_token.lexeme)
            return nodes.LiteralNode(value=float(number_token.lexeme))

        if self._match(TokenType.STRING):
            return nodes.LiteralNode(value=self._previous().lexeme[1:-1])

        # Handle full selector references like #box.width
        if self._peek().type in [TokenType.HASH, TokenType.DOT]:
            selector_tokens = [self._advance()] # Consume '#' or '.'
            selector_tokens.append(self._consume(TokenType.IDENTIFIER, "Expect identifier after '#' or '.' in selector."))

            self._consume(TokenType.DOT, "Expect '.' to access a property in a reference.")
            prop_name_token = self._consume(TokenType.IDENTIFIER, "Expect property name after '.' in a reference.")

            return nodes.ReferenceNode(property_name=prop_name_token.lexeme, selector_tokens=selector_tokens)

        if self._match(TokenType.IDENTIFIER):
            # This is a local reference.
            return nodes.ReferenceNode(property_name=self._previous().lexeme)

        raise self._error(self._peek(), "Expect expression.")

    # --- Helpers ---
    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype): return self._advance()
        raise self._error(self._peek(), message)
    def _check(self, ttype: TokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == ttype
    def _is_at_end(self) -> bool: return self.current >= len(self.tokens)
    def _peek(self) -> Token:
        if self._is_at_end(): return self.tokens[-1] # Gracefully handle peeking at the end
        return self.tokens[self.current]
    def _previous(self) -> Token: return self.tokens[self.current - 1]
    def _advance(self) -> Token:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _match(self, *types: TokenType) -> bool:
        for ttype in types:
            if self._check(ttype):
                self._advance(); return True
        return False
    def _error(self, token: Token, message: str) -> ParseError:
        return ParseError(f"Expression Parse Error at '{token.lexeme}': {message}")
