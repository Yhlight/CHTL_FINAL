from CHTL.CHTLLexer.keywords import Token, TokenType
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode, LiteralNode,
    ImportNode, ConfigNode, OriginNode
)
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.expression_parser import ExpressionParser
from typing import List, Optional, Union

class Parser:
    def __init__(self, tokens: List[Token], source_code: str, context: CompilationContext):
        self.tokens = tokens
        self.source_code = source_code
        self.pos = 0
        self.context = context

    def peek(self, offset: int = 0) -> Optional[Token]:
        if self.pos + offset < len(self.tokens):
            return self.tokens[self.pos + offset]
        return None

    def current_token(self) -> Optional[Token]:
        return self.peek(0)

    def consume(self, expected_type: Optional[TokenType] = None, expected_value: Optional[str] = None) -> Token:
        token = self.current_token()
        if not token:
            raise RuntimeError("Unexpected end of token stream.")
        if expected_type and token.type != expected_type:
            raise RuntimeError(f"Line {token.lineno}: Expected token {expected_type}, but got {token.type} ('{token.value}')")
        if expected_value and token.value != expected_value:
            raise RuntimeError(f"Line {token.lineno}: Expected '{expected_value}', but got '{token.value}'")
        self.pos += 1
        return token

    def parse(self) -> DocumentNode:
        doc = DocumentNode(lineno=1)
        while self.current_token() and self.current_token().type != TokenType.EOF:
            token = self.current_token()
            if token.type == TokenType.LBRACK:
                self.consume(TokenType.LBRACK)
                keyword_token = self.consume()
                if keyword_token.type == TokenType.TEMPLATE:
                    doc.add_child(self.parse_template_definition(keyword_token))
                elif keyword_token.type == TokenType.CUSTOM:
                    doc.add_child(self.parse_template_definition(keyword_token, is_custom=True))
                elif keyword_token.type == TokenType.IMPORT:
                    doc.add_child(self.parse_import_statement(keyword_token))
                elif keyword_token.type == TokenType.ORIGIN:
                    doc.add_child(self.parse_origin_block(keyword_token))
                elif keyword_token.type == TokenType.CONFIGURATION:
                    doc.add_child(self.parse_configuration_block(keyword_token))
                else:
                    raise RuntimeError(f"Unknown definition type: {keyword_token.value}")
            elif token.type == TokenType.COMMENT:
                doc.add_child(self.parse_comment())
            elif token.type == TokenType.STYLE:
                doc.add_child(self.parse_style_block(parent_element=None))
            elif token.type == TokenType.IDENTIFIER:
                if token.value == 'title' and self.peek(1) and self.peek(1).type == TokenType.COLON:
                    doc.add_child(self.parse_title_shorthand())
                else:
                    doc.add_child(self.parse_element())
            else:
                raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') at top level.")
        return doc

    def parse_title_shorthand(self) -> ElementNode:
        title_token = self.consume(TokenType.IDENTIFIER, 'title')
        self.consume(TokenType.COLON)
        value_token = self.consume(TokenType.STRING)
        value = value_token.value[1:-1]
        title_element = ElementNode(tag_name='title', lineno=title_token.lineno)
        title_element.add_child(TextNode(value=value, lineno=value_token.lineno))
        if self.current_token() and self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
        return title_element

    def parse_definition_content(self, template_type: str):
        if template_type == 'Element':
            return self.parse_element_template_content()
        elif template_type == 'Style':
            return self.parse_style_template_content()
        elif template_type == 'Var':
            return self.parse_var_template_content()
        else:
            raise RuntimeError(f"Unknown template type for definition: {template_type}")

    def parse_element_template_content(self) -> List[ElementNode]:
        content = []
        self.consume(TokenType.LBRACE)
        while self.current_token().type != TokenType.RBRACE:
            content.append(self.parse_element())
        self.consume(TokenType.RBRACE)
        return content

    def parse_style_template_content(self) -> List[CssPropertyNode]:
        content = []
        self.consume(TokenType.LBRACE)
        while self.current_token().type != TokenType.RBRACE:
            content.append(self.parse_css_property())
        self.consume(TokenType.RBRACE)
        return content

    def parse_var_template_content(self) -> List[AttributeNode]:
        content = []
        self.consume(TokenType.LBRACE)
        while self.current_token().type != TokenType.RBRACE:
            attr_name_token = self.consume(TokenType.IDENTIFIER)
            self.consume()
            value = self.parse_value_tokens([TokenType.SEMICOLON, TokenType.RBRACE])
            content.append(AttributeNode(name=attr_name_token.value, value=value, lineno=attr_name_token.lineno))
            if self.current_token().type == TokenType.SEMICOLON:
                self.consume(TokenType.SEMICOLON)
        self.consume(TokenType.RBRACE)
        return content

    def parse_template_definition(self, start_token, is_custom=False) -> Union[TemplateDefinitionNode, CustomDefinitionNode]:
        self.consume(TokenType.RBRACK)
        self.consume(TokenType.AT_SYMBOL)
        template_type_token = self.consume(TokenType.IDENTIFIER)
        template_name_token = self.consume(TokenType.IDENTIFIER)
        template_type = template_type_token.value
        template_name = template_name_token.value
        content = self.parse_definition_content(template_type)
        node_class = CustomDefinitionNode if is_custom else TemplateDefinitionNode
        return node_class(template_type=template_type, name=template_name, content=content, lineno=start_token.lineno)

    def parse_origin_block(self, start_token: Token) -> OriginNode:
        self.consume(TokenType.RBRACK)
        self.consume(TokenType.AT_SYMBOL)
        origin_type = self.consume(TokenType.IDENTIFIER).value

        name = None
        content = ""

        if self.current_token().type != TokenType.LBRACE:
            name = self.consume(TokenType.IDENTIFIER).value

        if self.current_token().type == TokenType.LBRACE:
            open_brace_token = self.consume(TokenType.LBRACE)
            content_start_index = open_brace_token.char_offset + len(open_brace_token.value)

            brace_level = 1
            closing_brace_token = None
            while brace_level > 0:
                token = self.consume()
                if token.type == TokenType.LBRACE:
                    brace_level += 1
                elif token.type == TokenType.RBRACE:
                    brace_level -= 1
                    if brace_level == 0:
                        closing_brace_token = token
                        break
                elif token.type == TokenType.EOF:
                    raise RuntimeError(f"Unmatched opening brace for [Origin] block starting at line {start_token.lineno}")

            if closing_brace_token:
                content_end_index = closing_brace_token.char_offset
                content = self.source_code[content_start_index:content_end_index]
        else:
            self.consume(TokenType.SEMICOLON)

        return OriginNode(origin_type=origin_type, name=name, content=content, lineno=start_token.lineno)

    def parse_configuration_block(self, start_token: Token) -> ConfigNode:
        self.consume(TokenType.RBRACK)
        self.consume(TokenType.LBRACE)

        config_data = {}
        while self.current_token().type != TokenType.RBRACE:
            key_token = self.consume() # Key can be a keyword or identifier
            self.consume(TokenType.COLON)
            value_token = self.consume(TokenType.STRING)

            key = key_token.value
            value = value_token.value[1:-1] # Remove quotes
            config_data[key] = value

            if self.current_token().type == TokenType.COMMA:
                self.consume(TokenType.COMMA)

        self.consume(TokenType.RBRACE)

        # Update the context
        self.context.set_keyword_config(config_data)

        return ConfigNode(lineno=start_token.lineno, config=config_data)

    def parse_at_usage(self):
        start_token = self.consume(TokenType.AT_SYMBOL)
        template_type_token = self.consume(TokenType.IDENTIFIER)
        template_name_token = self.consume(TokenType.IDENTIFIER)
        from_namespace = None
        if self.current_token().type == TokenType.FROM:
            self.consume(TokenType.FROM)
            namespace_parts = [self.consume(TokenType.IDENTIFIER).value]
            while self.current_token().type == TokenType.DOT:
                self.consume(TokenType.DOT)
                namespace_parts.append(self.consume(TokenType.IDENTIFIER).value)
            from_namespace = ".".join(namespace_parts)
        if self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
            return TemplateUsageNode(
                template_type=template_type_token.value,
                name=template_name_token.value,
                from_namespace=from_namespace,
                lineno=start_token.lineno
            )
        else:
            body = self.parse_custom_usage_body()
            return CustomUsageNode(
                template_type=template_type_token.value,
                name=template_name_token.value,
                from_namespace=from_namespace,
                body=body,
                lineno=start_token.lineno
            )

    def parse_custom_usage_body(self):
        body = []
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            token = self.current_token()
            if token.type == TokenType.COMMENT:
                self.consume()
                continue
            elif token.type == TokenType.DELETE:
                body.append(self.parse_delete_rule())
            elif token.type == TokenType.INSERT:
                body.append(self.parse_insert_rule())
            else:
                body.append(self.parse_element())
        self.consume(TokenType.RBRACE)
        return body

    def parse_delete_rule(self) -> DeleteNode:
        start_token = self.consume(TokenType.DELETE)
        targets = []
        while self.current_token().type != TokenType.SEMICOLON:
            if self.current_token().type == TokenType.COMMA: self.consume()
            targets.append(self.consume().value)
        self.consume(TokenType.SEMICOLON)
        return DeleteNode(targets=targets, lineno=start_token.lineno)

    def parse_insert_rule(self) -> InsertNode:
        start_token = self.consume(TokenType.INSERT)
        position_token = self.consume()
        position_parts = [position_token.value]
        if position_token.type == TokenType.AT_KEYWORD:
            position_parts.append(self.consume().value)
        position = " ".join(position_parts)
        target_selector = self.consume(TokenType.IDENTIFIER).value
        content = self.parse_element_template_content()
        return InsertNode(position=position, target_selector=target_selector, content=content, lineno=start_token.lineno)

    def parse_import_statement(self, start_token: Token) -> ImportNode:
        self.consume(TokenType.RBRACK)
        is_construct_import = self.current_token().type == TokenType.LBRACK
        construct_type = None
        if is_construct_import:
            self.consume(TokenType.LBRACK)
            construct_type = self.consume().value
            self.consume(TokenType.RBRACK)
        self.consume(TokenType.AT_SYMBOL)
        import_type = self.consume().value
        imported_item = None
        if self.current_token().type != TokenType.FROM:
            imported_item = self.consume().value
        self.consume(TokenType.FROM)
        path_token = self.current_token()
        if path_token.type not in (TokenType.STRING, TokenType.UNQUOTED_LITERAL):
             raise RuntimeError(f"Line {path_token.lineno}: Expected a path string or literal for import.")
        self.consume()
        path = path_token.value
        if path_token.type == TokenType.STRING:
            path = path[1:-1]
        alias = None
        if self.current_token() and self.current_token().type == TokenType.AS:
            self.consume(TokenType.AS)
            alias_token = self.consume(TokenType.IDENTIFIER)
            alias = alias_token.value
        if self.current_token() and self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
        return ImportNode(lineno=start_token.lineno, import_type=import_type, construct_type=construct_type, path=path, alias=alias, imported_item=imported_item)

    def parse_comment(self) -> CommentNode:
        token = self.consume(TokenType.COMMENT)
        cleaned_value = token.value.strip()
        return CommentNode(value=cleaned_value, lineno=token.lineno)

    def parse_value_tokens(self, terminators: List[TokenType]) -> str:
        if self.current_token().type == TokenType.STRING:
            token = self.consume(TokenType.STRING)
            return token.value[1:-1]
        value_parts = []
        while self.current_token() and self.current_token().type not in terminators:
            token = self.consume()
            value_parts.append(token.value)
        return " ".join(value_parts).strip()

    def parse_element(self) -> ElementNode:
        tag_token = self.consume(TokenType.IDENTIFIER)
        element = ElementNode(tag_name=tag_token.value, lineno=tag_token.lineno)
        if self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
            return element
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type not in (TokenType.RBRACE, TokenType.EOF):
            token = self.current_token()
            if token.type == TokenType.AT_SYMBOL:
                element.add_child(self.parse_at_usage())
            elif token.type == TokenType.COMMENT:
                element.add_child(self.parse_comment())
            elif token.type == TokenType.STYLE:
                element.add_child(self.parse_style_block(element))
            elif token.type == TokenType.IDENTIFIER or token.type == TokenType.TEXT:
                next_token = self.peek(1)
                if next_token and next_token.type == TokenType.LBRACE:
                    element.add_child(self.parse_element())
                elif next_token and next_token.type in (TokenType.COLON, TokenType.EQUALS):
                     self.parse_attribute(element)
                else:
                    element.add_child(self.parse_element())
            else:
                 raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') inside element '{element.tag_name}'")
        self.consume(TokenType.RBRACE)
        return element

    def parse_attribute(self, element: ElementNode):
        attr_name_token = self.consume()
        self.consume()
        if attr_name_token.type == TokenType.TEXT:
             value = self.parse_value_tokens([TokenType.SEMICOLON, TokenType.RBRACE])
             element.add_child(TextNode(value=value, lineno=attr_name_token.lineno))
        else:
             if attr_name_token.type != TokenType.IDENTIFIER:
                 raise RuntimeError(f"Unexpected token {attr_name_token.type} for an attribute name.")
             value = self.parse_value_tokens([TokenType.SEMICOLON, TokenType.RBRACE])
             element.attributes.append(AttributeNode(name=attr_name_token.value, value=value, lineno=attr_name_token.lineno))
        if self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)

    def parse_style_block(self, parent_element: Optional[ElementNode]) -> StyleNode:
        style_token = self.consume(TokenType.STYLE)
        self.consume(TokenType.LBRACE)
        style_node = StyleNode(lineno=style_token.lineno)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            if self.current_token().type == TokenType.AT_SYMBOL:
                style_node.add_child(self.parse_at_usage())
            else:
                is_rule = False
                i = 0
                while True:
                    peek_token = self.peek(i)
                    if not peek_token or peek_token.type in (TokenType.SEMICOLON, TokenType.RBRACE):
                        is_rule = False; break
                    if peek_token.type == TokenType.LBRACE:
                        is_rule = True; break
                    i += 1
                if is_rule:
                    style_node.children.append(self.parse_css_rule(parent_element))
                else:
                    style_node.children.append(self.parse_css_property())
        self.consume(TokenType.RBRACE)
        return style_node

    def parse_css_property(self) -> CssPropertyNode:
        name_token = self.consume(TokenType.IDENTIFIER)
        self.consume(TokenType.COLON)
        expression_tokens = []
        while self.current_token() and self.current_token().type not in (TokenType.SEMICOLON, TokenType.RBRACE):
            expression_tokens.append(self.consume())
        if not expression_tokens:
            raise RuntimeError(f"Line {name_token.lineno}: Missing value for CSS property '{name_token.value}'")
        if len(expression_tokens) == 1 and expression_tokens[0].type in (TokenType.IDENTIFIER, TokenType.UNQUOTED_LITERAL, TokenType.STRING):
            token = expression_tokens[0]
            value = token.value[1:-1] if token.type == TokenType.STRING else token.value
            value_node = LiteralNode(value=value, lineno=token.lineno)
        else:
            expr_parser = ExpressionParser(expression_tokens)
            value_node = expr_parser.parse()
        if self.current_token() and self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
        return CssPropertyNode(name=name_token.value, value=value_node, lineno=name_token.lineno)

    def parse_css_rule(self, parent_element: Optional[ElementNode]) -> CssRuleNode:
        selector_parts = []
        rule_lineno = self.current_token().lineno
        while self.current_token() and self.current_token().type != TokenType.LBRACE:
            selector_parts.append(self.consume().value)
        selector = "".join(selector_parts).strip()
        if parent_element:
            if selector.startswith('.'):
                class_name = selector.split(':')[0].split(' ')[0][1:]
                found = False
                for attr in parent_element.attributes:
                    if attr.name == 'class':
                        if class_name not in attr.value.split(): attr.value += f" {class_name}"
                        found = True; break
                if not found: parent_element.attributes.append(AttributeNode(name='class', value=class_name))
            elif selector.startswith('#'):
                id_name = selector.split(':')[0].split(' ')[0][1:]
                found = False
                for attr in parent_element.attributes:
                    if attr.name == 'id':
                        attr.value = id_name; found = True; break
                if not found: parent_element.attributes.append(AttributeNode(name='id', value=id_name))
        rule_node = CssRuleNode(selector=selector, lineno=rule_lineno)
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            rule_node.properties.append(self.parse_css_property())
        self.consume(TokenType.RBRACE)
        return rule_node
