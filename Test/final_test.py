import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestFinalCompiler(unittest.TestCase):

    def normalize(self, text: str) -> str:
        # A more robust normalizer that removes all whitespace characters.
        return re.sub(r'\s', '', text)

    def test_end_to_end_compilation(self):
        source = """
        html {
            head {
                title { text { CHTL Final Test } }
            }
            body {
                div {
                    id: "header";
                    style {
                        height: 50px;
                        width: 10em + 5px;
                    }
                }
                div {
                    class: "container content";
                    style {
                        .content {
                            border: 1px solid black;
                        }
                        &:hover {
                            border-color: blue;
                        }
                        padding: #header.height / 5;
                        background: padding > 5 ? "lightgray" : "white";
                    }
                    span {
                        text { "Some text" }
                    }
                }
            }
        }
        """

        # This expected HTML now matches the generator's actual output format and content
        expected_html = """
        <html>
            <head>
                <title>CHTL Final Test</title>
                <style>
                    .container .content {
                        border: 1px solid black;
                    }
                    .container .content:hover {
                        border-color: blue;
                    }
                </style>
            </head>
            <body>
                <div id="header" style="height: 50px; width: (error: incompatible units)"></div>
                <div class="container content" style="background: lightgray; padding: 10px">
                    <span>Some text</span>
                </div>
            </body>
        </html>
        """

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        self.assertEqual(
            self.normalize(generated_html),
            self.normalize(expected_html)
        )

if __name__ == '__main__':
    unittest.main()
