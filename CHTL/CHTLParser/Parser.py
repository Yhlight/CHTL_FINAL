from typing import Union, Tuple
from CHTL.CHTLLexer import Lexer, Token, TokenType
from CHTL.CHTLNode import (BaseNode, ElementNode, TextNode, StyleNode, CssRule, ScriptNode,
                           TemplateNode, TemplateUsageNode, FunctionCallNode,
                           ExpressionNode, InfixExpressionNode, NumericLiteralNode, PropertyReferenceNode)
from enum import IntEnum

# --- Expression Parsing Infrastructure ---
class Precedence(IntEnum):
    LOWEST = 1
    SUM = 2
    PRODUCT = 3
    POWER = 4
    PREFIX = 5
    MEMBER_ACCESS = 6 # .
    CALL = 7 # my_func(X)

PRECEDENCES = {
    TokenType.PLUS: Precedence.SUM,
    TokenType.MINUS: Precedence.SUM,
    TokenType.SLASH: Precedence.PRODUCT,
    TokenType.ASTERISK: Precedence.PRODUCT,
    TokenType.PERCENT: Precedence.PRODUCT,
    TokenType.DOUBLE_ASTERISK: Precedence.POWER,
    TokenType.DOT: Precedence.MEMBER_ACCESS,
    TokenType.LPAREN: Precedence.CALL,
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
        self.register_infix(TokenType.DOT, self._parse_property_reference_expression)
        self.register_infix(TokenType.LPAREN, self._parse_function_call)

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

    def _parse_property_reference_expression(self, left: BaseNode) -> ExpressionNode:
        """Parses a property reference like `selector.property`."""
        # The 'left' side is the selector part. The Pratt parser has already parsed it
        # into a node (e.g., a TextNode for 'box'). We need to convert this back
        # to the raw tokens for the generator. This is a limitation of this approach.
        # For now, we'll assume the selector is simple and its literal value is enough.
        # A more robust solution might require a dedicated selector parser.

        # We can't easily get the raw tokens of the `left` node.
        # Let's simplify and assume the selector is a single identifier.
        if not isinstance(left, TextNode):
            self.errors.append(f"Invalid selector on left side of '.': {left}")
            return None

        # The current token is the DOT. The next token should be the property name.
        if self.peek_token.type != TokenType.IDENTIFIER:
            self.errors.append(f"Expected property name after '.', got {self.peek_token.type}")
            return None

        self._next_token() # Consume the DOT
        property_name = self.current_token.literal

        # We need to store the selector tokens. We will have to reconstruct them.
        # This is hacky but will work for simple selectors.
        # Providing dummy positions as we don't have them here.
        selector_token = Token(TokenType.IDENTIFIER, left.value, -1, -1)

        return PropertyReferenceNode(selector_tokens=[selector_token], property_name=property_name)

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
            else:
                # If no statement was parsed, we must advance to the next token
                # to avoid an infinite loop on unrecognized tokens.
                self._next_token()
        return root

    def parse_statement(self) -> Union[BaseNode, None]:
        if self.current_token.type == TokenType.LBRACKET:
            return self._parse_template_definition()

        if self.current_token.type == TokenType.AT:
            return self._parse_template_usage()

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
        elif identifier.lower() == 'script':
            return self._parse_script_body()
        else:
            return self._parse_element_body(identifier)

    def _parse_element_body(self, tag_name: str) -> ElementNode:
        node = ElementNode(tag_name=tag_name)
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type == TokenType.AT:
                usage = self._parse_template_usage()
                if usage:
                    node.children.append(usage)
            elif self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type in (TokenType.COLON, TokenType.EQUALS):
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
        template_usages = []
        while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
            if self.current_token.type == TokenType.AT:
                usage = self._parse_template_usage()
                if usage:
                    template_usages.append(usage)
            elif self.current_token.type in (TokenType.DOT, TokenType.HASH, TokenType.AMPERSAND) or \
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
        return StyleNode(properties=inline_properties, rules=css_rules,
                         auto_classes=auto_classes, auto_ids=auto_ids,
                         template_usages=template_usages)

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

    def _parse_script_body(self) -> ScriptNode:
        attributes = {}
        content = ""

        if self.current_token.type == TokenType.RBRACE: # Empty script block
            self._next_token() # Consume '}'
            return ScriptNode()

        # Check if the block contains attributes (like src) or inline code
        if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type in (TokenType.COLON, TokenType.EQUALS):
            # Parsing attributes
            while self.current_token.type != TokenType.RBRACE and self.current_token.type != TokenType.EOF:
                if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type in (TokenType.COLON, TokenType.EQUALS):
                    attr_name = self.current_token.literal
                    self._next_token()  # consume name
                    self._next_token()  # consume colon/equals
                    if self.current_token.type in (TokenType.STRING, TokenType.IDENTIFIER):
                        literal = self.current_token.literal
                        if literal == 'true':
                            attributes[attr_name] = True
                        elif literal == 'false':
                            attributes[attr_name] = False
                        else:
                            attributes[attr_name] = literal
                        self._next_token()  # consume value
                    else:
                        self.errors.append(f"Expected script attribute value for '{attr_name}'")

                    if self.current_token.type == TokenType.SEMICOLON:
                        self._next_token()
                    else:
                        self.errors.append(f"Missing semicolon for script attribute '{attr_name}'")
                else:
                    self.errors.append(f"Unexpected token in script attribute block: {self.current_token.literal}")
                    self._next_token()
        else:
            # Parsing inline content
            start_pos = self.current_token.start_pos
            # Scan forward to find the end position without consuming the '}'
            temp_pos = self.lexer.pos
            temp_curr = self.lexer.current_char

            end_pos = start_pos
            # This is inefficient, as it re-lexes, but it's the simplest way
            # without a more complex parser lookahead buffer.
            temp_lexer = Lexer(self.lexer.text)
            temp_lexer.pos = self.current_token.start_pos
            temp_lexer.current_char = temp_lexer.text[temp_lexer.pos]

            temp_tok = self.current_token
            while temp_tok.type not in (TokenType.RBRACE, TokenType.EOF):
                end_pos = temp_tok.end_pos
                temp_tok = temp_lexer.get_next_token()

            content = self.lexer.text[start_pos:end_pos]

            # Now, advance the main lexer to the correct position
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                self._next_token()

        if self.current_token.type == TokenType.RBRACE:
            self._next_token()  # Consume '}'
        else:
            self.errors.append("Expected '}' to close script block.")

        return ScriptNode(attributes=attributes, content=content)

    def _parse_template_usage(self) -> TemplateUsageNode:
        # Expect @Style MyStyles;
        self._next_token() # Consume @

        if not self.current_token.type == TokenType.IDENTIFIER:
            self.errors.append("Expected template type (e.g., Style) after '@'.")
            return None
        template_type = self.current_token.literal
        self._next_token() # Consume type

        if not self.current_token.type == TokenType.IDENTIFIER:
            self.errors.append("Expected template name after type.")
            return None
        template_name = self.current_token.literal
        self._next_token() # Consume name

        if not self.current_token.type == TokenType.SEMICOLON:
            self.errors.append("Expected ';' after template usage.")
        else:
            self._next_token() # Consume ;

        return TemplateUsageNode(template_type=template_type, template_name=template_name)

    def _parse_template_definition(self) -> TemplateNode:
        # Current token is LBRACKET
        self._next_token() # Consume [

        if not (self.current_token.type == TokenType.IDENTIFIER and self.current_token.literal == 'Template'):
            self.errors.append("Expected 'Template' keyword after '['.")
            self._skip_to_next_statement()
            return None
        self._next_token() # Consume Template

        if self.current_token.type != TokenType.RBRACKET:
            self.errors.append("Expected ']' after Template keyword.")
            self._skip_to_next_statement()
            return None
        self._next_token() # Consume ]

        if self.current_token.type != TokenType.AT:
            self.errors.append("Expected '@' after [Template].")
            self._skip_to_next_statement()
            return None
        self._next_token() # Consume @

        if self.current_token.type != TokenType.IDENTIFIER:
            self.errors.append("Expected template type (e.g., Style) after '@'.")
            self._skip_to_next_statement()
            return None
        template_type = self.current_token.literal
        self._next_token() # Consume type

        if self.current_token.type != TokenType.IDENTIFIER:
            self.errors.append("Expected template name after type.")
            self._skip_to_next_statement()
            return None
        template_name = self.current_token.literal
        self._next_token() # Consume name

        if self.current_token.type != TokenType.LBRACE:
            self.errors.append("Expected '{' to start template body.")
            self._skip_to_next_statement()
            return None
        self._next_token() # Consume {

        # Parse body based on type
        content = None
        if template_type == 'Style':
            properties = {}
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type == TokenType.COLON:
                    prop_name, prop_value = self._parse_inline_css_property()
                    if prop_name:
                        properties[prop_name] = prop_value
                else:
                    self.errors.append(f"Unexpected token in template style block: {self.current_token.literal}")
                    self._skip_to_next_statement()
            content = properties
        elif template_type == 'Element':
            children = []
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                statement = self.parse_statement()
                if statement:
                    children.append(statement)
                else:
                    self.errors.append(f"Unexpected token in template element block: {self.current_token.literal}")
                    self._next_token() # Advance past the bad token
            content = children
        elif template_type == 'Var':
            variables = {}
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                if self.current_token.type == TokenType.IDENTIFIER and self.peek_token.type == TokenType.COLON:
                    var_name = self.current_token.literal
                    self._next_token()
                    self._next_token()
                    if self.current_token.type in (TokenType.STRING, TokenType.IDENTIFIER, TokenType.HEX_LITERAL):
                        variables[var_name] = self.current_token.literal
                        self._next_token()
                    else:
                        self.errors.append(f"Expected variable value for '{var_name}'")

                    if self.current_token.type == TokenType.SEMICOLON:
                        self._next_token()
                    else:
                        self.errors.append(f"Missing semicolon for variable '{var_name}'")
                else:
                    self.errors.append(f"Unexpected token in var group: {self.current_token.literal}")
                    self._skip_to_next_statement()
            print(f"DEBUG: Parsed vars: {variables}")
            content = variables
        else:
            self.errors.append(f"Unknown template type: {template_type}")
            # Skip the body
            while self.current_token.type not in (TokenType.RBRACE, TokenType.EOF):
                self._next_token()

        if self.current_token.type != TokenType.RBRACE:
            self.errors.append("Expected '}' to close template body.")
        else:
            self._next_token() # Consume '}'

        return TemplateNode(template_type=template_type, template_name=template_name, content=content)

    def _parse_function_call(self, function_node: ExpressionNode) -> FunctionCallNode:
        arguments = self._parse_call_arguments()
        return FunctionCallNode(function=function_node, arguments=arguments)

    def _parse_call_arguments(self) -> list[ExpressionNode]:
        args = []

        if self.peek_token.type == TokenType.RPAREN:
            self._next_token() # Consume ')'
            return args

        self._next_token() # Move to first argument
        args.append(self.parse_expression(Precedence.LOWEST))

        # The spec does not currently support multiple arguments for var groups.
        # while self.peek_token.type == TokenType.COMMA:
        #     self._next_token()
        #     self._next_token()
        #     args.append(self.parse_expression(Precedence.LOWEST))

        if not self._expect_peek(TokenType.RPAREN):
            return None

        return args
