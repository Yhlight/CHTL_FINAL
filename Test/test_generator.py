import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.parser import Parser
from CHTL.generator import Generator

class TestGenerator(unittest.TestCase):

    def test_full_pipeline_and_global_styles(self):
        source = textwrap.dedent("""
            head {
                title { text: "Test"; }
            }
            body {
                div {
                    class: "box";
                    style {
                        width: 100px;
                        .box {
                            color: red;
                            height: width;
                        }
                    }
                }
            }
        """)
        expected_html = textwrap.dedent("""
            <head>
                <title>
                    Test
                </title>
                <style>
            .box {
                color: red;
                height: 100px;
            }
            </style>
            </head>
            <body>
                <div class="box" style="width: 100px;">
                </div>
            </body>
        """).strip()

        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        generator = Generator(indent=4)
        generated_html = generator.generate(ast)

        normalized_generated = " ".join(generated_html.split())
        normalized_expected = " ".join(expected_html.split())
        self.assertEqual(normalized_generated, normalized_expected)

    def test_cross_element_reference_generation(self):
        source = textwrap.dedent("""
            div { id: "box"; style { width: 100px; } }
            p { style { width: #box.width / 2; } }
        """)
        expected_html = textwrap.dedent("""
            <div id="box" style="width: 100px;">
            </div>
            <p style="width: 50px;">
            </p>
        """).strip()

        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        generator = Generator(indent=4)
        generated_html = generator.generate(ast)
        self.assertEqual(generated_html.strip(), expected_html)

if __name__ == '__main__':
    unittest.main()
