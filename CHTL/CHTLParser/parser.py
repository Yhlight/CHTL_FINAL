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
            next_word = self._peek(1).lexeme
            if next_word == "Template": return self._template_definition()
            if next_word == "Import": return self._import_statement()
            if next_word == "Custom": return self._custom_definition()
            if next_word == "Origin": return self._origin_statement()
        if self._check(TokenType.IDENTIFIER):
            return self._element_statement()
        if self._check(TokenType.EOF): return None
        raise self._error(self._peek(), "Expected an element, template, import, custom, or origin definition.")

    def _import_statement(self):
        self._consume(TokenType.LBRACKET, "[")
        self._consume(TokenType.IDENTIFIER, "'Import'")
        self._consume(TokenType.RBRACKET, "]")
        self._consume(TokenType.AT, "@")
        import_type = self._consume(TokenType.IDENTIFIER, "import type").lexeme
        self._consume(TokenType.IDENTIFIER, "'from'")
        path_token = self._advance()
        path = path_token.lexeme[1:-1] if path_token.type == TokenType.STRING else path_token.lexeme
        alias = None
        if self._match(TokenType.IDENTIFIER) and self._previous().lexeme == 'as':
            alias = self._consume(TokenType.IDENTIFIER, "alias name").lexeme
        self._consume(TokenType.SEMICOLON, ";")
        return nodes.ImportNode(import_type=import_type, path=path, alias=alias)

    def _template_definition(self):
        self._consume(TokenType.LBRACKET, "[")
        self._consume(TokenType.IDENTIFIER, "'Template'")
        self._consume(TokenType.RBRACKET, "]")
        self._consume(TokenType.AT, "@")
        template_type = self._consume(TokenType.IDENTIFIER, "template type").lexeme
        name = self._consume(TokenType.IDENTIFIER, "template name").lexeme
        body = []
        self._consume(TokenType.LBRACE, "{")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            body.append(self._style_property())
        self._consume(TokenType.RBRACE, "}")
        return nodes.TemplateDefinitionNode(template_type=template_type, name=name, body=body)

    def _custom_definition(self):
        self._consume(TokenType.LBRACKET, "[")
        self._consume(TokenType.IDENTIFIER, "'Custom'")
        self._consume(TokenType.RBRACKET, "]")
        self._consume(TokenType.AT, "@")
        custom_type = self._consume(TokenType.IDENTIFIER, "custom type").lexeme
        name = self._consume(TokenType.IDENTIFIER, "custom name").lexeme
        body = []
        self._consume(TokenType.LBRACE, "{")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._peek().lexeme == 'delete':
                body.append(self._delete_statement())
            else:
                body.append(self._style_property())
        self._consume(TokenType.RBRACE, "}")
        return nodes.CustomDefinitionNode(custom_type=custom_type, name=name, body=body)

    def _delete_statement(self):
        self._consume(TokenType.IDENTIFIER, "'delete'")
        targets = []
        while not self._check(TokenType.SEMICOLON) and not self._is_at_end():
            targets.append(self._consume(TokenType.IDENTIFIER, "delete target").lexeme)
            if not self._check(TokenType.COMMA):
                break
            self._consume(TokenType.COMMA, "','")
        self._consume(TokenType.SEMICOLON, "';'")
        return nodes.DeleteNode(targets=targets)

    def _insert_statement(self):
        self._consume(TokenType.IDENTIFIER, "'insert'")
        position = self._consume(TokenType.IDENTIFIER, "insert position").lexeme
        target = self._consume(TokenType.IDENTIFIER, "insert target").lexeme

        body = []
        self._consume(TokenType.LBRACE, "{")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            body.append(self._element_statement()) # Assuming elements are inserted
        self._consume(TokenType.RBRACE, "}")
        return nodes.InsertNode(position=position, target_selector=target, body=body)

    def _origin_statement(self):
        self._consume(TokenType.LBRACKET, "[")
        self._consume(TokenType.IDENTIFIER, "'Origin'")
        self._consume(TokenType.RBRACKET, "]")
        self._consume(TokenType.AT, "@")
        origin_type = self._consume(TokenType.IDENTIFIER, "origin type").lexeme
        name = None
        if self._peek().type == TokenType.IDENTIFIER:
            name = self._advance().lexeme

        self._consume(TokenType.LBRACE, "{")
        content_tokens = []
        brace_level = 1
        while not self._is_at_end():
            if self._peek().type == TokenType.LBRACE:
                brace_level += 1
            elif self._peek().type == TokenType.RBRACE:
                brace_level -= 1

            if brace_level == 0:
                break

            content_tokens.append(self._advance())

        content = "".join(t.lexeme for t in content_tokens)
        self._consume(TokenType.RBRACE, "}")
        return nodes.OriginNode(origin_type=origin_type, name=name, content=content)

    def _template_usage(self):
        self._consume(TokenType.AT, "@")
        template_type = self._consume(TokenType.IDENTIFIER, "template type").lexeme
        name = self._consume(TokenType.IDENTIFIER, "template name").lexeme

        body = None
        if self._match(TokenType.LBRACE):
            body = []
            while not self._check(TokenType.RBRACE) and not self._is_at_end():
                if self._peek().lexeme == 'delete':
                    body.append(self._delete_statement())
                # Placeholder for other specialization statements
            self._consume(TokenType.RBRACE, "}")
        else:
            self._consume(TokenType.SEMICOLON, ";")

        return nodes.TemplateUsageNode(template_type=template_type, name=name, specialization_body=body)

    def _element_statement(self):
        tag_name = self._consume(TokenType.IDENTIFIER, "element tag name").lexeme
        element = nodes.ElementNode(tag_name=tag_name)

        # Special handling for script tags to treat their content as a single block of text
        if tag_name.lower() == 'script':
            self._consume(TokenType.LBRACE, "{")

            # Find the start and end of the script content tokens
            content_start_index = self.current
            brace_level = 1
            content_end_index = -1

            i = self.current
            while i < len(self.tokens):
                token_type = self.tokens[i].type
                if token_type == TokenType.LBRACE: brace_level += 1
                elif token_type == TokenType.RBRACE: brace_level -= 1
                if brace_level == 0:
                    content_end_index = i
                    break
                i += 1

            if content_end_index != -1:
                content_tokens = self.tokens[content_start_index:content_end_index]
                # Reconstruct the raw string from the tokens' lexemes
                raw_content = "".join(t.lexeme for t in content_tokens)
                element.children.append(nodes.TextNode(value=raw_content))
                self.current = content_end_index # Move the parser's cursor

            self._consume(TokenType.RBRACE, "}")
            return element

        self._consume(TokenType.LBRACE, "{")
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.AT): element.children.append(self._template_usage())
            elif self._check(TokenType.STYLE): element.children.append(self._style_statement(element))
            elif self._check(TokenType.TEXT): element.children.append(self._text_statement())
            elif self._is_attribute(): element.attributes.append(self._attribute())
            elif self._check(TokenType.IDENTIFIER): element.children.append(self._element_statement())
            else: raise self._error(self._peek(), "Unexpected token inside element block.")
        self._consume(TokenType.RBRACE, "}")
        return element

    def _text_statement(self):
        self._consume(TokenType.TEXT, "'text'")
        self._consume(TokenType.LBRACE, "{")
        value_parts = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            value_parts.append(self._advance().lexeme)
        value = " ".join(value_parts).strip()
        self._consume(TokenType.RBRACE, "}")
        return nodes.TextNode(value=value)

    def _style_statement(self, parent_element: nodes.ElementNode):
        from CHTL.CHTLParser.expression_parser import ExpressionParser
        self._consume(TokenType.STYLE, "'style'")
        self._consume(TokenType.LBRACE, "{")
        style_node = nodes.StyleNode()
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            if self._check(TokenType.AT): style_node.children.append(self._template_usage())
            else:
                i = 0; is_selector = False
                while not self._is_at_end(offset=i):
                    token_type = self._peek(offset=i).type
                    if token_type == TokenType.LBRACE: is_selector = True; break
                    if token_type == TokenType.SEMICOLON: is_selector = False; break
                    i += 1
                if is_selector: style_node.children.append(self._style_selector_rule())
                else: style_node.children.append(self._style_property())
        self._consume(TokenType.RBRACE, "}")
        return style_node

    def _style_property(self):
        from CHTL.CHTLParser.expression_parser import ExpressionParser
        name = self._consume(TokenType.IDENTIFIER, "property name").lexeme
        self._consume(TokenType.COLON, "':'")
        value_tokens = []
        while not self._check(TokenType.SEMICOLON) and not self._is_at_end():
            value_tokens.append(self._advance())
        operator_types = {TokenType.PLUS, TokenType.MINUS, TokenType.STAR, TokenType.SLASH, TokenType.PERCENT, TokenType.QUESTION}
        if any(t.type in operator_types for t in value_tokens):
            expression_ast = ExpressionParser(value_tokens).parse()
        else:
            literal_value = " ".join(t.lexeme for t in value_tokens)
            expression_ast = nodes.LiteralNode(value=literal_value)
        self._consume(TokenType.SEMICOLON, "';'")
        return nodes.StylePropertyNode(name=name, value_expression=expression_ast)

    def _style_selector_rule(self):
        selector_tokens = []
        while not self._check(TokenType.LBRACE) and not self._is_at_end():
            selector_tokens.append(self._advance())
        self._consume(TokenType.LBRACE, "{")
        properties = []
        while not self._check(TokenType.RBRACE) and not self._is_at_end():
            properties.append(self._style_property())
        self._consume(TokenType.RBRACE, "}")
        return nodes.StyleSelectorRuleNode(selector_tokens=selector_tokens, properties=properties)

    def _attribute(self):
        name = self._consume(TokenType.IDENTIFIER, "attribute name").lexeme
        if not self._match(TokenType.EQUALS, TokenType.COLON):
            raise self._error(self._peek(), "'=' or ':'")
        if self._match(TokenType.STRING): value = self._previous().lexeme[1:-1]
        elif self._match(TokenType.IDENTIFIER): value = self._previous().lexeme
        else: raise self._error(self._peek(), "string or identifier.")
        self._consume(TokenType.SEMICOLON, "';'")
        return nodes.AttributeNode(name=name, value=value)

    def _is_attribute(self):
        return self._check(TokenType.IDENTIFIER) and self._peek(1).type in [TokenType.COLON, TokenType.EQUALS]
    def _consume(self, ttype: TokenType, message: str) -> Token:
        if self._check(ttype): return self._advance()
        raise self._error(self._peek(), f"Expect {message}")
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
