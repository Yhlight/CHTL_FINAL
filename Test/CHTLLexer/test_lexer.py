import unittest
from CHTL.CHTLLexer.lexer import Lexer, TokenType

class TestLexer(unittest.TestCase):

    def test_single_tokens(self):
        source = "{}:;=,"
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        expected_types = [
            TokenType.LBRACE, TokenType.RBRACE, TokenType.COLON,
            TokenType.SEMICOLON, TokenType.EQUALS, TokenType.COMMA,
            TokenType.EOF
        ]
        self.assertEqual(len(tokens), len(expected_types))
        for token, expected_type in zip(tokens, expected_types):
            self.assertEqual(token.type, expected_type)

    def test_identifiers(self):
        source = "div p span my_custom_tag"
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        self.assertEqual(tokens[0].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[0].value, "div")
        self.assertEqual(tokens[1].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[1].value, "p")
        self.assertEqual(tokens[2].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[2].value, "span")
        self.assertEqual(tokens[3].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[3].value, "my_custom_tag")
        self.assertEqual(tokens[4].type, TokenType.EOF)

    def test_string_literals(self):
        source = '"hello" \'world\''
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        self.assertEqual(tokens[0].type, TokenType.STRING)
        self.assertEqual(tokens[0].value, '"hello"')
        self.assertEqual(tokens[1].type, TokenType.STRING)
        self.assertEqual(tokens[1].value, "'world'")
        self.assertEqual(tokens[2].type, TokenType.EOF)

    def test_comments(self):
        source = """
        // single line
        -- generator comment
        /* multi
           line */
        """
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        self.assertEqual(tokens[0].type, TokenType.COMMENT)
        self.assertEqual(tokens[0].value, "// single line")
        self.assertEqual(tokens[1].type, TokenType.COMMENT)
        self.assertEqual(tokens[1].value, "-- generator comment")
        self.assertEqual(tokens[2].type, TokenType.COMMENT)
        self.assertTrue('multi' in tokens[2].value)
        self.assertEqual(tokens[3].type, TokenType.EOF)

    def test_hyphenated_identifiers(self):
        source = "some-value_123 font-weight"
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        self.assertEqual(len(tokens), 3)
        self.assertEqual(tokens[0].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[0].value, "some-value_123")
        self.assertEqual(tokens[1].type, TokenType.IDENTIFIER)
        self.assertEqual(tokens[1].value, "font-weight")
        self.assertEqual(tokens[2].type, TokenType.EOF)


    def test_full_snippet(self):
        source = """
        div {
            id: main;
        }
        """
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        expected_types = [
            TokenType.IDENTIFIER, TokenType.LBRACE, TokenType.IDENTIFIER,
            TokenType.COLON, TokenType.IDENTIFIER, TokenType.SEMICOLON,
            TokenType.RBRACE, TokenType.EOF
        ]
        token_types = [t.type for t in tokens]
        self.assertEqual(token_types, expected_types)

    def test_keywords(self):
        source = "style text script from as use except html5 inherit delete insert after before replace at top bottom"
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        expected_types = [
            TokenType.STYLE, TokenType.TEXT, TokenType.SCRIPT,
            TokenType.FROM, TokenType.AS, TokenType.USE, TokenType.EXCEPT,
            TokenType.HTML5, TokenType.INHERIT, TokenType.DELETE,
            TokenType.INSERT, TokenType.AFTER, TokenType.BEFORE,
            TokenType.REPLACE, TokenType.AT_KEYWORD, TokenType.TOP,
            TokenType.BOTTOM, TokenType.EOF
        ]
        token_types = [t.type for t in tokens]
        self.assertEqual(token_types, expected_types)

    def test_bracketed_keywords(self):
        source = "[Template] [Custom]"
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        expected_types = [
            TokenType.LBRACK, TokenType.TEMPLATE, TokenType.RBRACK,
            TokenType.LBRACK, TokenType.CUSTOM, TokenType.RBRACK,
            TokenType.EOF,
        ]
        token_types = [t.type for t in tokens]
        self.assertEqual(token_types, expected_types)


if __name__ == '__main__':
    unittest.main()
