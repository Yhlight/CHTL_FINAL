import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token_type import TokenType

class TestLexer(unittest.TestCase):

    def assert_tokens_equal(self, tokens, expected_types_lexemes):
        """Helper to compare token types and lexemes."""
        self.assertEqual(len(tokens), len(expected_types_lexemes), "Mismatched number of tokens")
        for i, (token, (expected_type, expected_lexeme)) in enumerate(zip(tokens, expected_types_lexemes)):
            self.assertEqual(token.type, expected_type, f"Token {i} type mismatch")
            if expected_lexeme is not None:
                self.assertEqual(token.lexeme, expected_lexeme, f"Token {i} lexeme mismatch")

    def test_property_with_units(self):
        source = "width: 100px;"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            (TokenType.IDENTIFIER, "width"),
            (TokenType.COLON, ":"),
            (TokenType.NUMBER, "100"),
            (TokenType.IDENTIFIER, "px"),
            (TokenType.SEMICOLON, ";"),
            (TokenType.EOF, None),
        ])

    def test_selector(self):
        source = "&:hover"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            (TokenType.IDENTIFIER, "&:hover"),
            (TokenType.EOF, None),
        ])

    def test_full_style_block(self):
        source = """
        style {
            .box {
                background-color: blue;
            }
            &:hover {
                opacity: 0.8;
            }
        }
        """
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            (TokenType.STYLE, "style"),
            (TokenType.LBRACE, "{"),
            (TokenType.IDENTIFIER, ".box"),
            (TokenType.LBRACE, "{"),
            (TokenType.IDENTIFIER, "background-color"),
            (TokenType.COLON, ":"),
            (TokenType.IDENTIFIER, "blue"),
            (TokenType.SEMICOLON, ";"),
            (TokenType.RBRACE, "}"),
            (TokenType.IDENTIFIER, "&:hover"),
            (TokenType.LBRACE, "{"),
            (TokenType.IDENTIFIER, "opacity"),
            (TokenType.COLON, ":"),
            (TokenType.NUMBER, "0.8"),
            (TokenType.SEMICOLON, ";"),
            (TokenType.RBRACE, "}"),
            (TokenType.RBRACE, "}"),
            (TokenType.EOF, None),
        ])

if __name__ == '__main__':
    unittest.main()
