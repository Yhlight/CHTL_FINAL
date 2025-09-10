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

    def setUp(self):
        # A helper to remove whitespace for easier comparison
        self.remove_whitespace = lambda s: re.sub(r'\s+', '', s)

    def test_full_generation(self):
        source = """
        html {
            head {
                title { text { My CHTL Page } }
            }
            body {
                div {
                    id: "main";
                    class = "container";

                    h1 { text { Welcome! } }
                    p { text { This is generated from CHTL. } }
                    img { src: "image.jpg"; }
                }
            }
        }
        """

        expected_html = """
        <html>
            <head>
                <title>My CHTL Page</title>
            </head>
            <body>
                <div id="main" class="container">
                    <h1>Welcome!</h1>
                    <p>This is generated from CHTL.</p>
                    <img src="image.jpg" />
                </div>
            </body>
        </html>
        """

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        self.assertEqual(
            self.remove_whitespace(generated_html),
            self.remove_whitespace(expected_html)
        )

    def test_self_closing_tag(self):
        source = "br {}"
        expected_html = "<br />"

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        self.assertEqual(
            self.remove_whitespace(generated_html),
            self.remove_whitespace(expected_html)
        )

if __name__ == '__main__':
    unittest.main()
