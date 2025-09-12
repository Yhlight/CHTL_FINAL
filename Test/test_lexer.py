import sys
import os
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token import TokenType

class TestLexer(unittest.TestCase):

    def test_base_tokens(self):
        source = "div { id: 'a'; }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        types = [t.type for t in tokens]
        self.assertIn(TokenType.IDENTIFIER, types)
        self.assertIn(TokenType.LBRACE, types)
        self.assertIn(TokenType.COLON, types)
        self.assertIn(TokenType.STRING, types)
        self.assertIn(TokenType.SEMICOLON, types)
        self.assertIn(TokenType.RBRACE, types)

    def test_expression_tokens(self):
        source = "+ - * / % ** > < >= <= == != && || ?"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        types = [t.type for t in tokens]
        self.assertIn(TokenType.PLUS, types)
        self.assertIn(TokenType.GREATER, types)
        self.assertIn(TokenType.AMPERSAND_AMPERSAND, types)
        self.assertIn(TokenType.QUESTION, types)

    def test_template_tokens(self):
        source = "[Template] @Style MyStyles; inherit"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        types = [t.type for t in tokens]
        self.assertIn(TokenType.TEMPLATE_KEYWORD, types)
        self.assertIn(TokenType.AT, types)
        self.assertIn(TokenType.IDENTIFIER, types)
        self.assertIn(TokenType.INHERIT, types)

    def test_custom_tokens(self):
        source = "[Custom] delete insert after before replace"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        types = [t.type for t in tokens]
        self.assertIn(TokenType.CUSTOM_KEYWORD, types)
        self.assertIn(TokenType.DELETE, types)
        self.assertIn(TokenType.INSERT, types)
        self.assertIn(TokenType.AFTER, types)
        self.assertIn(TokenType.BEFORE, types)
        self.assertIn(TokenType.REPLACE, types)

if __name__ == '__main__':
    unittest.main()
