import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestFunctionalFeatures(unittest.TestCase):

    def normalize_html(self, html: str) -> str:
        """Removes whitespace and newlines to make comparison easier."""
        return re.sub(r'\s+', '', html)

    def test_import_and_template_expansion(self):
        # The test works by compiling the main_test.chtl file,
        # which in turn imports templates.chtl.

        main_file_path = "main_test.chtl"
        with open(main_file_path, 'r') as f:
            source = f.read()

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        normalized_html = self.normalize_html(generated_html)

        # The button should have styles from the template and the local definition.
        # We check for each property individually to avoid order-dependency issues.
        self.assertIn('background-color:blue', normalized_html)
        self.assertIn('border-radius:5px', normalized_html)
        self.assertIn('font-weight:bold', normalized_html)
        self.assertIn('padding:10px20px', normalized_html)

if __name__ == '__main__':
    unittest.main()
