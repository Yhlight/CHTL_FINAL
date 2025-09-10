import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode import nodes

class TestParser(unittest.TestCase):

    def test_parsing_nested_elements(self):
        source = """
        html {
            body {
                div {}
            }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        self.assertIsInstance(ast, nodes.ProgramNode)
        self.assertEqual(len(ast.children), 1)

        html_node = ast.children[0]
        self.assertIsInstance(html_node, nodes.ElementNode)
        self.assertEqual(html_node.tag_name, "html")
        self.assertEqual(len(html_node.children), 1)

        body_node = html_node.children[0]
        self.assertIsInstance(body_node, nodes.ElementNode)
        self.assertEqual(body_node.tag_name, "body")
        self.assertEqual(len(body_node.children), 1)

        div_node = body_node.children[0]
        self.assertIsInstance(div_node, nodes.ElementNode)
        self.assertEqual(div_node.tag_name, "div")
        self.assertEqual(len(div_node.children), 0)

    def test_parsing_attributes(self):
        source = """
        div {
            id: "main";
            class = "container";
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        div_node = ast.children[0]
        self.assertEqual(len(div_node.attributes), 2)

        attr1 = div_node.attributes[0]
        self.assertIsInstance(attr1, nodes.AttributeNode)
        self.assertEqual(attr1.name, "id")
        self.assertEqual(attr1.value, "main")

        attr2 = div_node.attributes[1]
        self.assertIsInstance(attr2, nodes.AttributeNode)
        self.assertEqual(attr2.name, "class")
        self.assertEqual(attr2.value, "container")

    def test_parsing_text_node(self):
        source = """
        p {
            text { "Hello, CHTL!" }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        p_node = ast.children[0]
        self.assertEqual(len(p_node.children), 1)

        text_node = p_node.children[0]
        self.assertIsInstance(text_node, nodes.TextNode)
        self.assertEqual(text_node.value, "Hello, CHTL!")

    def test_parsing_unquoted_literal_text(self):
        source = """
        p {
            text { Hello_World }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()

        p_node = ast.children[0]
        text_node = p_node.children[0]
        self.assertIsInstance(text_node, nodes.TextNode)
        self.assertEqual(text_node.value, "Hello_World")

if __name__ == '__main__':
    unittest.main()
