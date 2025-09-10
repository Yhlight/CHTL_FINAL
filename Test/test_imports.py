import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes

class TestImportParser(unittest.TestCase):

    def test_simple_import(self):
        source = '[Import] @Chtl from "./components.chtl";'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertEqual(len(ast.children), 1)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, nodes.ImportNode)
        self.assertEqual(import_node.import_type, "Chtl")
        self.assertEqual(import_node.path, "./components.chtl")
        self.assertIsNone(import_node.alias)

    def test_import_with_alias(self):
        source = '[Import] @Style from "theme.css" as Theme;'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertEqual(len(ast.children), 1)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, nodes.ImportNode)
        self.assertEqual(import_node.import_type, "Style")
        self.assertEqual(import_node.path, "theme.css")
        self.assertEqual(import_node.alias, "Theme")

if __name__ == '__main__':
    unittest.main()
