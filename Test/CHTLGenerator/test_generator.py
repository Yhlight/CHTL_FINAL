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

    def test_customization_and_generation(self):
        source = """
        [Custom] @Element MyBox {
            div {
                id: "my-id";
                class: "box";
            }
        }
        body {
            @Element MyBox {
                delete id;
            }
        }
        """
        expected = self._dedent("""
        <body>
          <div class="box">
          </div>
        </body>
        """)
        self.assertEqual(self._compile_source(source), expected)


if __name__ == '__main__':
    unittest.main()
