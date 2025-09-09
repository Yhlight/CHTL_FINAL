from CHTL.CHTLLexer.lexer import Token, TokenType
from CHTL.CHTLNode.nodes import (
    ExpressionNode, LiteralNode, BinaryOpNode, LogicalOpNode,
    ConditionalExpressionNode, PropertyReferenceNode
)
from typing import List, Dict, Optional

# Operator precedence mapping for binary operators
PRECEDENCE: Dict[TokenType, int] = {
    TokenType.OR: 1,
    TokenType.AND: 2,
    TokenType.EQ_EQ: 3,
    TokenType.NOT_EQ: 3,
    TokenType.LT: 4,
    TokenType.LTE: 4,
    TokenType.GT: 4,
    TokenType.GTE: 4,
    TokenType.PLUS: 5,
    TokenType.MINUS: 5,
    TokenType.STAR: 6,
    TokenType.SLASH: 6,
}

class ExpressionParser:
    """
    Parses a stream of tokens into an expression AST.
    Uses a precedence climbing algorithm.
    """
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.pos = 0

    def current_token(self) -> Optional[Token]:
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None

    def peek_token(self, offset: int = 1) -> Optional[Token]:
        if self.pos + offset < len(self.tokens):
            return self.tokens[self.pos + offset]
        return None

    def consume(self) -> Token:
        token = self.current_token()
        self.pos += 1
        return token

    def parse(self) -> ExpressionNode:
        """Main entry point for parsing an expression."""
        if not self.tokens:
            raise ValueError("Cannot parse an empty token stream.")

        expr = self.parse_expression(0)

        # After parsing the main expression, check for a ternary operator `? :`
        # and also for chained ternaries via comma
        while self.current_token() and self.current_token().type == TokenType.QUESTION:
            expr = self.parse_ternary_expression(expr)
            if self.current_token() and self.current_token().type == TokenType.COMMA:
                self.consume() # consume comma and look for next ternary
            else:
                break

        return expr

    def parse_expression(self, min_precedence: int) -> ExpressionNode:
        """Parses a binary expression using the precedence climbing algorithm."""
        left = self.parse_primary()

        while True:
            op = self.current_token()
            if op is None or op.type not in PRECEDENCE or PRECEDENCE[op.type] < min_precedence:
                break

            self.consume() # consume operator
            precedence = PRECEDENCE[op.type]

            right = self.parse_expression(precedence + 1)

            NodeClass = LogicalOpNode if op.type in (TokenType.AND, TokenType.OR) else BinaryOpNode
            left = NodeClass(left=left, op=op, right=right, lineno=op.lineno)

        return left

    def parse_primary(self) -> ExpressionNode:
        """Parses primary-level expressions: literals, property refs, and parenthesized expressions."""
        token = self.current_token()
        if token is None:
            raise ValueError("Unexpected end of expression.")

        if token.type == TokenType.LPAREN:
            self.consume() # consume '('
            expr = self.parse_expression(0)
            if not self.current_token() or self.current_token().type != TokenType.RPAREN:
                raise ValueError("Mismatched parentheses in expression.")
            self.consume() # consume ')'
            return expr

        # Check for a property reference like '.box.width' or 'div.height'
        if token.type == TokenType.DOT or (token.type == TokenType.IDENTIFIER and self.peek_token() and self.peek_token().type == TokenType.DOT):
            return self.parse_property_reference()

        if token.type in (TokenType.STRING, TokenType.UNQUOTED_LITERAL, TokenType.IDENTIFIER):
            self.consume()
            if token.type == TokenType.STRING:
                return LiteralNode(value=token.value[1:-1], lineno=token.lineno)
            # An identifier by itself is a local property reference
            if token.type == TokenType.IDENTIFIER:
                 return PropertyReferenceNode(selector=None, property_name=token.value, lineno=token.lineno)
            # An unquoted literal is just a value
            return LiteralNode(value=token.value, lineno=token.lineno)

        raise ValueError(f"Unexpected token in expression: {token.type} ('{token.value}')")

    def parse_property_reference(self) -> PropertyReferenceNode:
        """Parses a property reference like '.box.width' or 'div.height'."""
        selector_parts = []
        lineno = self.current_token().lineno

        # Consume selector parts until we hit the property access dot
        while self.peek_token() and self.peek_token().type != TokenType.DOT:
            selector_parts.append(self.consume().value)
        selector_parts.append(self.consume().value)

        selector = "".join(selector_parts)

        self.consume() # consume '.'

        prop_name_token = self.consume()
        if prop_name_token.type != TokenType.IDENTIFIER:
            raise ValueError(f"Expected property name after '.', got {prop_name_token.type}")

        return PropertyReferenceNode(selector=selector, property_name=prop_name_token.value, lineno=lineno)

    def parse_ternary_expression(self, condition: ExpressionNode) -> ConditionalExpressionNode:
        """Parses the true/false branches of a ternary `? :` expression."""
        self.consume() # consume '?'

        true_expr = self.parse_expression(0)
        false_expr = None

        if self.current_token() and self.current_token().type == TokenType.COLON:
            self.consume() # consume ':'
            false_expr = self.parse_expression(0)

        return ConditionalExpressionNode(
            condition=condition,
            true_expr=true_expr,
            false_expr=false_expr,
            lineno=condition.lineno
        )
