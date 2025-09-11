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
        """A more careful normalizer that keeps spaces within quotes."""
        # This is a simplified version for testing; a real one might need a proper parser.
        return re.sub(r'\s+(?=[^"]*(?:"[^"]*"[^"]*)*$)', '', text).replace("\n", "")

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
                        // This should fail evaluation
                        width: 10em + 5px;
                    }
                }
                div {
                    class: "container content";
                    style {
                        // This should be ignored due to the error above, or produce a fallback
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
                    .container.content {
                        border: 1px solid black;
                    }
                    .container.content:hover {
                        border-color: blue;
                    }
                </style>
                <title>CHTL Final Test</title>
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

        # This is still brittle, but good enough for a final check
        self.assertEqual(
            self.normalize(generated_html),
            self.normalize(expected_html)
        )

if __name__ == '__main__':
    unittest.main()
