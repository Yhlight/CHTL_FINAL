from CHTL.CHTLLexer.lexer import Token, TokenType
from CHTL.CHTLNode.nodes import (
    BaseNode, DocumentNode, ElementNode, AttributeNode, TextNode, CommentNode, Node,
    StyleNode, CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode, LiteralNode, OriginNode,
    ImportNode, NamespaceNode
)
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.expression_parser import ExpressionParser
from typing import List, Optional, Union

class Parser:
    def __init__(self, tokens: List[Token], context: CompilationContext):
        self.tokens = tokens
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
        if not token: raise RuntimeError("Unexpected end of token stream.")
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
                keyword_token = self.peek(1)
                if not keyword_token: raise RuntimeError("Unexpected end of file after '['")

                keyword = keyword_token.value
                if keyword == "Origin": doc.add_child(self.parse_origin_block())
                elif keyword == "Import": doc.add_child(self.parse_import_statement())
                elif keyword == "Namespace": doc.add_child(self.parse_namespace_block())
                elif keyword in ("Template", "Custom"):
                    self.consume(TokenType.LBRACK)
                    keyword_tok = self.consume(TokenType.IDENTIFIER)
                    doc.add_child(self.parse_template_definition(keyword_tok, is_custom=(keyword == "Custom")))
                else:
                    raise RuntimeError(f"Unknown definition type: {keyword}")
            elif token.type == TokenType.COMMENT:
                doc.add_child(self.parse_comment())
            elif token.type == TokenType.IDENTIFIER:
                if token.value == 'title' and self.peek(1) and self.peek(1).type == TokenType.COLON:
                    doc.add_child(self.parse_title_shorthand())
                elif token.value == 'style' and self.peek(1) and self.peek(1).type == TokenType.LBRACE:
                    doc.add_child(self.parse_style_block(parent_element=None))
                else:
                    doc.add_child(self.parse_element())
            else:
                raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') at top level.")
        return doc

    def parse_import_statement(self) -> ImportNode:
        start_token = self.consume(TokenType.LBRACK)
        self.consume(TokenType.IDENTIFIER, "Import")
        self.consume(TokenType.RBRACK) # This was missing

        type_parts = []
        if self.current_token().type == TokenType.LBRACK:
            self.consume(TokenType.LBRACK)
            type_parts.append(f"[{self.consume(TokenType.IDENTIFIER).value}]")
            self.consume(TokenType.RBRACK)

        if self.current_token().type == TokenType.AT:
            self.consume(TokenType.AT)
            type_parts.append(f"@{self.consume(TokenType.IDENTIFIER).value}")

        item_name = None
        if self.current_token().type == TokenType.IDENTIFIER and self.peek(1) and self.peek(1).value == 'from':
            item_name = self.consume(TokenType.IDENTIFIER).value

        self.consume(TokenType.IDENTIFIER, "from")

        path_token = self.consume()
        if path_token.type not in (TokenType.STRING, TokenType.UNQUOTED_LITERAL, TokenType.IDENTIFIER):
            raise RuntimeError(f"Expected file path, got {path_token.type}")
        file_path = path_token.value.strip('"\'')

        alias = None
        if self.current_token().type == TokenType.IDENTIFIER and self.current_token().value == 'as':
            self.consume(TokenType.IDENTIFIER, "as")
            alias = self.consume(TokenType.IDENTIFIER).value

        # This was also missing from the original logic for some reason
        # self.consume(TokenType.RBRACK)
        return ImportNode(import_type_parts=type_parts, imported_item_name=item_name, file_path=file_path, alias=alias, lineno=start_token.lineno)

    def parse_namespace_block(self) -> NamespaceNode:
        start_token = self.consume(TokenType.LBRACK)
        self.consume(TokenType.IDENTIFIER, "Namespace")
        self.consume(TokenType.RBRACK)
        name = self.consume(TokenType.IDENTIFIER).value

        node = NamespaceNode(name=name, lineno=start_token.lineno)
        if self.current_token().type == TokenType.LBRACE:
            self.consume(TokenType.LBRACE)
            while self.current_token() and self.current_token().type != TokenType.RBRACE:
                node.children.append(self.parse_element())
            self.consume(TokenType.RBRACE)
        return node

    def parse_origin_block(self) -> OriginNode:
        start_token = self.consume(TokenType.LBRACK); self.consume(TokenType.IDENTIFIER, "Origin"); self.consume(TokenType.RBRACK)
        origin_type = self.consume(TokenType.IDENTIFIER).value
        name = self.consume(TokenType.IDENTIFIER).value if self.current_token().type == TokenType.IDENTIFIER else None
        self.consume(TokenType.LBRACE)
        content_token = self.consume(TokenType.RAW_CONTENT)
        self.consume(TokenType.RBRACE)
        return OriginNode(origin_type=origin_type, name=name, content=content_token.value, lineno=start_token.lineno)

    def parse_at_usage(self):
        start_token = self.consume(TokenType.AT)
        template_type = self.consume(TokenType.IDENTIFIER).value
        template_name = self.consume(TokenType.IDENTIFIER).value

        namespace_from = None
        if self.current_token().type == TokenType.IDENTIFIER and self.current_token().value == 'from':
            self.consume(TokenType.IDENTIFIER, 'from')
            namespace_parts = [self.consume(TokenType.IDENTIFIER).value]
            while self.current_token().type == TokenType.DOT:
                self.consume(TokenType.DOT)
                namespace_parts.append(self.consume(TokenType.IDENTIFIER).value)
            namespace_from = namespace_parts

        if self.current_token().type == TokenType.SEMICOLON:
            self.consume(TokenType.SEMICOLON)
            return TemplateUsageNode(template_type=template_type, name=template_name, namespace_from=namespace_from, lineno=start_token.lineno)
        else:
            body = self.parse_custom_usage_body()
            return CustomUsageNode(template_type=template_type, name=template_name, body=body, lineno=start_token.lineno)

    def parse_title_shorthand(self) -> ElementNode:
        title_token = self.consume(TokenType.IDENTIFIER, 'title'); self.consume(TokenType.COLON)
        value = self.consume(TokenType.STRING).value[1:-1]
        title_element = ElementNode(tag_name='title', lineno=title_token.lineno)
        title_element.add_child(TextNode(value=value, lineno=title_token.lineno))
        if self.current_token() and self.current_token().type == TokenType.SEMICOLON: self.consume(TokenType.SEMICOLON)
        return title_element
    def parse_definition_content(self, template_type):
        content = []; self.consume(TokenType.LBRACE)
        while self.current_token().type != TokenType.RBRACE: content.append(self.parse_element())
        self.consume(TokenType.RBRACE); return content
    def parse_template_definition(self, start_token, is_custom=False) -> Union[TemplateDefinitionNode, CustomDefinitionNode]:
        self.consume(TokenType.RBRACK); self.consume(TokenType.AT)
        template_type = self.consume(TokenType.IDENTIFIER).value
        template_name = self.consume(TokenType.IDENTIFIER).value
        content = self.parse_definition_content(template_type)
        if template_type == 'Element': self.context.add_element_template(template_name, content)
        node_class = CustomDefinitionNode if is_custom else TemplateDefinitionNode
        return node_class(template_type=template_type, name=template_name, content=content, lineno=start_token.lineno)
    def parse_custom_usage_body(self):
        body = []; self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            token = self.current_token()
            if token.type == TokenType.IDENTIFIER and token.value == 'delete': body.append(self.parse_delete_rule())
            elif token.type == TokenType.IDENTIFIER and token.value == 'insert': body.append(self.parse_insert_rule())
            else: body.append(self.parse_element())
        self.consume(TokenType.RBRACE); return body
    def parse_delete_rule(self) -> DeleteNode:
        start_token = self.consume(TokenType.IDENTIFIER, 'delete'); targets = []
        while self.current_token().type != TokenType.SEMICOLON:
            if self.current_token().type == TokenType.COMMA: self.consume()
            targets.append(self.consume().value)
        self.consume(TokenType.SEMICOLON); return DeleteNode(targets=targets, lineno=start_token.lineno)
    def parse_insert_rule(self) -> InsertNode:
        start_token = self.consume(TokenType.IDENTIFIER, 'insert')
        position_parts = [self.consume(TokenType.IDENTIFIER).value]
        if position_parts[0] == 'at': position_parts.append(self.consume(TokenType.IDENTIFIER).value)
        position = " ".join(position_parts); target_selector = self.consume(TokenType.IDENTIFIER).value
        content = self.parse_definition_content('Element')
        return InsertNode(position=position, target_selector=target_selector, content=content, lineno=start_token.lineno)
    def parse_comment(self) -> CommentNode:
        token = self.consume(TokenType.COMMENT); return CommentNode(value=token.value.strip(), lineno=token.lineno)
    def parse_element(self) -> ElementNode:
        tag_token = self.consume(TokenType.IDENTIFIER); element = ElementNode(tag_name=tag_token.value, lineno=tag_token.lineno)
        if self.current_token().type == TokenType.SEMICOLON: self.consume(TokenType.SEMICOLON); return element
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type not in (TokenType.RBRACE, TokenType.EOF):
            token = self.current_token()
            if token.type == TokenType.AT: element.add_child(self.parse_at_usage())
            elif token.type == TokenType.COMMENT: element.add_child(self.parse_comment())
            elif token.type == TokenType.LBRACK and self.peek(1) and self.peek(1).value == "Origin": element.add_child(self.parse_origin_block())
            elif token.type == TokenType.IDENTIFIER and token.value == 'style': element.add_child(self.parse_style_block(element))
            elif token.type == TokenType.IDENTIFIER:
                next_token = self.peek(1)
                if next_token and next_token.type == TokenType.LBRACE: element.add_child(self.parse_element())
                elif next_token and next_token.type in (TokenType.COLON, TokenType.EQUALS): self.parse_attribute(element)
                else: element.add_child(self.parse_element())
            else: raise RuntimeError(f"Line {token.lineno}: Unexpected token {token.type} ('{token.value}') inside element '{element.tag_name}'")
        self.consume(TokenType.RBRACE); return element
    def parse_attribute(self, element: ElementNode):
        attr_name_token = self.consume(TokenType.IDENTIFIER); self.consume()
        value = self.parse_value_tokens([TokenType.SEMICOLON, TokenType.RBRACE])
        if attr_name_token.value == 'text': element.add_child(TextNode(value=value, lineno=attr_name_token.lineno))
        else: element.attributes.append(AttributeNode(name=attr_name_token.value, value=value, lineno=attr_name_token.lineno))
        if self.current_token().type == TokenType.SEMICOLON: self.consume(TokenType.SEMICOLON)
    def parse_style_block(self, parent_element: Optional[ElementNode]) -> StyleNode:
        style_token = self.consume(TokenType.IDENTIFIER); self.consume(TokenType.LBRACE)
        style_node = StyleNode(lineno=style_token.lineno)
        while self.current_token() and self.current_token().type != TokenType.RBRACE:
            if self.current_token().type == TokenType.AT: style_node.add_child(self.parse_at_usage())
            else:
                is_rule = False; i = 0
                while True:
                    peek_token = self.peek(i)
                    if not peek_token or peek_token.type in (TokenType.SEMICOLON, TokenType.RBRACE): is_rule = False; break
                    if peek_token.type == TokenType.LBRACE: is_rule = True; break
                    i += 1
                if is_rule: style_node.children.append(self.parse_css_rule(parent_element))
                else: style_node.children.append(self.parse_css_property())
        self.consume(TokenType.RBRACE); return style_node
    def parse_css_property(self) -> CssPropertyNode:
        name_token = self.consume(TokenType.IDENTIFIER); self.consume(TokenType.COLON)
        expression_tokens = []
        while self.current_token() and self.current_token().type not in (TokenType.SEMICOLON, TokenType.RBRACE): expression_tokens.append(self.consume())
        if not expression_tokens: raise RuntimeError(f"Line {name_token.lineno}: Missing value for CSS property '{name_token.value}'")
        if len(expression_tokens) == 1 and expression_tokens[0].type in (TokenType.IDENTIFIER, TokenType.UNQUOTED_LITERAL, TokenType.STRING):
            token = expression_tokens[0]
            value = token.value[1:-1] if token.type == TokenType.STRING else token.value
            value_node = LiteralNode(value=value, lineno=token.lineno)
        else:
            expr_parser = ExpressionParser(expression_tokens); value_node = expr_parser.parse()
        if self.current_token() and self.current_token().type == TokenType.SEMICOLON: self.consume(TokenType.SEMICOLON)
        return CssPropertyNode(name=name_token.value, value=value_node, lineno=name_token.lineno)
    def parse_css_rule(self, parent_element: Optional[ElementNode]) -> CssRuleNode:
        selector_parts = []; rule_lineno = self.current_token().lineno
        while self.current_token() and self.current_token().type != TokenType.LBRACE: selector_parts.append(self.consume().value)
        selector = "".join(selector_parts).strip()
        if parent_element:
            if selector.startswith('.'):
                class_name = selector.split(':')[0].split(' ')[0][1:]
                found = False
                for attr in parent_element.attributes:
                    if attr.name == 'class':
                        if class_name not in attr.value.split(): attr.value += f" {class_name}"; found = True; break
                if not found: parent_element.attributes.append(AttributeNode(name='class', value=class_name))
            elif selector.startswith('#'):
                id_name = selector.split(':')[0].split(' ')[0][1:]
                found = False
                for attr in parent_element.attributes:
                    if attr.name == 'id': attr.value = id_name; found = True; break
                if not found: parent_element.attributes.append(AttributeNode(name='id', value=id_name))
        rule_node = CssRuleNode(selector=selector, lineno=rule_lineno)
        self.consume(TokenType.LBRACE)
        while self.current_token() and self.current_token().type != TokenType.RBRACE: rule_node.properties.append(self.parse_css_property())
        self.consume(TokenType.RBRACE); return rule_node
    def parse_value_tokens(self, terminators: List[TokenType]) -> str:
        if self.current_token().type == TokenType.STRING: return self.consume(TokenType.STRING).value[1:-1]
        value_parts = []
        while self.current_token() and self.current_token().type not in terminators: value_parts.append(self.consume().value)
        return " ".join(value_parts).strip()
