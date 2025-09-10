import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token_type import TokenType

class TestDumbLexer(unittest.TestCase):

    def assert_tokens_equal(self, tokens, expected_types):
        token_types = [token.type for token in tokens]
        self.assertEqual(token_types, expected_types, "Token stream mismatch")

    def test_property(self):
        source = "color: red;"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            TokenType.IDENTIFIER,
            TokenType.COLON,
            TokenType.IDENTIFIER,
            TokenType.SEMICOLON,
            TokenType.EOF,
        ])

    def test_selector(self):
        source = "&:hover"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            TokenType.AMPERSAND,
            TokenType.COLON,
            TokenType.IDENTIFIER,
            TokenType.EOF,
        ])

    def test_class_selector(self):
        source = ".box"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            TokenType.DOT,
            TokenType.IDENTIFIER,
            TokenType.EOF,
        ])

    def test_expression(self):
        source = "100px + #box.width"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            TokenType.NUMBER,
            TokenType.IDENTIFIER,
            TokenType.PLUS,
            TokenType.HASH,
            TokenType.IDENTIFIER,
            TokenType.DOT,
            TokenType.IDENTIFIER,
            TokenType.EOF,
        ])

if __name__ == '__main__':
    unittest.main()
