import unittest
import sys
import os
import re

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestGenerator(unittest.TestCase):

    def normalize_html(self, html: str) -> str:
        """Removes whitespace and newlines to make comparison easier."""
        return re.sub(r'\s+', '', html)

    def test_full_generation_with_styles(self):
        source = """
        html {
            head {
                title { text { Styled Page } }
            }
            body {
                div {
                    style {
                        width: 100%;
                        .box {
                            color: red;
                        }
                    }
                    p { text { Hello } }
                }
            }
        }
        """

        expected_html = """
        <html>
            <head>
                <style>
                    .box {
                        color: red;
                    }
                </style>
                <title>Styled Page</title>
            </head>
            <body>
                <div class="box" style="width: 100%">
                    <p>Hello</p>
                </div>
            </body>
        </html>
        """

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        self.assertEqual(
            self.normalize_html(generated_html),
            self.normalize_html(expected_html)
        )

if __name__ == '__main__':
    unittest.main()
