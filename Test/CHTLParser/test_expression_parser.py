import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.expression_parser import ExpressionParser
from CHTL.CHTLNode.nodes import BinaryOpNode, LiteralNode, PropertyReferenceNode, ConditionalExpressionNode, LogicalOpNode

class TestExpressionParser(unittest.TestCase):

    def _parse_expr(self, source_str):
        lexer = Lexer(source_str)
        tokens = lexer.tokenize()
        # Remove EOF token for parser
        parser = ExpressionParser(tokens[:-1])
        return parser.parse()

    def test_simple_addition(self):
        ast = self._parse_expr("1 + 2")
        self.assertIsInstance(ast, BinaryOpNode)
        self.assertEqual(ast.op.value, '+')
        self.assertIsInstance(ast.left, LiteralNode)
        self.assertEqual(ast.left.value, '1')
        self.assertIsInstance(ast.right, LiteralNode)
        self.assertEqual(ast.right.value, '2')

    def test_operator_precedence(self):
        ast = self._parse_expr("1 + 2 * 3")
        self.assertIsInstance(ast, BinaryOpNode)
        self.assertEqual(ast.op.value, '+')
        # Right side should be the multiplication
        right_node = ast.right
        self.assertIsInstance(right_node, BinaryOpNode)
        self.assertEqual(right_node.op.value, '*')
        self.assertEqual(right_node.left.value, '2')
        self.assertEqual(right_node.right.value, '3')

    def test_parentheses_override_precedence(self):
        ast = self._parse_expr("(1 + 2) * 3")
        self.assertIsInstance(ast, BinaryOpNode)
        self.assertEqual(ast.op.value, '*')
        # Left side should be the addition
        left_node = ast.left
        self.assertIsInstance(left_node, BinaryOpNode)
        self.assertEqual(left_node.op.value, '+')
        self.assertEqual(left_node.left.value, '1')
        self.assertEqual(left_node.right.value, '2')

    def test_ternary_operator(self):
        ast = self._parse_expr("a > b ? 10 : 20")
        self.assertIsInstance(ast, ConditionalExpressionNode)
        # Condition
        cond_node = ast.condition
        self.assertIsInstance(cond_node, BinaryOpNode)
        self.assertEqual(cond_node.op.value, '>')
        # True expression
        self.assertIsInstance(ast.true_expr, LiteralNode)
        self.assertEqual(ast.true_expr.value, '10')
        # False expression
        self.assertIsInstance(ast.false_expr, LiteralNode)
        self.assertEqual(ast.false_expr.value, '20')

    def test_property_reference(self):
        ast = self._parse_expr(".box.width")
        self.assertIsInstance(ast, PropertyReferenceNode)
        self.assertEqual(ast.selector, ".box")
        self.assertEqual(ast.property_name, "width")

    def test_local_property_reference(self):
        ast = self._parse_expr("width")
        self.assertIsInstance(ast, PropertyReferenceNode)
        self.assertIsNone(ast.selector)
        self.assertEqual(ast.property_name, "width")

    def test_logical_operators(self):
        ast = self._parse_expr("a && b || c")
        self.assertIsInstance(ast, LogicalOpNode)
        self.assertEqual(ast.op.value, '||')
        # Left side should be the AND expression
        left_node = ast.left
        self.assertIsInstance(left_node, LogicalOpNode)
        self.assertEqual(left_node.op.value, '&&')
        self.assertEqual(left_node.left.property_name, 'a')
        self.assertEqual(left_node.right.property_name, 'b')
        # Right side of OR
        self.assertEqual(ast.right.property_name, 'c')

if __name__ == '__main__':
    unittest.main()
