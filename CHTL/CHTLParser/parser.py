from CHTL.CHTLLexer.token import Token
from CHTL.CHTLLexer.token_type import TokenType
from CHTL.CHTLNode import nodes

class ParseError(Exception): pass

class Parser:
    def __init__(self, tokens: list[Token]):
        self.tokens = tokens
        self.current = 0

    def parse(self) -> nodes.ProgramNode:
        program = nodes.ProgramNode()
        while not self._is_at_end():
            node = self._declaration()
            if node: program.children.append(node)
        return program

    def _declaration(self):
        if self._check(TokenType.LBRACKET):
            # Look ahead to see if it's [Template] or [Import]
            if self._peek(1).lexeme == "Template":
                return self._template_definition()
            elif self._peek(1).lexeme == "Import":
                return self._import_statement()
        if self._check(TokenType.IDENTIFIER):
            return self._element_statement()
        if self._check(TokenType.EOF): return None
        raise self._error(self._peek(), "Expected an element, template, or import definition.")

    def _import_statement(self):
        self._consume(TokenType.LBRACKET, "Expect '['.")
        self._consume(TokenType.IDENTIFIER, "Expect 'Import'.")
        self._consume(TokenType.RBRACKET, "Expect ']'.")

        self._consume(TokenType.AT, "Expect '@'.")
        import_type = self._consume(TokenType.IDENTIFIER, "Expect import type.").lexeme

        self._consume(TokenType.IDENTIFIER, "Expect 'from'.") # Expect 'from' keyword

        path_token = self._advance()
        if path_token.type not in [TokenType.STRING, TokenType.IDENTIFIER]:
            raise self._error(path_token, "Expect path string or literal.")
        path = path_token.lexeme[1:-1] if path_token.type == TokenType.STRING else path_token.lexeme

        alias = None
        if self._match(TokenType.IDENTIFIER) and self._previous().lexeme == 'as':
            alias = self._consume(TokenType.IDENTIFIER, "Expect alias name.").lexeme

        self._consume(TokenType.SEMICOLON, "Expect ';' to end import statement.")
        return nodes.ImportNode(import_type=import_type, path=path, alias=alias)

    def _template_definition(self):
        self._consume(TokenType.LBRACKET, "Expect '['.")
        self._consume(TokenType.IDENTIFIER, "Expect 'Template'.")
        self._consume(TokenType.RBRACKET, "Expect ']'.")
        self._consume(TokenType.AT, "Expect '@'.")
        template_type = self._consume(TokenType.IDENTIFIER, "Expect template type.").lexeme
        name = self._consume(TokenType.IDENTIFIER, "Expect template name.").lexeme
        body = []
        self._consume(TokenType.LBRACE, "Expect '{'.")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            body.append(self._style_property())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return nodes.TemplateDefinitionNode(template_type=template_type, name=name, body=body)

    def _template_usage(self):
        self._consume(TokenType.AT, "Expect '@'.")
        template_type = self._consume(TokenType.IDENTIFIER, "Expect template type.").lexeme
        name = self._consume(TokenType.IDENTIFIER, "Expect template name.").lexeme
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return nodes.TemplateUsageNode(template_type=template_type, name=name)

    def _element_statement(self):
        tag_name = self._consume(TokenType.IDENTIFIER, "Expect element tag name.").lexeme
        element = nodes.ElementNode(tag_name=tag_name)
        self._consume(TokenType.LBRACE, "Expect '{'.")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.AT):
                element.children.append(self._template_usage())
            elif self._check(TokenType.STYLE):
                element.children.append(self._style_statement(element))
            elif self._check(TokenType.TEXT):
                 element.children.append(self._text_statement())
            elif self._is_attribute():
                element.attributes.append(self._attribute())
            elif self._check(TokenType.IDENTIFIER):
                element.children.append(self._element_statement())
            else:
                raise self._error(self._peek(), "Unexpected token inside element block.")
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return element

    def _text_statement(self):
        self._consume(TokenType.TEXT, "Expect 'text'.")
        self._consume(TokenType.LBRACE, "Expect '{'.")
        value_parts = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            value_parts.append(self._advance().lexeme)
        value = " ".join(value_parts).strip()
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return nodes.TextNode(value=value)

    def _style_statement(self, parent_element: nodes.ElementNode):
        from CHTL.CHTLParser.expression_parser import ExpressionParser
        self._consume(TokenType.STYLE, "Expect 'style'.")
        self._consume(TokenType.LBRACE, "Expect '{'.")
        style_node = nodes.StyleNode()
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.AT):
                style_node.children.append(self._template_usage())
            else:
                i = 0; is_selector = False
                while not self._is_at_end(offset=i):
                    token_type = self._peek(offset=i).type
                    if token_type == TokenType.LBRACE: is_selector = True; break
                    if token_type == TokenType.SEMICOLON: is_selector = False; break
                    i += 1
                if is_selector: style_node.children.append(self._style_selector_rule())
                else: style_node.children.append(self._style_property())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return style_node

    def _style_property(self):
        from CHTL.CHTLParser.expression_parser import ExpressionParser
        name = self._consume(TokenType.IDENTIFIER, "Expect property name.").lexeme
        self._consume(TokenType.COLON, "Expect ':'.")
        value_tokens = []
        while not self._check(TokenType.SEMICOLON) and not self._is_at_end():
            value_tokens.append(self._advance())
        expression_ast = ExpressionParser(value_tokens).parse()
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return nodes.StylePropertyNode(name=name, value_expression=expression_ast)

    def _style_selector_rule(self):
        selector_tokens = []
        while not self._check(TokenType.LBRACE) and not self._is_at_end():
            selector_tokens.append(self._advance())
        self._consume(TokenType.LBRACE, "Expect '{'.")
        properties = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            properties.append(self._style_property())
        self._consume(TokenType.RBRACE, "Expect '}'.")
        return nodes.StyleSelectorRuleNode(selector_tokens=selector_tokens, properties=properties)

    def _attribute(self):
        name = self._consume(TokenType.IDENTIFIER, "Expect attribute name.").lexeme
        if not self._match(TokenType.EQUALS, TokenType.COLON):
            raise self._error(self._peek(), "Expect '=' or ':'.")
        if self._match(TokenType.STRING): value = self._previous().lexeme[1:-1]
        elif self._match(TokenType.IDENTIFIER): value = self._previous().lexeme
        else: raise self._error(self._peek(), "Expect string or identifier.")
        self._consume(TokenType.SEMICOLON, "Expect ';'.")
        return nodes.AttributeNode(name=name, value=value)

    def _is_attribute(self):
        return self._check(TokenType.IDENTIFIER) and self._peek(1).type in [TokenType.COLON, TokenType.EQUALS]

    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype): return self._advance()
        raise self._error(self._peek(), message)
    def _check(self, ttype: TokenType) -> bool:
        if self._is_at_end(): return False
        return self._peek().type == ttype
    def _is_at_end(self, offset=0) -> bool: return self.current + offset >= len(self.tokens) - 1
    def _peek(self, offset=0) -> Token: return self.tokens[self.current + offset]
    def _previous(self) -> Token: return self.tokens[self.current - 1]
    def _advance(self) -> Token:
        if not self._is_at_end(): self.current += 1
        return self._previous()
    def _match(self, *types: TokenType) -> bool:
        if self._is_at_end(): return False
        for ttype in types:
            if self._peek().type == ttype:
                self._advance(); return True
        return False
    def _error(self, token: Token, message: str) -> ParseError:
        return ParseError(f"[Line {token.line}] Error at '{token.lexeme}': {message}")
