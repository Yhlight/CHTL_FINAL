import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token_type import TokenType

class TestLexer(unittest.TestCase):
    def test_simple_html_structure(self):
        source = """
        html {
            // This is a comment
            body {
                div {
                    id: "main-content";
                    class = box;
                }
            }
        }
        """
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()

        expected_types = [
            TokenType.IDENTIFIER,   # html
            TokenType.LBRACE,       # {
            TokenType.IDENTIFIER,   # body
            TokenType.LBRACE,       # {
            TokenType.IDENTIFIER,   # div
            TokenType.LBRACE,       # {
            TokenType.IDENTIFIER,   # id
            TokenType.COLON,        # :
            TokenType.STRING,       # "main-content"
            TokenType.SEMICOLON,    # ;
            TokenType.IDENTIFIER,   # class
            TokenType.EQUALS,       # =
            TokenType.IDENTIFIER,   # box
            TokenType.SEMICOLON,    # ;
            TokenType.RBRACE,       # }
            TokenType.RBRACE,       # }
            TokenType.RBRACE,       # }
            TokenType.EOF,
        ]

        token_types = [token.type for token in tokens]
        self.assertEqual(token_types, expected_types)

        # Check a specific token's lexeme
        self.assertEqual(tokens[8].lexeme, "main-content") # The string literal
        self.assertEqual(tokens[12].lexeme, "box") # The unquoted literal

    def test_text_node(self):
        source = """
        text {
            "Hello World"
        }
        """
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()

        expected_types = [
            TokenType.TEXT,
            TokenType.LBRACE,
            TokenType.STRING,
            TokenType.RBRACE,
            TokenType.EOF
        ]

        token_types = [token.type for token in tokens]
        self.assertEqual(token_types, expected_types)
        self.assertEqual(tokens[2].lexeme, "Hello World")

if __name__ == '__main__':
    unittest.main()
