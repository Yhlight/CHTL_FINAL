import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLParser.selector_parser import SelectorParser
from CHTL.CHTLUtils.ast_search import find_nodes_by_selector
from CHTL.CHTLevaluator.evaluator import ExpressionEvaluator
from CHTL.CHTLNode.nodes import DocumentNode, ElementNode, StyleNode, CssPropertyNode, LiteralNode

class TestExpressionEvaluator(unittest.TestCase):

    def _setup_test_env(self, chtl_source):
        """Helper to create a full AST from source for testing."""
        context = CompilationContext()
        parser = Parser(Lexer(chtl_source).tokenize(), context)
        doc_ast = parser.parse()
        return doc_ast

    def _evaluate_expression_in_source(self, source: str, context_selector: str, prop_name: str):
        """
        A helper to find a specific property expression in a source file,
        find its context element, and evaluate it.
        """
        doc_ast = self._setup_test_env(source)

        # Correctly find the context element using the new selector engine
        selector_ast = SelectorParser(context_selector).parse()
        target_nodes = find_nodes_by_selector(doc_ast, selector_ast)

        if not target_nodes:
            self.fail(f"Test setup failed: could not find context element '{context_selector}'")

        target_element = target_nodes[0]

        # Find the expression to evaluate on that element
        expr_to_eval = None
        for child in target_element.children:
            if isinstance(child, StyleNode):
                for prop in child.children:
                    if isinstance(prop, CssPropertyNode) and prop.name == prop_name:
                        expr_to_eval = prop.value
                        break
            if expr_to_eval:
                break

        if not expr_to_eval:
            self.fail(f"Test setup failed: could not find property '{prop_name}' on '{context_selector}'")

        evaluator = ExpressionEvaluator(doc_ast)
        return evaluator.evaluate(expr_to_eval, target_element)

    def test_simple_arithmetic(self):
        doc = DocumentNode()
        context_el = ElementNode(tag_name='div')
        evaluator = ExpressionEvaluator(doc)

        from CHTL.CHTLParser.expression_parser import ExpressionParser
        expr_ast = ExpressionParser(Lexer("10 + 5").tokenize()[:-1]).parse()
        result = evaluator.evaluate(expr_ast, context_el)
        self.assertEqual(result, "15")

    def test_local_property_reference(self):
        source = """
        div {
            style {
                width: 100px;
                height: width / 2;
            }
        }
        """
        result = self._evaluate_expression_in_source(source, "div", "height")
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
        result = self._evaluate_expression_in_source(source, "p", "width")
        self.assertEqual(result, "400px")

    def test_descendant_selector_property_reference(self):
        source = """
        div {
            id: "app";
            span {
                class: "label";
                style {
                    font-size: 16px;
                }
            }
        }
        footer {
            style {
                margin-top: #app .label.font-size * 2;
            }
        }
        """
        result = self._evaluate_expression_in_source(source, "footer", "margin-top")
        self.assertEqual(result, "32px")

    def test_indexed_selector_property_reference(self):
        source = """
        div {
            p { class: "item"; style { height: 20px; } }
            p { class: "item"; style { height: 30px; } }
        }
        footer {
            style {
                height: .item[1].height;
            }
        }
        """
        result = self._evaluate_expression_in_source(source, "footer", "height")
        self.assertEqual(result, "30px")


if __name__ == '__main__':
    unittest.main()
