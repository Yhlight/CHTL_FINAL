import unittest
import textwrap
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import HTMLGenerator

class TestGenerator(unittest.TestCase):

    def _compile_source(self, source):
        # Helper to run the full pipeline
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens)
        ast = parser.parse()
        generator = HTMLGenerator(ast)
        return generator.generate()

    def _dedent(self, text):
        # Helper to dedent multiline strings for comparison
        return textwrap.dedent(text).strip()

    def test_basic_html(self):
        source = "html { head {} body {} }"
        expected = self._dedent("""
        <html>
          <head></head>
          <body></body>
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
        # The generator should escape the special characters
        expected = '<p>&lt;a&gt; &amp; &#x27;b&#x27;</p>'
        self.assertEqual(self._compile_source(source), expected)

    def test_comment_generation(self):
        source = """
        div {
            -- a real comment
            // an ignored comment
        }
        """
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

    def test_complex_document(self):
        source = """
        html {
            head {
                title: My Test Page;
            }
            body {
                div {
                    id = "app";
                    h1 { text { Welcome } }
                    p {
                        class: "intro";
                        text: "This is a test.";
                    }
                }
            }
        }
        """
        expected = self._dedent("""
        <html>
          <head>
            <title>My Test Page</title>
          </head>
          <body>
            <div id="app">
              <h1>Welcome</h1>
              <p class="intro">This is a test.</p>
            </div>
          </body>
        </html>
        """)
        self.assertEqual(self._compile_source(source), expected)

if __name__ == '__main__':
    unittest.main()
