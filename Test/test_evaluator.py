import sys
import os
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.token import Token, TokenType
from CHTL.CHTLNode.nodes import (
    StylePropertyNode, BinaryOpNode, DimensionNode, ConditionalNode, IdentifierNode, RootNode
)
from CHTL.evaluator import Evaluator, EvaluatedDimension, EvaluatedBool, EvaluatedString
from CHTL.element_registry import ElementRegistry
from CHTL.template_registry import TemplateRegistry

class TestEvaluator(unittest.TestCase):

    def setUp(self):
        dummy_root = RootNode([])
        self.element_registry = ElementRegistry(dummy_root)
        self.template_registry = TemplateRegistry(dummy_root)

    def test_arithmetic_evaluation(self):
        prop_node = StylePropertyNode("prop", BinaryOpNode(DimensionNode("10", "px"), Token(TokenType.PLUS, '+', 1, 1), DimensionNode("5", "px")))
        evaluator = Evaluator([prop_node], self.element_registry, self.template_registry)
        result = evaluator.evaluate("prop")
        self.assertIsInstance(result, EvaluatedDimension)
        self.assertEqual(result.value, 15.0)

    def test_dependency_resolution(self):
        width_node = StylePropertyNode("width", DimensionNode("100", "px"))
        height_node = StylePropertyNode("height", BinaryOpNode(IdentifierNode("width"), Token(TokenType.STAR, '*', 1, 1), DimensionNode("2", "")))
        evaluator = Evaluator([width_node, height_node], self.element_registry, self.template_registry)
        result = evaluator.evaluate("height")
        self.assertIsInstance(result, EvaluatedDimension)
        self.assertEqual(result.value, 200.0)

    def test_conditional_with_dependency(self):
        width_node = StylePropertyNode("width", DimensionNode("100", "px"))
        color_node = StylePropertyNode("color", ConditionalNode(BinaryOpNode(IdentifierNode("width"), Token(TokenType.GREATER, '>', 1, 1), DimensionNode("50", "px")), IdentifierNode("red"), IdentifierNode("blue")))
        evaluator = Evaluator([width_node, color_node], self.element_registry, self.template_registry)
        result = evaluator.evaluate("color")
        self.assertIsInstance(result, EvaluatedString)
        self.assertEqual(result.value, "red")

    def test_cycle_detection(self):
        prop1 = StylePropertyNode("prop1", IdentifierNode("prop2"))
        prop2 = StylePropertyNode("prop2", IdentifierNode("prop1"))
        evaluator = Evaluator([prop1, prop2], self.element_registry, self.template_registry)
        with self.assertRaisesRegex(Exception, "Circular dependency"):
            evaluator.evaluate("prop1")

if __name__ == '__main__':
    unittest.main()
