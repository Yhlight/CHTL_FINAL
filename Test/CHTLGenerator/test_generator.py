import unittest
import textwrap
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import HTMLGenerator

class TestGenerator(unittest.TestCase):

    def _compile_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens)
        ast = parser.parse()
        generator = HTMLGenerator(ast)
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

    def test_html_escaping(self):
        source = 'p { text: "<a> & \'b\'"; }'
        expected = '<p>&lt;a&gt; &amp; &#x27;b&#x27;</p>'
        self.assertEqual(self._compile_source(source), expected)

    def test_comment_generation(self):
        source = "div { -- a real comment\n }"
        expected = self._dedent("""
        <div>
          <!-- a real comment -->
        </div>
        """)
        self.assertEqual(self._compile_source(source), expected)

    def test_self_closing_tags(self):
        source = "div { br; hr; }"
        expected = self._dedent("""
        <div>
          <br>
          <hr>
        </div>
        """)
        self.assertEqual(self._compile_source(source), expected)

    def test_style_generation(self):
        source = """
        html {
            head {}
            body {
                div {
                    class: container;
                    style {
                        color: red;
                        font-size: 16px;
                        .box {
                            border: 1px solid black;
                        }
                    }
                }
            }
        }
        """
        expected = self._dedent("""
        <html>
          <head>
            <style>
              .box {
                border: 1px solid black;
              }
            </style>
          </head>
          <body>
            <div class="container box" style="color: red; font-size: 16px;">
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
