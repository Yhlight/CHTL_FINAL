import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestCorePipeline(unittest.TestCase):

    def normalize(self, text: str) -> str:
        """Removes all whitespace for resilient comparison."""
        return re.sub(r'\s', '', text)

    def compile_chtl(self, source: str) -> str:
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        return Generator().generate(ast)

    def test_basic_pipeline(self):
        source = """
        html {
            head {
                title { text { "My First CHTL Page" } }
            }
            body {
                div {
                    id: "main-content";
                    class: "container";
                    h1 { text { "Welcome!" } }
                    p { text { "This is a paragraph." } }
                }
                br;
            }
        }
        """

        expected_html = """
        <html>
            <head>
                <title>My First CHTL Page</title>
            </head>
            <body>
                <div class="container" id="main-content">
                    <h1>Welcome!</h1>
                    <p>This is a paragraph.</p>
                </div>
                <br />
            </body>
        </html>
        """

        generated_html = self.compile_chtl(source)

        self.assertEqual(
            self.normalize(generated_html),
            self.normalize(expected_html)
        )

if __name__ == '__main__':
    unittest.main()
