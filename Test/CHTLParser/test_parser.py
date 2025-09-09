import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode,
    StyleNode, CssRuleNode, CssPropertyNode
)

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

    def test_nested_elements(self):
        ast = self._parse_source("div { p { text: 'hello'; } }")
        div_node = ast.children[0]
        p_node = div_node.children[0]
        self.assertIsInstance(p_node, ElementNode)
        text_node = p_node.children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "hello")

    def test_attributes(self):
        ast = self._parse_source('div { id="main"; class: container; }')
        element = ast.children[0]
        self.assertEqual(len(element.attributes), 2)
        self.assertEqual(element.attributes[0].name, "id")
        self.assertEqual(element.attributes[0].value, "main")
        self.assertEqual(element.attributes[1].name, "class")
        self.assertEqual(element.attributes[1].value, "container")

    def test_unquoted_values(self):
        ast = self._parse_source("p { text: Hello brave new world; }")
        text_node = ast.children[0].children[0]
        self.assertEqual(text_node.value, "Hello brave new world")

    def test_text_block(self):
        ast = self._parse_source("p { text { Another way to write text } }")
        text_node = ast.children[0].children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "Another way to write text")

    def test_self_closing_tag(self):
        ast = self._parse_source("br;")
        element = ast.children[0]
        self.assertEqual(element.tag_name, "br")

    def test_comment_inside_element(self):
        # Test a valid comment on its own line
        ast = self._parse_source("div {\n -- a comment\n }")
        comment_node = ast.children[0].children[0]
        self.assertIsInstance(comment_node, CommentNode)
        self.assertEqual(comment_node.value, "-- a comment")

    def test_ambiguous_comment_fails(self):
        # Test that a comment consuming a closing brace is invalid syntax
        # The parser should fail because it reaches EOF without finding the RBRACE
        with self.assertRaisesRegex(RuntimeError, "Expected token TokenType.RBRACE"):
            self._parse_source("div { -- a comment }")

    def test_title_shorthand(self):
        ast = self._parse_source("head { title: My Page; }")
        title_node = ast.children[0].children[0]
        self.assertEqual(title_node.tag_name, "title")
        self.assertEqual(title_node.children[0].value, "My Page")

    def test_style_block_parsing(self):
        ast = self._parse_source("div { style { color: red; .box { width: 100px; } } }")
        div_node = ast.children[0]
        style_node = div_node.children[0]
        self.assertIsInstance(style_node, StyleNode)
        self.assertEqual(len(style_node.children), 2)

        prop_node = style_node.children[0]
        self.assertIsInstance(prop_node, CssPropertyNode)
        self.assertEqual(prop_node.name, "color")
        self.assertEqual(prop_node.value, "red")

        rule_node = style_node.children[1]
        self.assertIsInstance(rule_node, CssRuleNode)
        self.assertEqual(rule_node.selector, ".box")
        self.assertEqual(len(rule_node.properties), 1)
        self.assertEqual(rule_node.properties[0].name, "width")

    def test_style_auto_attribute_addition(self):
        ast = self._parse_source('div { class: "old"; style { .box {} #main {} } }')
        div_node = ast.children[0]

        class_attr = next((attr for attr in div_node.attributes if attr.name == 'class'), None)
        self.assertIsNotNone(class_attr)
        self.assertIn("old", class_attr.value.split())
        self.assertIn("box", class_attr.value.split())

        id_attr = next((attr for attr in div_node.attributes if attr.name == 'id'), None)
        self.assertIsNotNone(id_attr)
        self.assertEqual(id_attr.value, "main")

if __name__ == '__main__':
    unittest.main()
