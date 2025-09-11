import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes
from CHTL.CHTLGenerator.generator import Generator

class TestDeclarationParser(unittest.TestCase):

    def test_origin_parsing(self):
        source = '[Origin] @Html myCode { <div>Hello</div> }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertEqual(len(ast.children), 1)
        origin_node = ast.children[0]
        self.assertIsInstance(origin_node, nodes.OriginNode)
        self.assertEqual(origin_node.origin_type, "Html")
        self.assertEqual(origin_node.name, "myCode")
        # The content check is tricky due to token joining, but we can check for parts
        self.assertIn("div", origin_node.content)
        self.assertIn("Hello", origin_node.content)

        # Test generation
        generated_html = Generator().generate(ast)
        self.assertIn("<div>Hello</div>", generated_html)

    def test_custom_parsing(self):
        source = '[Custom] @Style MyTheme { delete color; }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertEqual(len(ast.children), 1)
        custom_node = ast.children[0]
        self.assertIsInstance(custom_node, nodes.CustomDefinitionNode)
        self.assertEqual(custom_node.custom_type, "Style")
        self.assertEqual(custom_node.name, "MyTheme")
        self.assertEqual(len(custom_node.body), 1)
        delete_node = custom_node.body[0]
        self.assertIsInstance(delete_node, nodes.DeleteNode)
        self.assertEqual(delete_node.targets, ["color"])

if __name__ == '__main__':
    unittest.main()
