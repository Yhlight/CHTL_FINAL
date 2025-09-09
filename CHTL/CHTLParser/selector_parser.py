from CHTL.CHTLLexer.lexer import Lexer, TokenType, Token
from CHTL.CHTLNode.nodes import SelectorNode, SelectorPartNode, SelectorType
from typing import List, Optional

class SelectorParser:
    """
    Parses a selector string (e.g., '.box button[0]') into a structured
    SelectorNode AST.
    """
    def __init__(self, selector_string: str):
        # We can reuse the main CHTL lexer for this task
        lexer = Lexer(selector_string)
        self.tokens = lexer.tokenize()
        # Filter out the final EOF token as we don't need it for parsing
        if self.tokens and self.tokens[-1].type == TokenType.EOF:
            self.tokens.pop()
        self.pos = 0

    def current_token(self) -> Optional[Token]:
        if self.pos < len(self.tokens):
            return self.tokens[self.pos]
        return None

    def consume(self, expected_type: Optional[TokenType] = None) -> Token:
        token = self.current_token()
        if not token or (expected_type and token.type != expected_type):
            raise ValueError(f"Unexpected token: got {token.type if token else 'None'}, expected {expected_type}")
        self.pos += 1
        return token

    def parse(self) -> SelectorNode:
        selector_node = SelectorNode()

        while self.current_token():
            selector_node.parts.append(self.parse_selector_part())

        return selector_node

    def parse_selector_part(self) -> SelectorPartNode:
        """Parses one component of a selector, e.g., '.box' or 'button[0]'."""
        token = self.current_token()
        sel_type = None
        sel_value = ""

        if token.type == TokenType.DOT:
            self.consume(TokenType.DOT)
            sel_type = SelectorType.CLASS
            sel_value = self.consume(TokenType.IDENTIFIER).value
        elif token.type == TokenType.IDENTIFIER and token.value.startswith('#'):
            sel_type = SelectorType.ID
            sel_value = self.consume(TokenType.IDENTIFIER).value[1:] # remove #
        elif token.type == TokenType.IDENTIFIER:
            sel_type = SelectorType.TAG
            sel_value = self.consume(TokenType.IDENTIFIER).value
        else:
            raise ValueError(f"Invalid start of selector part: {token.type}")

        # Check for an index, e.g., [0]
        sel_index = None
        if self.current_token() and self.current_token().type == TokenType.LBRACK:
            self.consume(TokenType.LBRACK)
            index_token = self.consume(TokenType.UNQUOTED_LITERAL) # Lexer sees '0' as unquoted
            try:
                sel_index = int(index_token.value)
            except (ValueError, TypeError):
                raise ValueError(f"Invalid index in selector: {index_token.value}")
            self.consume(TokenType.RBRACK)

        return SelectorPartNode(type=sel_type, value=sel_value, index=sel_index)
