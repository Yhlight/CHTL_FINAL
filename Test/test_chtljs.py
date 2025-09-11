import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL_JS.CHTLJSLexer.lexer import CHTLJSLexer
from CHTL_JS.CHTLJSLexer.token_type import CHTLJSTokenType
from CHTL_JS.CHTLJSParser.parser import CHTLJSParser
from CHTL_JS.CHTLJSNode import nodes
from CHTL_JS.CHTLJSGenerator.generator import CHTLJSGenerator

class TestCHTLJSLexer(unittest.TestCase):

    def assert_tokens_equal(self, tokens, expected_types):
        token_types = [token.type for token in tokens]
        self.assertEqual(token_types, expected_types, "Token stream mismatch")

    def test_enhanced_selector(self):
        source = "{{button}}->listen{}"
        lexer = CHTLJSLexer(source)
        tokens = lexer.scan_tokens()
        self.assert_tokens_equal(tokens, [
            CHTLJSTokenType.LBRACE_LBRACE,
            CHTLJSTokenType.IDENTIFIER,
            CHTLJSTokenType.RBRACE_RBRACE,
            CHTLJSTokenType.ARROW,
            CHTLJSTokenType.IDENTIFIER,
            CHTLJSTokenType.LBRACE,
            CHTLJSTokenType.RBRACE,
            CHTLJSTokenType.EOF,
        ])
        self.assertEqual(tokens[1].lexeme, "button")
        self.assertEqual(tokens[4].lexeme, "listen")

class TestCHTLJSParser(unittest.TestCase):

    def test_listen_statement_parsing(self):
        source = """
        {{button}}->listen {
            click: () => {};
        }
        """
        tokens = CHTLJSLexer(source).scan_tokens()
        ast = None
        try:
            ast = CHTLJSParser(tokens).parse()
        except Exception as e:
            print("\n--- CHTL JS LEXER TOKENS ---")
            for token in tokens:
                print(token)
            print("--- END OF TOKENS ---\n")
            raise e

        self.assertIsNotNone(ast)
        self.assertEqual(len(ast.children), 1)
        listen_node = ast.children[0]
        self.assertIsInstance(listen_node, nodes.ListenBlockNode)
        self.assertEqual(listen_node.target.selector_text, "button")

        self.assertEqual(len(listen_node.listeners), 1)
        event_listener = listen_node.listeners[0]
        self.assertEqual(event_listener.event_name, "click")
        self.assertEqual(event_listener.callback_code, "()=>{}")

class TestCHTLJSGenerator(unittest.TestCase):
    def test_generator(self):
        source = "{{#my-btn}}->listen{click: () => console.log('hi');}"
        tokens = CHTLJSLexer(source).scan_tokens()
        ast = CHTLJSParser(tokens).parse()
        js_code = CHTLJSGenerator().generate(ast)

        expected_code = "document.querySelector('#my-btn').addEventListener('click', () => console.log('hi'));"
        self.assertEqual(js_code.replace(" ", ""), expected_code.replace(" ", ""))

if __name__ == '__main__':
    unittest.main()
