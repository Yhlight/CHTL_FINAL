import unittest
import textwrap
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator

class TestGenerator(unittest.TestCase):

    def _compile_source(self, source, use_default_structure=False):
        context = CompilationContext()
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens, context)
        ast = parser.parse()
        transformer = ASTTransformer(ast, context, "test.chtl")
        transformed_ast = transformer.transform()
        generator = HTMLGenerator(transformed_ast)
        return generator.generate(use_default_structure=use_default_structure)

    def _dedent(self, text):
        return textwrap.dedent(text).strip()

    def _assert_html_equal(self, actual, expected):
        # Normalize whitespace for a more robust comparison
        normalized_actual = " ".join(actual.split())
        normalized_expected = " ".join(self._dedent(expected).split())
        self.assertEqual(normalized_actual, normalized_expected)

    # --- Tests for Fragment Generation (Default Behavior) ---

    def test_fragment_generation_default(self):
        source = 'div { id="main"; p { text: "Hello"; } }'
        expected = """
        <div id="main">
          <p>Hello</p>
        </div>
        """
        actual = self._compile_source(source, use_default_structure=False)
        self._assert_html_equal(actual, expected)

    def test_fragment_with_style_hoisting_and_auto_class(self):
        source = """
        div {
            style {
                .box { color: red; }
            }
            p { text: "I'm in a box"; }
        }
        """
        expected = """
        <style>
        .box { color: red; }
        </style>
        <div class="box">
          <p>I&#x27;m in a box</p>
        </div>
        """
        actual = self._compile_source(source)
        self._assert_html_equal(actual, expected)

    # --- Tests for Full Document Generation (--default-struct) ---

    def test_full_document_generation_basic(self):
        source = 'div { p { text: "Hello"; } }'
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
        </head>
        <body>
          <div>
            <p>Hello</p>
          </div>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        self._assert_html_equal(actual, expected)

    def test_full_document_with_title(self):
        source = 'title: "My Page"'
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
          <title>My Page</title>
        </head>
        <body>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        self._assert_html_equal(actual, expected)

    def test_full_document_with_styles(self):
        source = """
        style { .box { color: red; } }
        div { class: "box"; }
        """
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
          <style>
            .box {
              color: red;
            }
          </style>
        </head>
        <body>
          <div class="box"></div>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        self._assert_html_equal(actual, expected)

    def test_full_document_with_title_and_styles(self):
        source = """
        title: "Styled Page"
        style {
            body { font-family: sans-serif; }
            .container { width: 960px; }
        }
        div { class: "container"; }
        """
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
          <title>Styled Page</title>
          <style>
            body {
              font-family: sans-serif;
            }
            .container {
              width: 960px;
            }
          </style>
        </head>
        <body>
          <div class="container"></div>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        self._assert_html_equal(actual, expected)

    def test_full_document_retains_comments(self):
        source = """
        -- This is a test comment
        div {}
        // This comment is ignored
        """
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
        </head>
        <body>
          <!-- This is a test comment -->
          <div></div>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        self._assert_html_equal(actual, expected)

    # --- Tests for Expression Evaluation in Generator ---

    def test_generator_with_arithmetic_expression(self):
        source = """
        div {
            style {
                width: 100px + 50px;
            }
        }
        """
        expected = """
        <div style="width: 150px;"></div>
        """
        actual = self._compile_source(source)
        self._assert_html_equal(actual, expected)

    def test_generator_with_conditional_expression(self):
        source = """
        div {
            style {
                width: 200px;
                background-color: width > 150px ? "green" : "red";
            }
        }
        """
        expected = """
        <div style="width: 200px; background-color: green;"></div>
        """
        actual = self._compile_source(source)
        self._assert_html_equal(actual, expected)


if __name__ == '__main__':
    unittest.main()
