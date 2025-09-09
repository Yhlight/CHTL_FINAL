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
        transformer = ASTTransformer(ast, context)
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

    def test_fragment_generation_default(self):
        source = 'div { id="main"; p { text: "Hello"; } }'
        expected = """
        <div id="main">
            <p>Hello</p>
        </div>
        """
        self._assert_html_equal(self._compile_source(source), expected)

    def test_origin_block_rendering(self):
        source = """
        div {
            [Origin] @Html {<p>This is &lt;raw&gt; HTML.</p>}
        }
        """
        # We don't use the helper here because we need to check raw content precisely
        actual = self._compile_source(source)
        self.assertIn('<p>This is &lt;raw&gt; HTML.</p>', actual)
        self.assertNotIn('&amp;lt;', actual) # Ensure no double-escaping

    def test_origin_style_hoisting(self):
        source = """
        [Origin] @Style { .raw-style { color: blue; } }
        div {}
        """
        expected = """
        <!DOCTYPE html>
        <html>
        <head>
        .raw-style { color: blue; }
        </head>
        <body>
        <div></div>
        </body>
        </html>
        """
        actual = self._compile_source(source, use_default_structure=True)
        # We don't use the helper here to avoid normalizing the raw style block
        self.assertIn('.raw-style { color: blue; }', actual)
        self.assertTrue(actual.find('<head>') < actual.find('.raw-style'))
        self.assertTrue(actual.find('.raw-style') < actual.find('</head>'))

    def test_generator_with_arithmetic_expression(self):
        source = """
        div {
            style {
                width: 100px + 50px;
            }
        }
        """
        expected = '<div style="width: 150px;"></div>'
        self._assert_html_equal(self._compile_source(source), expected)

    def test_generator_with_conditional_expression(self):
        source = """
        div {
            style {
                width: 200px;
                background-color: width > 150px ? "green" : "red";
            }
        }
        """
        expected = '<div style="width: 200px; background-color: green;"></div>'
        self._assert_html_equal(self._compile_source(source), expected)


if __name__ == '__main__':
    unittest.main()
