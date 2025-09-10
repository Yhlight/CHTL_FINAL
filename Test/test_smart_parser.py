import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes

class TestSmartParser(unittest.TestCase):

    def test_selector_assembly(self):
        source = "div { style { &:hover { color: red; } } }"
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        style_node = ast.children[0].children[0]
        selector_rule = style_node.children[0]
        self.assertIsInstance(selector_rule, nodes.StyleSelectorRuleNode)

        # Check that the tokens were assembled correctly
        selector_lexemes = "".join(t.lexeme for t in selector_rule.selector_tokens)
        self.assertEqual(selector_lexemes, "&:hover")

    def test_attribute_parsing(self):
        source = 'div { id: "test"; class = "box"; }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        div_node = ast.children[0]
        self.assertEqual(len(div_node.attributes), 2)
        self.assertEqual(div_node.attributes[0].name, "id")
        self.assertEqual(div_node.attributes[1].name, "class")

    def test_expression_parsing_precedence(self):
        source = "div { style { width: 1 + 2 * 3; } }"
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        prop_node = ast.children[0].children[0].children[0]
        expr = prop_node.value_expression

        # Should be parsed as (1 + (2 * 3))
        self.assertIsInstance(expr, nodes.BinaryOpNode)
        self.assertEqual(expr.op.lexeme, "+")

        self.assertIsInstance(expr.right, nodes.BinaryOpNode)
        self.assertEqual(expr.right.op.lexeme, "*")
        self.assertEqual(expr.right.left.value, 2.0)
        self.assertEqual(expr.right.right.value, 3.0)

    def test_conditional_expression_parsing(self):
        source = 'div { style { color: width > 50 ? "red" : "blue"; } }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        prop_node = ast.children[0].children[0].children[0]
        expr = prop_node.value_expression

        self.assertIsInstance(expr, nodes.ConditionalNode)
        self.assertIsInstance(expr.condition, nodes.BinaryOpNode)
        self.assertEqual(expr.condition.op.lexeme, ">")
        self.assertIsInstance(expr.true_branch, nodes.LiteralNode)
        self.assertEqual(expr.true_branch.value, "red")

if __name__ == '__main__':
    unittest.main()
