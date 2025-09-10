import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestFinalCompiler(unittest.TestCase):

    def normalize_html(self, html: str) -> str:
        """Removes whitespace and newlines to make comparison easier."""
        return re.sub(r'\s+', '', html)

    def test_end_to_end_compilation(self):
        source = """
        html {
            head {
                title { text { CHTL Final Test } }
            }
            body {
                div {
                    id = "header";
                    style {
                        height: 50px;
                    }
                }
                div {
                    class: "container";
                    style {
                        // Auto-injects class="content"
                        .content {
                            border: 1px solid black;
                        }
                        // Resolves to .container .content:hover
                        &:hover {
                            border-color: blue;
                        }
                        // Reference and arithmetic
                        padding: #header.height / 5; // Should be 10px
                        // Conditional expression
                        background: padding > 5 ? "lightgray" : "white";
                    }
                    span {
                        text { "Some text" }
                    }
                }
            }
        }
        """

        expected_html = """
        <html>
            <head>
                <style>
                    .content {
                        border: 1px solid black;
                    }
                    .container .content:hover {
                        border-color: blue;
                    }
                </style>
                <title>CHTL Final Test</title>
            </head>
            <body>
                <div id="header" style="height: 50px"></div>
                <div class="container content" style="padding: 10.0px; background: lightgray">
                    <span>Some text</span>
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
