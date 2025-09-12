import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token import TokenType
from CHTL.parser import Parser
from CHTL.CHTLNode.nodes import (
    RootNode, ElementNode, AttributeNode, TextNode, CommentNode,
    StyleNode, StylePropertyNode, BinaryOpNode, DimensionNode, ConditionalNode,
    IdentifierNode, AttributeReferenceNode, SelectorBlockNode
)

class TestParser(unittest.TestCase):

    def test_parser_structure(self):
        source = """
-- A generator comment
div {
    id = "container";
    class: box;
    span { text: "Hello"; }
    p { text { "World" } }
}
"""
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        self.assertIsInstance(ast, RootNode)
        self.assertEqual(len(ast.children), 2)

    def test_style_block_parsing(self):
        source = "div { style { width: 100px; } }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        style_node = ast.children[0].children[0]
        self.assertIsInstance(style_node, StyleNode)
        self.assertEqual(len(style_node.rules), 1)

    def test_style_expression_parsing(self):
        source = "div { style { width: 10 + 2 * 3px; } }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        expr_root = ast.children[0].children[0].rules[0].value
        self.assertIsInstance(expr_root, BinaryOpNode)
        self.assertEqual(expr_root.op.type, TokenType.PLUS)

    def test_conditional_expression_parsing(self):
        source = 'div { style { color: width > 10px ? "red" : "blue"; } }'
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        expr_root = ast.children[0].children[0].rules[0].value
        self.assertIsInstance(expr_root, ConditionalNode)

    def test_attribute_reference_parsing(self):
        source = 'div { style { width: .box.width; } }'
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        expr_root = ast.children[0].children[0].rules[0].value
        self.assertIsInstance(expr_root, AttributeReferenceNode)

    def test_selector_block_parsing(self):
        source = 'div { style { .my-class { color: red; } } }'
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        style_node = ast.children[0].children[0]
        self.assertIsInstance(style_node.rules[0], SelectorBlockNode)
        self.assertEqual(style_node.rules[0].selector, ".my-class")

if __name__ == '__main__':
    unittest.main()
