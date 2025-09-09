import unittest
import textwrap
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator

class TestGenerator(unittest.TestCase):

    def _compile_source(self, source):
        context = CompilationContext()
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens, context)
        ast = parser.parse()
        transformer = ASTTransformer(ast, context)
        transformed_ast = transformer.transform()
        generator = HTMLGenerator(transformed_ast)
        return generator.generate()

    def _dedent(self, text):
        return textwrap.dedent(text).strip()

    def test_basic_html(self):
        source = "html { head {} body {} }"
        expected = self._dedent("""
        <html>
          <head>
          </head>
          <body>
          </body>
        </html>
        """)
        self.assertEqual(self._compile_source(source), expected)

    def test_attributes_and_text(self):
        source = 'div { id="main"; p { text: "Hello"; } }'
        expected = self._dedent("""
        <div id="main">
          <p>Hello</p>
        </div>
        """)
        self.assertEqual(self._compile_source(source), expected)

    def test_template_expansion_and_generation(self):
        source = """
        [Template] @Element MyBox {
            div { class: "box"; }
        }
        body { @Element MyBox; }
        """
        expected = self._dedent("""
        <body>
          <div class="box">
          </div>
        </body>
        """)
        self.assertEqual(self._compile_source(source), expected)

    def test_style_generation(self):
        source = """
        html {
            head {}
            body {
                div {
                    style {
                        .box {}
                    }
                }
            }
        }
        """
        expected = self._dedent("""
        <html>
          <head>
            <style>
              .box {}
            </style>
          </head>
          <body>
            <div class="box">
            </div>
          </body>
        </html>
        """)
        # Compare without regard to whitespace to avoid brittle tests
        self.assertEqual(
            "".join(self._compile_source(source).split()),
            "".join(expected.split())
        )

if __name__ == '__main__':
    unittest.main()
