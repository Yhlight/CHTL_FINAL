from typing import List, Union

from .CHTLLexer.token import Token, TokenType
from .CHTLNode.nodes import (
    RootNode, ElementNode, TextNode, AttributeNode, CommentNode, Node,
    StyleNode, StylePropertyNode, SelectorBlockNode,
    ExprNode, BinaryOpNode, DimensionNode, IdentifierNode, ConditionalNode,
    AttributeReferenceNode
)

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> RootNode:
        children = []
        while not self._is_at_end():
            children.append(self._declaration())
        return RootNode(children=children)

    def _declaration(self) -> Node:
        if self._match(TokenType.GEN_COMMENT):
            return self._comment_node()
        return self._element_declaration()

    def _statement(self) -> Node:
        if self._peek().type == TokenType.IDENTIFIER and self._peek().value == 'style' and self._peek_next().type == TokenType.LBRACE:
            return self._parse_style_block()
        if self._peek().type == TokenType.IDENTIFIER and self._peek_next().type in [TokenType.COLON, TokenType.EQUALS]:
            return self._attribute_or_text_statement()
        if self._match(TokenType.GEN_COMMENT):
            return self._comment_node()
        return self._element_declaration()

    def _element_declaration(self) -> ElementNode:
        tag_token = self._consume(TokenType.IDENTIFIER, "Expect element name.")
        if tag_token.value == 'text':
            self._consume(TokenType.LBRACE, "Expect '{' after 'text' keyword.")
            if self._match(TokenType.STRING):
                content_token = self._previous()
            else:
                content_token = self._consume(TokenType.UNQUOTED_LITERAL, "Expect literal inside text block.")
            self._consume(TokenType.RBRACE, "Expect '}' after text block.")
            return TextNode(content=content_token.value)
        self._consume(TokenType.LBRACE, "Expect '{' after element name.")
        attributes = []
        children = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            node = self._statement()
            if isinstance(node, AttributeNode):
                attributes.append(node)
            else:
                children.append(node)
        self._consume(TokenType.RBRACE, "Expect '}' after element block.")
        return ElementNode(tag_name=tag_token.value, attributes=attributes, children=children)

    def _attribute_or_text_statement(self) -> Union[AttributeNode, TextNode]:
        name_token = self._consume(TokenType.IDENTIFIER, "Expect attribute name.")
        self._consume_any([TokenType.COLON, TokenType.EQUALS], "Expect ':' or '=' after attribute name.")
        if self._match(TokenType.STRING): value_token = self._previous()
        elif self._match(TokenType.IDENTIFIER): value_token = self._previous()
        elif self._match(TokenType.UNQUOTED_LITERAL): value_token = self._previous()
        else: self._error(self._peek(), "Expect attribute value.")
        self._consume(TokenType.SEMICOLON, "Expect ';' after attribute value.")
        if name_token.value == 'text': return TextNode(content=value_token.value)
        return AttributeNode(name=name_token.value, value=value_token.value)

    def _comment_node(self) -> CommentNode:
        return CommentNode(content=self._previous().value)

    def _parse_style_block(self) -> StyleNode:
        self._consume(TokenType.IDENTIFIER, "Expect 'style' keyword.")
        self._consume(TokenType.LBRACE, "Expect '{' after 'style'.")
        rules = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            is_property = (self._peek().type == TokenType.IDENTIFIER and self._peek_next().type == TokenType.COLON)
            if is_property: rules.append(self._parse_style_property())
            else: rules.append(self._parse_selector_block())
        self._consume(TokenType.RBRACE, "Expect '}' after style block.")
        return StyleNode(rules=rules)

    def _parse_selector_block(self) -> SelectorBlockNode:
        selector_tokens = []
        while not self._check(TokenType.LBRACE) and not self._is_at_end():
            selector_tokens.append(self._advance())
        selector_parts = []
        for i, token in enumerate(selector_tokens):
            selector_parts.append(token.value)
            if (i + 1 < len(selector_tokens) and token.type == TokenType.IDENTIFIER and selector_tokens[i+1].type == TokenType.IDENTIFIER):
                selector_parts.append(" ")
        selector = "".join(selector_parts)
        self._consume(TokenType.LBRACE, "Expect '{' for selector block.")
        properties = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            properties.append(self._parse_style_property())
        self._consume(TokenType.RBRACE, "Expect '}' after selector block.")
        return SelectorBlockNode(selector=selector, properties=properties)

    def _parse_style_property(self) -> StylePropertyNode:
        name_token = self._consume(TokenType.IDENTIFIER, "Expect property name.")
        self._consume(TokenType.COLON, "Expect ':' after property name.")
        value_expr = self._parse_expression()
        self._consume(TokenType.SEMICOLON, "Expect ';' after property value.")
        return StylePropertyNode(property_name=name_token.value, value=value_expr)

    def _parse_expression(self) -> ExprNode: return self._parse_ternary()
    def _parse_ternary(self) -> ExprNode:
        expr = self._parse_logical_or()
        if self._match(TokenType.QUESTION):
            true_branch = self._parse_ternary()
            self._consume(TokenType.COLON, "Expect ':' for ternary operator.")
            false_branch = self._parse_ternary()
            return ConditionalNode(condition=expr, true_branch=true_branch, false_branch=false_branch)
        return expr
    def _parse_logical_or(self) -> ExprNode:
        expr = self._parse_logical_and()
        while self._match(TokenType.PIPE_PIPE): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_logical_and())
        return expr
    def _parse_logical_and(self) -> ExprNode:
        expr = self._parse_equality()
        while self._match(TokenType.AMPERSAND_AMPERSAND): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_equality())
        return expr
    def _parse_equality(self) -> ExprNode:
        expr = self._parse_comparison()
        while self._match(TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_comparison())
        return expr
    def _parse_comparison(self) -> ExprNode:
        expr = self._parse_term()
        while self._match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_term())
        return expr
    def _parse_term(self) -> ExprNode:
        expr = self._parse_factor()
        while self._match(TokenType.PLUS, TokenType.MINUS): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_factor())
        return expr
    def _parse_factor(self) -> ExprNode:
        expr = self._parse_power()
        while self._match(TokenType.STAR, TokenType.SLASH, TokenType.PERCENT): expr = BinaryOpNode(left=expr, op=self._previous(), right=self._parse_power())
        return expr
    def _parse_power(self) -> ExprNode:
        expr = self._parse_primary()
        if self._match(TokenType.STAR_STAR): return BinaryOpNode(left=expr, op=self._previous(), right=self._parse_power())
        return expr
    def _parse_primary(self) -> ExprNode:
        if self._match(TokenType.DOT, TokenType.HASH): return self._parse_attribute_reference(prefix_token=self._previous())
        if self._peek().type == TokenType.IDENTIFIER and self._peek_next().type == TokenType.DOT: return self._parse_attribute_reference()
        if self._match(TokenType.NUMBER):
            number_token = self._previous()
            unit = ""
            if self._peek().type == TokenType.IDENTIFIER: unit = self._advance().value
            return DimensionNode(value=number_token.value, unit=unit)
        if self._match(TokenType.STRING): return IdentifierNode(name=self._previous().value)
        if self._match(TokenType.IDENTIFIER): return IdentifierNode(name=self._previous().value)
        if self._match(TokenType.LPAREN):
            expr = self._parse_expression()
            self._consume(TokenType.RPAREN, "Expect ')' after expression.")
            return expr
        self._error(self._peek(), "Expect expression.")
    def _parse_attribute_reference(self, prefix_token: Token = None) -> AttributeReferenceNode:
        selector = ""
        if prefix_token: selector += prefix_token.value + self._consume(TokenType.IDENTIFIER, "Expect identifier after '.' or '#'").value
        else: selector += self._consume(TokenType.IDENTIFIER, "Expect identifier for selector.").value
        self._consume(TokenType.DOT, "Expect '.' after selector.")
        property_name = self._consume(TokenType.IDENTIFIER, "Expect property name after '.'.").value
        return AttributeReferenceNode(selector=selector, property_name=property_name)

    def _match(self, *types: TokenType) -> bool:
        for t in types:
            if self._check(t): self._advance(); return True
        return False
    def _consume(self, type: TokenType, message: str) -> Token:
        if self._check(type): return self._advance()
        self._error(self._peek(), message)
    def _consume_any(self, types: List[TokenType], message: str) -> Token:
        for t in types:
            if self._check(t): return self._advance()
        self._error(self._peek(), message)
    def _check(self, type: TokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == type
    def _advance(self) -> Token:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _is_at_end(self) -> bool: return self._peek().type == TokenType.EOF
    def _peek(self) -> Token: return self.tokens[self.current]
    def _peek_next(self) -> Token:
        if self.current + 1 >= len(self.tokens): return self.tokens[-1]
        return self.tokens[self.current + 1]
    def _previous(self) -> Token: return self.tokens[self.current - 1]
    def _error(self, token: Token, message: str):
        if token.type == TokenType.EOF: raise Exception(f"[Line {token.line}] Error at end: {message}")
        else: raise Exception(f"[Line {token.line}] Error at '{token.value}': {message}")
