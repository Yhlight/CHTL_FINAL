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
    IdentifierNode, AttributeReferenceNode, SelectorBlockNode,
    TemplateDefinitionNode, TemplateUsageNode
)

class TestParser(unittest.TestCase):

    def test_parser_structure(self):
        source = "div { id: 'a'; }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        self.assertIsInstance(ast, RootNode)

    def test_style_block_parsing(self):
        source = "div { style { width: 100px; } }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        style_node = ast.children[0].children[0]
        self.assertIsInstance(style_node, StyleNode)

    def test_selector_block_parsing(self):
        source = 'div { style { .my-class { color: red; } } }'
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        style_node = ast.children[0].children[0]
        self.assertIsInstance(style_node.rules[0], SelectorBlockNode)

    def test_template_definition_parsing(self):
        source = "[Template] @Style MyStyles { color: red; }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        self.assertIsInstance(ast.children[0], TemplateDefinitionNode)
        self.assertEqual(ast.children[0].name, "MyStyles")

    def test_template_usage_parsing(self):
        source = "div { @Element MyElement; }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, TemplateUsageNode)
        self.assertEqual(usage_node.name, "MyElement")

    def test_template_inheritance_parsing(self):
        source = "div { style { inherit @Style MyStyles; } }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        usage_node = ast.children[0].children[0].rules[0]
        self.assertIsInstance(usage_node, TemplateUsageNode)

if __name__ == '__main__':
    unittest.main()
