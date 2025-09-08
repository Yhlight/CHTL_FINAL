from lexer import Lexer, Token
from nodes import CommentNode, ElementNode, TextNode

class Parser:
    def __init__(self, lexer):
        self.lexer = lexer
        self.tokens = lexer.tokenize()
        self.token_buffer = []

    def peek(self, k=0):
        while len(self.token_buffer) <= k:
            self.token_buffer.append(next(self.tokens))
        return self.token_buffer[k]

    def consume(self, expected_kind=None):
        if not self.token_buffer: self.peek()
        if expected_kind and self.peek().type != expected_kind:
            raise RuntimeError(f"Unexpected token {self.peek()}, expected {expected_kind}")
        return self.token_buffer.pop(0)

    def parse(self):
        root = ElementNode('_root_')
        while self.peek().type != 'EOF':
            root.children.append(self.parse_statement())
        return root

    def parse_statement(self):
        if self.peek().type == 'GEN_CMNT':
            return CommentNode(self.consume().value)
        if self.peek().type == 'ID':
            if self.peek().value == 'text':
                return self.parse_text_element()
            return self.parse_element()
        raise RuntimeError(f"Invalid statement starting with {self.peek()}")

    def parse_element(self):
        tag_name = self.consume('ID').value
        node = ElementNode(tag_name)
        if self.peek().type == 'LBRACE':
            self.consume()
            node.attributes = self.parse_attributes()
            while self.peek().type != 'RBRACE':
                node.children.append(self.parse_statement())
            self.consume('RBRACE')
        return node

    def parse_attributes(self):
        attrs = {}
        while self.peek().type == 'ID' and self.peek(1).type == 'COLON':
            key = self.consume('ID').value
            self.consume('COLON')

            value = ""
            # Consume all tokens until the semicolon
            while self.peek().type != 'SEMI':
                token = self.consume()
                # For quoted strings, use the value directly. For others, add a space.
                if token.type == 'STRING':
                    value = token.value
                else:
                    value += token.value + " "

            attrs[key] = value.strip()
            self.consume('SEMI')
        return attrs

    def parse_text_element(self):
        self.consume('ID') # text
        self.consume('LBRACE')

        value = ""
        # Consume all tokens until the closing brace
        while self.peek().type != 'RBRACE':
            token = self.consume()
            if token.type == 'STRING':
                value = token.value
            else:
                value += token.value + " "

        self.consume('RBRACE')
        return TextNode(value.strip())
