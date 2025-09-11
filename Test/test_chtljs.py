import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Scanner.unified_scanner import CHTLUnifiedScanner
from CHTL_JS.CHTLJSLexer.lexer import CHTLJSLexer
from CHTL_JS.CHTLJSParser.parser import CHTLJSParser
from CHTL_JS.CHTLJSNode import nodes
from CHTL_JS.CHTLJSGenerator.generator import CHTLJSGenerator

class TestCHTLJSPipeline(unittest.TestCase):

    def test_full_pipeline(self):
        # This is the raw content of a <script> tag
        source_fragment = """
            {{#my-btn}}->listen {
                click: () => { console.log("hi"); };
            }
        """

        # 1. Scan the fragment to separate CHTL JS from raw JS
        scanner = CHTLUnifiedScanner()
        # For this test, we call the inner scanner directly
        processed_fragment, js_fragments = scanner.scan_script_fragment(source_fragment)

        # 2. Lex the processed fragment
        lexer = CHTLJSLexer(processed_fragment)
        tokens = lexer.scan_tokens()

        # 3. Parse the tokens into a CHTL JS AST
        parser = CHTLJSParser(tokens)
        ast = parser.parse()

        # 4. Generate JavaScript from the AST and fragments
        generator = CHTLJSGenerator(js_fragments)
        js_code = generator.generate(ast)

        # 5. Assert the final code is correct
        expected_code = "document.querySelector('#my-btn').addEventListener('click', () => { console.log(\"hi\"); });"
        self.assertEqual(js_code.replace(" ", "").replace("\n", ""), expected_code.replace(" ", ""))

if __name__ == '__main__':
    unittest.main()
