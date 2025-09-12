import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.parser import Parser
from CHTL.template_registry import TemplateRegistry
from CHTL.CHTLNode.nodes import TemplateDefinitionNode

class TestTemplateRegistry(unittest.TestCase):

    def setUp(self):
        source = textwrap.dedent("""
            [Template] @Style MyStyles {}
            [Template] @Element MyElement {}
        """)
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        self.registry = TemplateRegistry(ast)

    def test_find_style_template(self):
        template = self.registry.find("Style", "MyStyles")
        self.assertIsNotNone(template)
        self.assertIsInstance(template, TemplateDefinitionNode)

    def test_find_element_template(self):
        template = self.registry.find("Element", "MyElement")
        self.assertIsNotNone(template)

    def test_find_non_existent(self):
        template = self.registry.find("Style", "NotFound")
        self.assertIsNone(template)

if __name__ == '__main__':
    unittest.main()
