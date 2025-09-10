import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes
from CHTL.CHTLGenerator.generator import Generator

class TestTemplateParser(unittest.TestCase):

    def test_template_definition_and_usage(self):
        source = """
        [Template] @Style DefaultText {
            color: black;
            line-height: 1.6;
        }

        div {
            style {
                @Style DefaultText;
            }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        # Test Template Definition
        self.assertEqual(len(ast.children), 2)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, nodes.TemplateDefinitionNode)
        self.assertEqual(def_node.template_type, "Style")
        self.assertEqual(def_node.name, "DefaultText")
        self.assertEqual(len(def_node.body), 2)
        self.assertIsInstance(def_node.body[0], nodes.StylePropertyNode)
        self.assertEqual(def_node.body[0].name, "color")

        # Test Template Expansion
        generated_html = Generator().generate(ast)
        self.assertIn('style="color:black;line-height:1.6"', generated_html.replace(" ", ""))

if __name__ == '__main__':
    unittest.main()
