import sys
import os
import unittest

# Add project root to path to allow sibling directory imports
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token import TokenType, Token

class TestLexer(unittest.TestCase):

    def test_lexer_full_scan(self):
        source = """
// This is a comment.
html {
    id: "main-page";
    class = main;
    -- this is a generator comment

    body {
        text: 'Hello, CHTL!'
    }
}
/**
 * A multi-line comment.
 */
"""
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()

        expected_tokens = [
            Token(type=TokenType.IDENTIFIER, value='html', line=3, column=1),
            Token(type=TokenType.LBRACE, value='{', line=3, column=6),
            Token(type=TokenType.IDENTIFIER, value='id', line=4, column=5),
            Token(type=TokenType.COLON, value=':', line=4, column=7),
            Token(type=TokenType.STRING, value='main-page', line=4, column=9),
            Token(type=TokenType.SEMICOLON, value=';', line=4, column=22),
            Token(type=TokenType.IDENTIFIER, value='class', line=5, column=5),
            Token(type=TokenType.EQUALS, value='=', line=5, column=11),
            Token(type=TokenType.IDENTIFIER, value='main', line=5, column=13),
            Token(type=TokenType.SEMICOLON, value=';', line=5, column=17),
            Token(type=TokenType.GEN_COMMENT, value='this is a generator comment', line=6, column=5),
            Token(type=TokenType.IDENTIFIER, value='body', line=8, column=5),
            Token(type=TokenType.LBRACE, value='{', line=8, column=10),
            Token(type=TokenType.IDENTIFIER, value='text', line=9, column=9),
            Token(type=TokenType.COLON, value=':', line=9, column=13),
            Token(type=TokenType.STRING, value='Hello, CHTL!', line=9, column=15),
            Token(type=TokenType.RBRACE, value='}', line=10, column=5),
            Token(type=TokenType.RBRACE, value='}', line=11, column=1),
            Token(type=TokenType.EOF, value='', line=15, column=2)
        ]
        self.assertEqual(len(tokens), len(expected_tokens))
        for i, (token, expected) in enumerate(zip(tokens, expected_tokens)):
            with self.subTest(i=i):
                self.assertEqual(token.type, expected.type)
                self.assertEqual(token.value, expected.value)

    def test_hyphenated_identifier_and_number(self):
        source = "font-size: 16px;"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        expected_types = [
            TokenType.IDENTIFIER,
            TokenType.COLON,
            TokenType.NUMBER,
            TokenType.IDENTIFIER,
            TokenType.SEMICOLON,
            TokenType.EOF,
        ]
        self.assertEqual(len(tokens), len(expected_types))
        for token, expected_type in zip(tokens, expected_types):
            self.assertEqual(token.type, expected_type)
        self.assertEqual(tokens[0].value, "font-size")
        self.assertEqual(tokens[2].value, "16")
        self.assertEqual(tokens[3].value, "px")

    def test_arithmetic_operators(self):
        source = "+ - * / % **"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        expected_types = [
            TokenType.PLUS,
            TokenType.MINUS,
            TokenType.STAR,
            TokenType.SLASH,
            TokenType.PERCENT,
            TokenType.STAR_STAR,
            TokenType.EOF,
        ]
        self.assertEqual(len(tokens), len(expected_types))
        for token, expected_type in zip(tokens, expected_types):
            self.assertEqual(token.type, expected_type)

    def test_conditional_operators(self):
        source = "? == != < <= > >= && ||"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        expected_types = [
            TokenType.QUESTION,
            TokenType.EQUAL_EQUAL,
            TokenType.BANG_EQUAL,
            TokenType.LESS,
            TokenType.LESS_EQUAL,
            TokenType.GREATER,
            TokenType.GREATER_EQUAL,
            TokenType.AMPERSAND_AMPERSAND,
            TokenType.PIPE_PIPE,
            TokenType.EOF,
        ]
        self.assertEqual(len(tokens), len(expected_types))
        for token, expected_type in zip(tokens, expected_types):
            self.assertEqual(token.type, expected_type)

if __name__ == '__main__':
    unittest.main()
