import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes

class TestParser(unittest.TestCase):

    def test_nested_elements(self):
        source = "html { body { div {} } }"
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        self.assertIsInstance(ast.children[0], nodes.ElementNode)
        self.assertEqual(ast.children[0].tag_name, "html")
        self.assertEqual(ast.children[0].children[0].tag_name, "body")
        self.assertEqual(ast.children[0].children[0].children[0].tag_name, "div")

    def test_text_node(self):
        source = """p { text { Hello World } }"""
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        p_node = ast.children[0]
        text_node = p_node.children[0]
        self.assertIsInstance(text_node, nodes.TextNode)
        self.assertEqual(text_node.value, "Hello World")

    def test_style_parsing(self):
        source = """
        div {
            style {
                width: 100px;
                .box {
                    color: blue;
                }
                &:hover {
                    opacity: 0.8;
                }
            }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        div_node = ast.children[0]
        self.assertEqual(div_node.tag_name, "div")

        style_node = div_node.children[0]
        self.assertIsInstance(style_node, nodes.StyleNode)
        self.assertEqual(len(style_node.children), 3)

        # Check property
        prop = style_node.children[0]
        self.assertIsInstance(prop, nodes.StylePropertyNode)
        self.assertEqual(prop.name, "width")
        self.assertEqual(prop.value, "100 px") # Note: lexer separates 100 and px

        # Check first rule
        rule1 = style_node.children[1]
        self.assertIsInstance(rule1, nodes.StyleSelectorRuleNode)
        self.assertEqual(rule1.selector, ".box")
        self.assertEqual(rule1.properties[0].name, "color")

        # Check second rule
        rule2 = style_node.children[2]
        self.assertIsInstance(rule2, nodes.StyleSelectorRuleNode)
        self.assertEqual(rule2.selector, "&:hover")
        self.assertEqual(rule2.properties[0].name, "opacity")

    def test_attribute_parsing(self):
        source = 'div { id = "test-id"; }'
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        div_node = ast.children[0]
        self.assertEqual(len(div_node.attributes), 1)
        self.assertEqual(div_node.attributes[0].name, "id")
        self.assertEqual(div_node.attributes[0].value, "test-id")


if __name__ == '__main__':
    unittest.main()
