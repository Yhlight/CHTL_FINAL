from typing import List, Union

from .CHTLLexer.token import Token, TokenType
from .CHTLNode.nodes import (
    RootNode, ElementNode, TextNode, AttributeNode, CommentNode, Node,
    StyleNode, StylePropertyNode, SelectorBlockNode,
    ExprNode, BinaryOpNode, DimensionNode, IdentifierNode, ConditionalNode,
    AttributeReferenceNode, TemplateDefinitionNode, TemplateUsageNode, VarUsageNode,
    VarDefinitionNode, CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode
)

class Parser:
    def __init__(self, tokens: List[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> RootNode:
        children = []
        while not self._is_at_end(): children.append(self._declaration())
        return RootNode(children=children)

    def _declaration(self) -> Node:
        if self._match(TokenType.TEMPLATE_KEYWORD): return self._parse_template_definition()
        if self._match(TokenType.CUSTOM_KEYWORD): return self._parse_custom_definition()
        if self._match(TokenType.GEN_COMMENT): return self._comment_node()
        return self._element_declaration()

    def _statement(self) -> Node:
        if self._peek().type == TokenType.INSERT: return self._parse_insert_statement()
        if self._peek().type == TokenType.DELETE: return self._parse_delete_statement()
        if self._peek().type in [TokenType.AT, TokenType.INHERIT]: return self._parse_template_usage()
        if self._peek().type == TokenType.IDENTIFIER and self._peek().value == 'style' and self._peek_next().type == TokenType.LBRACE: return self._parse_style_block()
        if self._peek().type == TokenType.IDENTIFIER and self._peek_next().type in [TokenType.COLON, TokenType.EQUALS]: return self._attribute_or_text_statement()
        if self._match(TokenType.GEN_COMMENT): return self._comment_node()
        return self._element_declaration()

    def _element_declaration(self) -> ElementNode:
        tag_token = self._consume(TokenType.IDENTIFIER, "Expect element name.")
        if tag_token.value == 'text':
            self._consume(TokenType.LBRACE, "Expect '{'.")
            if self._match(TokenType.STRING): content_token = self._previous()
            else: content_token = self._consume(TokenType.UNQUOTED_LITERAL, "Expect literal.")
            self._consume(TokenType.RBRACE, "Expect '}'.")
            return TextNode(content=content_token.value)
        self._consume(TokenType.LBRACE, "Expect '{'.")
        attributes, children = [], []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            node = self._statement()
            if isinstance(node, AttributeNode): attributes.append(node)
            else: children.append(node)
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return ElementNode(tag_name=tag_token.value, attributes=attributes, children=children)

    def _attribute_or_text_statement(self) -> Union[AttributeNode, TextNode]:
        name_token = self._consume(TokenType.IDENTIFIER, "Expect attribute name.")
        self._consume_any([TokenType.COLON, TokenType.EQUALS], "Expect ':' or '='.")
        if self._match(TokenType.STRING): value_token = self._previous()
        elif self._match(TokenType.IDENTIFIER): value_token = self._previous()
        elif self._match(TokenType.UNQUOTED_LITERAL): value_token = self._previous()
        else: self._error(self._peek(), "Expect attribute value.")
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        if name_token.value == 'text': return TextNode(content=value_token.value)
        return AttributeNode(name=name_token.value, value=value_token.value)

    def _comment_node(self) -> CommentNode: return CommentNode(content=self._previous().value)

    def _parse_template_definition(self) -> TemplateDefinitionNode:
        self._consume(TokenType.AT, "Expect '@'.")
        type_token = self._consume(TokenType.IDENTIFIER, "Expect template type.")
        name_token = self._consume(TokenType.IDENTIFIER, "Expect template name.")
        self._consume(TokenType.LBRACE, "Expect '{'.")
        body = []
        while not self._check(TokenType.RBRACE):
            if type_token.value == "Style":
                if self._peek().type in [TokenType.AT, TokenType.INHERIT]: body.append(self._parse_template_usage())
                else: body.append(self._parse_style_property())
            elif type_token.value == "Element": body.append(self._statement())
            elif type_token.value == "Var": body.append(self._parse_var_definition())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return TemplateDefinitionNode(template_type=type_token.value, name=name_token.value, body=body)

    def _parse_custom_definition(self) -> CustomDefinitionNode:
        self._consume(TokenType.AT, "Expect '@'.")
        type_token = self._consume(TokenType.IDENTIFIER, "Expect custom type.")
        name_token = self._consume(TokenType.IDENTIFIER, "Expect custom name.")
        self._consume(TokenType.LBRACE, "Expect '{'.")
        body = [] # Parsing body same as template for now
        while not self._check(TokenType.RBRACE):
            if type_token.value == "Style": body.append(self._parse_style_property(allow_valueless=True))
            else: body.append(self._statement())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return CustomDefinitionNode(template_type=type_token.value, name=name_token.value, body=body)

    def _parse_var_definition(self) -> VarDefinitionNode:
        name, _ = self._consume(TokenType.IDENTIFIER, "Expect var name."), self._consume(TokenType.COLON, "Expect ':'.")
        value = self._parse_expression()
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return VarDefinitionNode(name.value, value)

    def _parse_template_usage(self) -> Union[TemplateUsageNode, CustomUsageNode]:
        self._match(TokenType.INHERIT)
        self._consume(TokenType.AT, "Expect '@'.")
        type_token = self._consume(TokenType.IDENTIFIER, "Expect type.")
        name_token = self._consume(TokenType.IDENTIFIER, "Expect name.")
        if self._match(TokenType.LBRACE):
            body = []
            while not self._check(TokenType.RBRACE):
                if self._peek().type == TokenType.DELETE: body.append(self._parse_delete_statement())
                elif self._peek().type == TokenType.INSERT: body.append(self._parse_insert_statement())
                else: body.append(self._statement())
            self._consume(TokenType.RBRACE, "Expect '}'.")
            return CustomUsageNode(type_token.value, name_token.value, body)
        else:
            self._consume(TokenType.SEMICOLON, "Expect ';'.")
            return TemplateUsageNode(type_token.value, name_token.value)

    def _parse_delete_statement(self) -> DeleteNode:
        self._consume(TokenType.DELETE, "Expect 'delete'.")
        targets = [self._consume(TokenType.IDENTIFIER, "Expect target.").value]
        while self._match(TokenType.COMMA):
            targets.append(self._consume(TokenType.IDENTIFIER, "Expect target.").value)
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return DeleteNode(targets)

    def _parse_insert_statement(self) -> InsertNode:
        self._consume(TokenType.INSERT, "Expect 'insert'.")
        pos = self._consume_any([TokenType.AFTER, TokenType.BEFORE, TokenType.REPLACE, TokenType.AT_TOP, TokenType.AT_BOTTOM], "Expect position.").value
        # Simplified selector parsing for now
        target = self._consume(TokenType.IDENTIFIER, "Expect target selector.").value
        self._consume(TokenType.LBRACE, "Expect '{'.")
        body = []
        while not self._check(TokenType.RBRACE): body.append(self._statement())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return InsertNode(pos, target, body)

    def _parse_style_block(self) -> StyleNode:
        self._consume(TokenType.IDENTIFIER, "Expect 'style'.")
        self._consume(TokenType.LBRACE, "Expect '{'.")
        rules = []
        while not self._check(TokenType.RBRACE):
            if self._peek().type in [TokenType.AT, TokenType.INHERIT]: rules.append(self._parse_template_usage())
            elif self._peek().type == TokenType.IDENTIFIER and self._peek_next().type == TokenType.COLON: rules.append(self._parse_style_property())
            else: rules.append(self._parse_selector_block())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return StyleNode(rules=rules)

    def _parse_selector_block(self) -> SelectorBlockNode:
        tokens = []
        while not self._check(TokenType.LBRACE): tokens.append(self._advance())
        selector = "".join(t.value for t in tokens) # Simplified
        self._consume(TokenType.LBRACE, "Expect '{'.")
        properties = []
        while not self._check(TokenType.RBRACE): properties.append(self._parse_style_property())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return SelectorBlockNode(selector, properties)

    def _parse_style_property(self, allow_valueless=False) -> StylePropertyNode:
        name = self._consume(TokenType.IDENTIFIER, "Expect property name.")
        if allow_valueless and self._peek().type == TokenType.SEMICOLON:
            self._advance()
            return StylePropertyNode(name.value, None)
        self._consume(TokenType.COLON, "Expect ':'.")
        value = self._parse_expression()
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return StylePropertyNode(name.value, value)

    def _parse_expression(self) -> ExprNode: return self._parse_ternary()
    def _parse_ternary(self) -> ExprNode:
        expr = self._parse_logical_or()
        if self._match(TokenType.QUESTION):
            true_branch, _ = self._parse_ternary(), self._consume(TokenType.COLON, "Expect ':'.")
            false_branch = self._parse_ternary()
            return ConditionalNode(expr, true_branch, false_branch)
        return expr
    def _parse_logical_or(self) -> ExprNode:
        expr = self._parse_logical_and()
        while self._match(TokenType.PIPE_PIPE): expr = BinaryOpNode(expr, self._previous(), self._parse_logical_and())
        return expr
    def _parse_logical_and(self) -> ExprNode:
        expr = self._parse_equality()
        while self._match(TokenType.AMPERSAND_AMPERSAND): expr = BinaryOpNode(expr, self._previous(), self._parse_equality())
        return expr
    def _parse_equality(self) -> ExprNode:
        expr = self._parse_comparison()
        while self._match(TokenType.EQUAL_EQUAL, TokenType.BANG_EQUAL): expr = BinaryOpNode(expr, self._previous(), self._parse_comparison())
        return expr
    def _parse_comparison(self) -> ExprNode:
        expr = self._parse_term()
        while self._match(TokenType.GREATER, TokenType.GREATER_EQUAL, TokenType.LESS, TokenType.LESS_EQUAL): expr = BinaryOpNode(expr, self._previous(), self._parse_term())
        return expr
    def _parse_term(self) -> ExprNode:
        expr = self._parse_factor()
        while self._match(TokenType.PLUS, TokenType.MINUS): expr = BinaryOpNode(expr, self._previous(), self._parse_factor())
        return expr
    def _parse_factor(self) -> ExprNode:
        expr = self._parse_power()
        while self._match(TokenType.STAR, TokenType.SLASH, TokenType.PERCENT): expr = BinaryOpNode(expr, self._previous(), self._parse_power())
        return expr
    def _parse_power(self) -> ExprNode:
        expr = self._parse_primary()
        if self._match(TokenType.STAR_STAR): expr = BinaryOpNode(expr, self._previous(), self._parse_power())
        return expr
    def _parse_primary(self) -> ExprNode:
        if self._match(TokenType.DOT, TokenType.HASH): return self._parse_attribute_reference(self._previous())
        if self._peek().type == TokenType.IDENTIFIER and self._peek_next().type == TokenType.DOT: return self._parse_attribute_reference()
        if self._peek().type == TokenType.IDENTIFIER and self._peek_next().type == TokenType.LPAREN: return self._parse_var_usage()
        if self._match(TokenType.NUMBER):
            number, unit = self._previous(), ""
            if self._peek().type == TokenType.IDENTIFIER: unit = self._advance().value
            return DimensionNode(number.value, unit)
        if self._match(TokenType.STRING): return IdentifierNode(self._previous().value)
        if self._match(TokenType.IDENTIFIER): return IdentifierNode(self._previous().value)
        if self._match(TokenType.LPAREN):
            expr = self._parse_expression()
            self._consume(TokenType.RPAREN, "Expect ')'.")
            return expr
        self._error(self._peek(), "Expect expression.")
    def _parse_var_usage(self) -> VarUsageNode:
        group, _ = self._consume(TokenType.IDENTIFIER, "Expect group."), self._consume(TokenType.LPAREN, "Expect '('.")
        var, _ = self._consume(TokenType.IDENTIFIER, "Expect var."), self._consume(TokenType.RPAREN, "Expect ')'.")
        return VarUsageNode(group.value, var.value)
    def _parse_attribute_reference(self, prefix: Token = None) -> AttributeReferenceNode:
        selector = (prefix.value + self._consume(TokenType.IDENTIFIER, "Expect id.").value) if prefix else self._consume(TokenType.IDENTIFIER, "Expect selector.").value
        self._consume(TokenType.DOT, "Expect '.'.")
        prop = self._consume(TokenType.IDENTIFIER, "Expect property.").value
        return AttributeReferenceNode(selector, prop)

    def _match(self, *types):
        for t in types:
            if self._check(t): self._advance(); return True
        return False
    def _consume(self, type, msg):
        if self._check(type): return self._advance()
        self._error(self._peek(), msg)
    def _consume_any(self, types, msg):
        for t in types:
            if self._check(t): return self._advance()
        self._error(self._peek(), msg)
    def _check(self, type): return not self._is_at_end() and self._peek().type == type
    def _advance(self):
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _is_at_end(self): return self._peek().type == TokenType.EOF
    def _peek(self): return self.tokens[self.current]
    def _peek_next(self): return self.tokens[self.current + 1] if self.current + 1 < len(self.tokens) else self.tokens[-1]
    def _previous(self): return self.tokens[self.current - 1]
    def _error(self, token, msg): raise Exception(f"[L{token.line}] at '{token.value}': {msg}")
