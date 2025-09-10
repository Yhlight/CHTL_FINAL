import unittest
import textwrap
from CHTL.api import compile_chtl # Import from the new API file
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator

class TestGenerator(unittest.TestCase):

    def _compile_source(self, source):
        """
        Helper to run the CHTL compiler and get the generated fragment.
        """
        # This now directly tests the CHTL compiler part of the pipeline
        return compile_chtl(source)

    def _dedent(self, text):
        return textwrap.dedent(text).strip()

    def _assert_html_equal(self, actual, expected):
        # Normalize whitespace for a more robust comparison
        normalized_actual = " ".join(actual.split())
        normalized_expected = " ".join(self._dedent(expected).split())
        self.assertEqual(normalized_actual, normalized_expected)

    def test_simple_fragment_generation(self):
        source = 'div { id="main"; p { text: "Hello"; } }'
        expected = """
        <div id="main">
            <p>Hello</p>
        </div>
        """
        self._assert_html_equal(self._compile_source(source), expected)

    def test_style_hoisting_in_fragment(self):
        source = """
        style { .rule { color: green; } }
        div {}
        """
        actual = self._compile_source(source)
        # Use more robust checks that are not sensitive to whitespace
        self.assertIn("<style>", actual)
        self.assertIn(".rule", actual)
        self.assertIn("color: green;", actual)
        self.assertIn("</style>", actual)
        self.assertIn("<div></div>", actual)

    def test_origin_block_rendering(self):
        source = """
        div {
            [Origin] @Html {<p>This is &lt;raw&gt; HTML.</p>}
        }
        """
        actual = self._compile_source(source)
        self.assertIn('<p>This is &lt;raw&gt; HTML.</p>', actual)
        self.assertNotIn('&amp;lt;', actual)

    def test_expression_evaluation_rendering(self):
        source = """
        div {
            style {
                width: 200px;
                height: width / 2;
                background-color: height > 50px ? "green" : "red";
            }
        }
        """
        expected = '<div style="width: 200px; height: 100px; background-color: green;"></div>'
        self._assert_html_equal(self._compile_source(source), expected)

if __name__ == '__main__':
    unittest.main()
