from CHTL.CHTLLexer.keywords import Token, TokenType
from CHTL.CHTLNode.nodes import (
    ExpressionNode, LiteralNode, BinaryOpNode, LogicalOpNode,
    ConditionalExpressionNode, PropertyReferenceNode, FunctionCallNode
)
from .selector_parser import SelectorParser
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

    def consume(self, expected_type: Optional[TokenType] = None) -> Token:
        token = self.current_token()
        if not token:
            raise RuntimeError("Unexpected end of expression token stream.")
        if expected_type and token.type != expected_type:
            raise RuntimeError(f"Line {token.lineno}: Expected token {expected_type} in expression, but got {token.type}")
        self.pos += 1
        return token

    def parse(self) -> ExpressionNode:
        if not self.tokens:
            raise ValueError("Cannot parse an empty token stream.")
        expr = self.parse_expression(0)
        while self.current_token() and self.current_token().type == TokenType.QUESTION:
            expr = self.parse_ternary_expression(expr)
            if self.current_token() and self.current_token().type == TokenType.COMMA:
                self.consume()
            else:
                break
        return expr

    def parse_expression(self, min_precedence: int) -> ExpressionNode:
        left = self.parse_primary()
        while True:
            op = self.current_token()
            if op is None or op.type not in PRECEDENCE or PRECEDENCE[op.type] < min_precedence:
                break
            self.consume()
            precedence = PRECEDENCE[op.type]
            right = self.parse_expression(precedence + 1)
            NodeClass = LogicalOpNode if op.type in (TokenType.AND, TokenType.OR) else BinaryOpNode
            left = NodeClass(left=left, op=op, right=right, lineno=op.lineno)
        return left

    def parse_primary(self) -> ExpressionNode:
        token = self.current_token()
        if token is None:
            raise ValueError("Unexpected end of expression.")

        if token.type == TokenType.LPAREN:
            self.consume()
            expr = self.parse_expression(0)
            if not self.current_token() or self.current_token().type != TokenType.RPAREN:
                raise ValueError("Mismatched parentheses in expression.")
            self.consume()
            return expr

        i = self.pos
        is_prop_ref = False
        while i < len(self.tokens):
            if self.tokens[i].type == TokenType.DOT:
                is_prop_ref = True
                break
            if self.tokens[i].type in PRECEDENCE or self.tokens[i].type == TokenType.QUESTION:
                break
            i += 1

        if is_prop_ref:
            return self.parse_property_reference()

        if token.type == TokenType.IDENTIFIER and self.peek_token() and self.peek_token().type == TokenType.LPAREN:
            return self.parse_function_call()

        if token.type == TokenType.IDENTIFIER:
            self.consume()
            return PropertyReferenceNode(property_name=token.value, selector=None, lineno=token.lineno)

        if token.type in (TokenType.STRING, TokenType.UNQUOTED_LITERAL):
            self.consume()
            value = token.value[1:-1] if token.type == TokenType.STRING else token.value
            return LiteralNode(value=value, lineno=token.lineno)

        raise ValueError(f"Unexpected token in expression: {token.type} ('{token.value}')")

    def parse_function_call(self) -> FunctionCallNode:
        callee_token = self.consume()
        self.consume(TokenType.LPAREN)

        args = []
        if self.current_token().type != TokenType.RPAREN:
            while True:
                # For now, assume arguments are simple identifiers/literals, not complex expressions
                arg_expr = self.parse_primary()
                args.append(arg_expr)
                if self.current_token().type == TokenType.RPAREN:
                    break
                self.consume(TokenType.COMMA)

        self.consume(TokenType.RPAREN)
        return FunctionCallNode(callee_name=callee_token.value, arguments=args, lineno=callee_token.lineno)

    def parse_property_reference(self) -> PropertyReferenceNode:
        lineno = self.current_token().lineno

        # Find the position of the last dot in this part of the expression
        last_dot_pos = -1
        end_pos = len(self.tokens)
        for i in range(self.pos, len(self.tokens)):
            if self.tokens[i].type in PRECEDENCE or self.tokens[i].type in (TokenType.QUESTION, TokenType.RPAREN, TokenType.COMMA, TokenType.COLON):
                end_pos = i
                break

        for i in range(end_pos - 1, self.pos - 1, -1):
            if self.tokens[i].type == TokenType.DOT:
                last_dot_pos = i
                break

        if last_dot_pos == -1:
            raise ValueError("Invalid property reference: no '.' found to separate selector and property.")

        selector_tokens = self.tokens[self.pos:last_dot_pos]
        self.pos = last_dot_pos # Move position to the last dot

        raw_selector_string = "".join(tok.value for tok in selector_tokens)
        selector_parser = SelectorParser(raw_selector_string)
        selector_node = selector_parser.parse()

        self.consume() # consume '.'

        prop_name_token = self.consume()
        if prop_name_token.type != TokenType.IDENTIFIER:
            raise ValueError(f"Expected property name after '.', got {prop_name_token.type}")

        return PropertyReferenceNode(selector=selector_node, property_name=prop_name_token.value, lineno=lineno)

    def parse_ternary_expression(self, condition: ExpressionNode) -> ConditionalExpressionNode:
        self.consume() # consume '?'
        true_expr = self.parse_expression(0)
        false_expr = None
        if self.current_token() and self.current_token().type == TokenType.COLON:
            self.consume()
            false_expr = self.parse_expression(0)
        return ConditionalExpressionNode(condition=condition, true_expr=true_expr, false_expr=false_expr, lineno=condition.lineno)
