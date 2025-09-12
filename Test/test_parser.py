import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLLexer.token import TokenType
from CHTL.parser import Parser
from CHTL.CHTLNode.nodes import (
    RootNode, ElementNode, StyleNode, SelectorBlockNode,
    TemplateDefinitionNode, TemplateUsageNode, CustomDefinitionNode,
    CustomUsageNode, DeleteNode, InsertNode, StylePropertyNode
)

class TestParser(unittest.TestCase):

    def test_template_and_custom_definitions(self):
        source = "[Template] @Style T {} [Custom] @Element C {}"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        self.assertIsInstance(ast, RootNode)
        self.assertEqual(len(ast.children), 2)
        self.assertIsInstance(ast.children[0], TemplateDefinitionNode)
        self.assertIsInstance(ast.children[1], CustomDefinitionNode)

    def test_custom_usage_and_delete(self):
        source = "div { style { @Style MyCustom { delete prop1; } } }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()

        style_node = ast.children[0].children[0]
        custom_usage = style_node.rules[0]
        self.assertIsInstance(custom_usage, CustomUsageNode)

        delete_node = custom_usage.body[0]
        self.assertIsInstance(delete_node, DeleteNode)
        self.assertEqual(delete_node.targets, ["prop1"])

    def test_insert_statement(self):
        source = 'div { insert after p { span {} } }'
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()

        insert_node = ast.children[0].children[0]
        self.assertIsInstance(insert_node, InsertNode)
        self.assertEqual(insert_node.position, "after")
        self.assertEqual(insert_node.target_selector, "p")
        self.assertIsInstance(insert_node.body[0], ElementNode)

    def test_valueless_property(self):
        source = "[Custom] @Style S { color; }"
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()

        custom_def = ast.children[0]
        prop_node = custom_def.body[0]
        self.assertIsInstance(prop_node, StylePropertyNode)
        self.assertEqual(prop_node.property_name, "color")
        self.assertIsNone(prop_node.value)


if __name__ == '__main__':
    unittest.main()
