from typing import Union, Tuple
from CHTL.CHTLLexer import Lexer, Token, TokenType
from CHTL.CHTLNode import (BaseNode, ElementNode, TextNode, StyleNode, CssRule,
                           ExpressionNode, InfixExpressionNode, NumericLiteralNode)
from enum import IntEnum

# --- Expression Parsing Infrastructure ---
class Precedence(IntEnum):
    LOWEST = 1
    SUM = 2
    PRODUCT = 3
    POWER = 4
    PREFIX = 5

PRECEDENCES = {
    TokenType.PLUS: Precedence.SUM,
    TokenType.MINUS: Precedence.SUM,
    TokenType.SLASH: Precedence.PRODUCT,
    TokenType.ASTERISK: Precedence.PRODUCT,
    TokenType.PERCENT: Precedence.PRODUCT,
    TokenType.DOUBLE_ASTERISK: Precedence.POWER,
}

class Parser:
    """
    The Parser for the CHTL language. It takes a lexer and builds an
    Abstract Syntax Tree (AST).
    """
    def __init__(self, lexer: Lexer):
        self.lexer = lexer
        self.errors = []
        self.current_token: Token = None
        self.peek_token: Token = None

        # Pratt Parser Setup
        self.prefix_parse_fns = {}
        self.infix_parse_fns = {}
        self.register_prefix(TokenType.IDENTIFIER, self._parse_identifier_value)
        self.register_prefix(TokenType.HEX_LITERAL, self._parse_text_literal)
        self.register_prefix(TokenType.STRING, self._parse_text_literal)
        self.register_infix(TokenType.PLUS, self._parse_infix_expression)
        self.register_infix(TokenType.MINUS, self._parse_infix_expression)
        self.register_infix(TokenType.SLASH, self._parse_infix_expression)
        self.register_infix(TokenType.ASTERISK, self._parse_infix_expression)
        self.register_infix(TokenType.PERCENT, self._parse_infix_expression)
        self.register_infix(TokenType.DOUBLE_ASTERISK, self._parse_infix_expression)

        self._next_token()
        self._next_token()

    def register_prefix(self, token_type, fn):
        self.prefix_parse_fns[token_type] = fn

    def register_infix(self, token_type, fn):
        self.infix_parse_fns[token_type] = fn

    def _peek_precedence(self):
        return PRECEDENCES.get(self.peek_token.type, Precedence.LOWEST)

    def _current_precedence(self):
        return PRECEDENCES.get(self.current_token.type, Precedence.LOWEST)

    def parse_expression(self, precedence):
        prefix = self.prefix_parse_fns.get(self.current_token.type)
        if not prefix:
            self.errors.append(f"No prefix parse function for {self.current_token.type} found")
            return None

        left_exp = prefix()

        while self.peek_token.type != TokenType.SEMICOLON and precedence < self._peek_precedence():
            infix = self.infix_parse_fns.get(self.peek_token.type)
            if not infix:
                return left_exp

            self._next_token()
            left_exp = infix(left_exp)

        return left_exp

    def _parse_identifier_value(self) -> BaseNode:
        import re
        literal = self.current_token.literal
        match = re.match(r"(-?\d+\.?\d*|-?\.\d+)(.*)", literal)
        if not match:
            return TextNode(value=literal)
        value_str, unit = match.groups()
        return NumericLiteralNode(value=float(value_str), unit=unit or None)

    def _parse_text_literal(self) -> TextNode:
        return TextNode(self.current_token.literal)

    def _parse_infix_expression(self, left: ExpressionNode) -> ExpressionNode:
        op = self.current_token.literal
        precedence = self._current_precedence()
        self._next_token()
        right = self.parse_expression(precedence)
        return InfixExpressionNode(left=left, operator=op, right=right)

    def _next_token(self):
        self.current_token = self.peek_token
        self.peek_token = self.lexer.get_next_token()

    def _expect_peek(self, token_type: TokenType) -> bool:
        if self.peek_token.type == token_type:
            self._next_token()
            return True
        else:
            self.errors.append(f"Expected next token to be {token_type}, got {self.peek_token.type} instead.")
            return False

    def parse_program(self) -> ElementNode:
        root = ElementNode(tag_name="root")
        while self.current_token.type != TokenType.EOF:
            statement = self.parse_statement()
            if statement:
                root.children.append(statement)
        return root

    def parse_statement(self) -> Union[BaseNode, None]:
        if self.current_token.type in (TokenType.IDENTIFIER, TokenType.DOT, TokenType.HASH, TokenType.AMPERSAND):
            # Check if it is a block statement
            if self.peek_token.type == TokenType.LBRACE:
                 return self.parse_block_statement()
        return None

    def parse_block_statement(self) -> Union[ElementNode, TextNode, StyleNode, None]:
        identifier = self.current_token.literal
        self._next_token() # Consume identifier
        self._next_token() # Consume '{'

        if identifier.lower() == 'text':
            return self._parse_text_body()
        elif identifier.lower() == 'style':
            return self._parse_style_body()
        else:
            return self._parse_element_body(identifier)

    def _parse_element_body(self, tag_name: str) -> ElementNode:
        node = ElementNode(tag_name=tag_name)
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type in (TokenType.COLON, TokenType.EQUALS):
                self._parse_attribute_statement(node)
            elif self.peek_token.type == TokenType.LBRACE:
                child_node = self.parse_block_statement()
                if child_node:
                    node.children.append(child_node)
            else:
                self.errors.append(f"Unexpected token in element block: {self.current_token.literal}")
                self._next_token()
        self._next_token() # Consume '}'
        return node

    def _parse_attribute_statement(self, node: ElementNode):
        attr_name = self.current_token.literal
        self._next_token() # Consume name
        self._next_token() # Consume ':' or '='

        if self.current_token.type in (TokenType.STRING, TokenType.IDENTIFIER, TokenType.HEX_LITERAL):
            node.attributes[attr_name] = self.current_token.literal
            self._next_token()
        else:
             self.errors.append(f"Expected attribute value for '{attr_name}', got {self.current_token.type}")

        if self.current_token.type == TokenType.SEMICOLON:
            self._next_token()
        else:
            self.errors.append(f"Missing semicolon ';' after attribute value for '{attr_name}'")

    def _skip_to_next_statement(self):
        while self.current_token.type not in (TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF):
            self._next_token()
        if self.current_token.type == TokenType.SEMICOLON:
            self._next_token()

    def _parse_style_body(self) -> StyleNode:
        inline_properties = {}
        css_rules = []
        auto_classes = []
        auto_ids = []
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type in (TokenType.DOT, TokenType.HASH, TokenType.AMPERSAND) or \
               (self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type == TokenType.LBRACE):
                rule, auto_class, auto_id = self._parse_css_rule()
                if rule: css_rules.append(rule)
                if auto_class: auto_classes.append(auto_class)
                if auto_id: auto_ids.append(auto_id)
            elif self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type == TokenType.COLON:
                prop_name, prop_value = self._parse_inline_css_property()
                if prop_name:
                    inline_properties[prop_name] = prop_value
            else:
                self.errors.append(f"Unexpected token in style block: {self.current_token.literal}")
                self._skip_to_next_statement()
        self._next_token() # Consume '}'
        return StyleNode(properties=inline_properties, rules=css_rules, auto_classes=auto_classes, auto_ids=auto_ids)

    def _parse_inline_css_property(self) -> Union[Tuple[str, list[BaseNode]], Tuple[None, None]]:
        prop_name = self.current_token.literal
        self._next_token() # consume property name
        self._next_token() # consume colon

        value_nodes = []
        # Loop until we hit the semicolon that terminates the property
        while self.current_token.type not in (TokenType.SEMICOLON, TokenType.RBRACE, TokenType.EOF):
            # In each iteration, parse one value, which could be a simple literal or a full expression.
            node = self.parse_expression(Precedence.LOWEST)
            if node:
                value_nodes.append(node)

            self._next_token() # Move to the next part of the value or the semicolon

        if self.current_token.type == TokenType.SEMICOLON:
            self._next_token() # Consume the semicolon
        else:
            self.errors.append(f"Missing semicolon for CSS property '{prop_name}'")

        return prop_name, value_nodes

    def _parse_css_rule(self) -> Union[Tuple[CssRule, str, str], Tuple[None, None, None]]:
        selector_tokens = []
        while self.current_token.type not in (TokenType.LBRACE, TokenType.EOF):
             selector_tokens.append(self.current_token)
             self._next_token()

        selector_string = "".join(t.literal for t in selector_tokens)
        auto_class, auto_id = (None, None)
        if len(selector_tokens) == 2:
            if selector_tokens[0].type == TokenType.DOT and selector_tokens[1].type == TokenType.IDENTIFIER:
                auto_class = selector_tokens[1].literal
            elif selector_tokens[0].type == TokenType.HASH and selector_tokens[1].type == TokenType.IDENTIFIER:
                auto_id = selector_tokens[1].literal

        self._next_token() # Consume '{'

        properties = {}
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type == TokenType.COLON:
                prop_name, prop_value = self._parse_inline_css_property()
                if prop_name:
                    properties[prop_name] = prop_value
            else:
                self.errors.append(f"Unexpected token in CSS rule body: {self.current_token.literal}")
                self._skip_to_next_statement()
        self._next_token() # Consume '}'

        rule = CssRule(selector=selector_string, properties=properties)
        return rule, auto_class, auto_id

    def _parse_text_body(self) -> TextNode:
        value_parts = []
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            value_parts.append(self.current_token.literal)
            self._next_token()
        value = " ".join(value_parts)
        self._next_token() # Consume '}'
        return TextNode(value=value)
