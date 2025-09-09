import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLParser.expression_parser import ExpressionParser
from CHTL.CHTLevaluator.evaluator import ExpressionEvaluator
from CHTL.CHTLNode.nodes import DocumentNode, ElementNode, StyleNode, CssPropertyNode, LiteralNode

class TestExpressionEvaluator(unittest.TestCase):

    def _setup_test_env(self, chtl_source):
        """Helper to create a full AST from source for testing."""
        context = CompilationContext()
        parser = Parser(Lexer(chtl_source).tokenize(), context)
        doc_ast = parser.parse()
        return doc_ast

    def _evaluate_expression(self, expr_str, doc_ast, context_element):
        """Helper to parse and evaluate a standalone expression string."""
        tokens = Lexer(expr_str).tokenize()[:-1] # Exclude EOF
        expr_ast = ExpressionParser(tokens).parse()
        evaluator = ExpressionEvaluator(doc_ast)
        return evaluator.evaluate(expr_ast, context_element)

    def test_simple_arithmetic(self):
        doc = DocumentNode() # Empty doc for this test
        context_el = ElementNode(tag_name='div')
        result = self._evaluate_expression("10 + 5", doc, context_el)
        self.assertEqual(result, "15")

        result_px = self._evaluate_expression("100px - 25px", doc, context_el)
        self.assertEqual(result_px, "75px")

        result_div = self._evaluate_expression("100 / 4", doc, context_el)
        self.assertEqual(result_div, "25")

    def test_comparison(self):
        doc = DocumentNode()
        context_el = ElementNode(tag_name='div')
        result = self._evaluate_expression("100 > 50", doc, context_el)
        self.assertTrue(result)

        result_false = self._evaluate_expression("10px < 5px", doc, context_el)
        self.assertFalse(result_false)

    def test_local_property_reference(self):
        source = """
        div {
            style {
                width: 100px;
                height: width / 2;
            }
        }
        """
        doc_ast = self._setup_test_env(source)
        div_element = doc_ast.children[0]

        # We need to find the expression node for `width / 2`
        style_node = div_element.children[0]
        height_prop = style_node.children[1]
        height_expr_ast = height_prop.value

        evaluator = ExpressionEvaluator(doc_ast)
        result = evaluator.evaluate(height_expr_ast, div_element)
        self.assertEqual(result, "50px")

    def test_external_property_reference(self):
        source = """
        div {
            id: "box";
            style {
                width: 200px;
            }
        }
        p {
            style {
                width: #box.width * 2;
            }
        }
        """
        doc_ast = self._setup_test_env(source)
        p_element = doc_ast.children[1]

        style_node = p_element.children[0]
        width_prop = style_node.children[0]
        width_expr_ast = width_prop.value

        evaluator = ExpressionEvaluator(doc_ast)
        result = evaluator.evaluate(width_expr_ast, p_element)
        self.assertEqual(result, "400px")

    def test_conditional_expression(self):
        source = """
        div {
            style {
                width: 150px;
                color: width > 100px ? "red" : "blue";
            }
        }
        """
        doc_ast = self._setup_test_env(source)
        div_element = doc_ast.children[0]

        style_node = div_element.children[0]
        color_prop = style_node.children[1]
        color_expr_ast = color_prop.value

        evaluator = ExpressionEvaluator(doc_ast)
        result = evaluator.evaluate(color_expr_ast, div_element)
        self.assertEqual(result, "red")

if __name__ == '__main__':
    unittest.main()
