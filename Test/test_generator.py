import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.parser import Parser
from CHTL.generator import Generator
from CHTL.template_registry import TemplateRegistry
from CHTL.template_processor import TemplateProcessor

class TestGenerator(unittest.TestCase):

    def test_end_to_end_with_all_styles(self):
        source = textwrap.dedent("""
            [Template] @Var MyVars { brand_color: "blue"; }
            head { title { text: "Test"; } }
            body {
                div {
                    id: "box";
                    class: "box";
                    style {
                        width: 100px;
                        color: MyVars(brand_color);
                        .box {
                            height: width;
                        }
                    }
                }
                p {
                    style {
                        width: #box.width / 2;
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
                height: 100px;
            }
            </style>
            </head>
            <body>
                <div id="box" class="box" style="width: 100px; color: blue;">
                </div>
                <p style="width: 50px;">
                </p>
            </body>
        """).strip()

        # Full pipeline
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        registry = TemplateRegistry(ast)
        processor = TemplateProcessor(ast, registry)
        processed_ast = processor.process()
        generator = Generator(indent=4)
        generated_html = generator.generate(processed_ast)

        normalized_generated = " ".join(generated_html.split())
        normalized_expected = " ".join(expected_html.split())
        self.assertEqual(normalized_generated, normalized_expected)

if __name__ == '__main__':
    unittest.main()
