import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestCustomization(unittest.TestCase):

    def normalize(self, text: str) -> str:
        return re.sub(r'\s+', '', text)

    def test_delete_property(self):
        source = """
        [Custom] @Style Base {
            color: red;
            font-size: 16px;
            border: 1px solid black;
        }

        div {
            style {
                @Style Base {
                    delete font-size, border;
                }
            }
        }
        """

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        # The final style should only contain 'color: red'
        normalized_html = self.normalize(generated_html)
        self.assertIn('style="color:red"', normalized_html)
        self.assertNotIn('font-size', normalized_html)
        self.assertNotIn('border', normalized_html)

if __name__ == '__main__':
    unittest.main()
