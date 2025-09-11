import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator
from CHTL.CHTLNode import nodes

class TestCustomization(unittest.TestCase):

    def test_custom_parsing(self):
        # This test only verifies that the parser can recognize the block.
        # Full functionality is not yet implemented.
        source = '[Custom] @Element MyBox { }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertEqual(len(ast.children), 1)
        custom_node = ast.children[0]
        self.assertIsInstance(custom_node, nodes.CustomDefinitionNode)
        self.assertEqual(custom_node.custom_type, "Element")
        self.assertEqual(custom_node.name, "MyBox")

if __name__ == '__main__':
    unittest.main()
