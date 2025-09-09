import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode.nodes import ElementNode, TextNode, AttributeNode, CommentNode

class TestParser(unittest.TestCase):

    def _parse_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens)
        return parser.parse()

    def test_simple_element(self):
        ast = self._parse_source("div {}")
        self.assertEqual(len(ast.children), 1)
        element = ast.children[0]
        self.assertIsInstance(element, ElementNode)
        self.assertEqual(element.tag_name, "div")
        self.assertEqual(len(element.children), 0)
        self.assertEqual(len(element.attributes), 0)

    def test_nested_elements(self):
        ast = self._parse_source("div { p { text: 'hello'; } }")
        div_node = ast.children[0]
        self.assertEqual(div_node.tag_name, "div")
        self.assertEqual(len(div_node.children), 1)
        p_node = div_node.children[0]
        self.assertIsInstance(p_node, ElementNode)
        self.assertEqual(p_node.tag_name, "p")
        self.assertEqual(len(p_node.children), 1)
        text_node = p_node.children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "hello")

    def test_attributes(self):
        ast = self._parse_source('div { id="main"; class: container; }')
        element = ast.children[0]
        self.assertEqual(len(element.attributes), 2)
        self.assertIsInstance(element.attributes[0], AttributeNode)
        self.assertEqual(element.attributes[0].name, "id")
        self.assertEqual(element.attributes[0].value, "main")
        self.assertIsInstance(element.attributes[1], AttributeNode)
        self.assertEqual(element.attributes[1].name, "class")
        self.assertEqual(element.attributes[1].value, "container")

    def test_unquoted_values(self):
        ast = self._parse_source("p { text: Hello brave new world; }")
        text_node = ast.children[0].children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "Hello brave new world")

    def test_text_block(self):
        ast = self._parse_source("p { text { Another way to write text } }")
        text_node = ast.children[0].children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "Another way to write text")

    def test_self_closing_tag(self):
        ast = self._parse_source("br;")
        element = ast.children[0]
        self.assertIsInstance(element, ElementNode)
        self.assertEqual(element.tag_name, "br")
        self.assertEqual(len(element.children), 0)

    def test_comments(self):
        ast = self._parse_source("div { -- a comment }")
        comment_node = ast.children[0].children[0]
        self.assertIsInstance(comment_node, CommentNode)
        self.assertEqual(comment_node.value, "-- a comment")

    def test_title_shorthand(self):
        ast = self._parse_source("head { title: My Page; }")
        head_node = ast.children[0]
        self.assertEqual(len(head_node.attributes), 0)
        self.assertEqual(len(head_node.children), 1)
        title_node = head_node.children[0]
        self.assertIsInstance(title_node, ElementNode)
        self.assertEqual(title_node.tag_name, "title")
        self.assertEqual(len(title_node.children), 1)
        text_node = title_node.children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "My Page")

if __name__ == '__main__':
    unittest.main()
