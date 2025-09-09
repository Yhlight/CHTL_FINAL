import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode, StyleNode,
    CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode, OriginNode
)

class TestParser(unittest.TestCase):

    def _parse_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        context = CompilationContext()
        parser = Parser(tokens, context)
        ast = parser.parse()
        return ast, context

    def test_custom_definition_parsing(self):
        source = "[Custom] @Element MyBox { div {} }"
        ast, context = self._parse_source(source)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, CustomDefinitionNode)
        self.assertEqual(def_node.name, "MyBox")

    def test_custom_usage_parsing(self):
        source = "body { @Element MyBox { delete id; } }"
        ast, context = self._parse_source(source)
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, CustomUsageNode)
        self.assertEqual(usage_node.name, "MyBox")
        delete_node = usage_node.body[0]
        self.assertIsInstance(delete_node, DeleteNode)
        self.assertEqual(delete_node.targets, ["id"])

    def test_insert_rule_parsing(self):
        source = "body { @Element MyBox { insert after div { p{} } } }"
        ast, context = self._parse_source(source)
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, CustomUsageNode)
        insert_node = usage_node.body[0]
        self.assertIsInstance(insert_node, InsertNode)
        self.assertEqual(insert_node.position, "after")
        self.assertEqual(insert_node.target_selector, "div")
        self.assertEqual(len(insert_node.content), 1)
        self.assertIsInstance(insert_node.content[0], ElementNode)
        self.assertEqual(insert_node.content[0].tag_name, "p")

    def test_style_override_parsing(self):
        source = "body { @Element MyBox { div { style { color: red; } } } }"
        ast, context = self._parse_source(source)
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, CustomUsageNode)
        override_div = usage_node.body[0]
        self.assertIsInstance(override_div, ElementNode)
        self.assertEqual(override_div.tag_name, "div")
        style_node = override_div.children[0]
        self.assertIsInstance(style_node, StyleNode)
        self.assertEqual(style_node.children[0].name, "color")

    def test_origin_block_parsing(self):
        source = """
        [Origin] @Html myRawBlock { <script>alert("toplevel");</script> }

        div {
            [Origin] @JavaScript { console.log("nested"); }
        }
        """
        ast, context = self._parse_source(source)

        # Test top-level origin block
        top_level_origin = ast.children[0]
        self.assertIsInstance(top_level_origin, OriginNode)
        self.assertEqual(top_level_origin.origin_type, "@Html")
        self.assertEqual(top_level_origin.name, "myRawBlock")
        self.assertIn('<script>alert("toplevel");</script>', top_level_origin.content)

        # Test nested origin block
        div_node = ast.children[1]
        nested_origin = div_node.children[0]
        self.assertIsInstance(nested_origin, OriginNode)
        self.assertEqual(nested_origin.origin_type, "@JavaScript")
        self.assertIsNone(nested_origin.name)
        self.assertIn('console.log("nested");', nested_origin.content)


if __name__ == '__main__':
    unittest.main()
