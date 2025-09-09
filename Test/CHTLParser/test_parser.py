import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode, StyleNode,
    CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode
)

class TestParser(unittest.TestCase):

    def _parse_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        context = CompilationContext()
        parser = Parser(tokens, context)
        ast = parser.parse()
        return ast, context

    def test_template_definition_parsing(self):
        source = "[Template] @Element MyBox { div {} }"
        ast, context = self._parse_source(source)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, TemplateDefinitionNode)
        self.assertEqual(def_node.name, "MyBox")

    def test_custom_definition_parsing(self):
        source = "[Custom] @Element MyBox { div {} }"
        ast, context = self._parse_source(source)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, CustomDefinitionNode)
        self.assertEqual(def_node.name, "MyBox")

    def test_template_usage_parsing(self):
        source = "body { @Element MyBox; }"
        ast, context = self._parse_source(source)
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, TemplateUsageNode)
        self.assertEqual(usage_node.name, "MyBox")

    def test_custom_usage_parsing(self):
        source = "body { @Element MyBox { delete id; } }"
        ast, context = self._parse_source(source)
        usage_node = ast.children[0].children[0]
        self.assertIsInstance(usage_node, CustomUsageNode)
        self.assertEqual(usage_node.name, "MyBox")
        self.assertEqual(len(usage_node.body), 1)
        delete_node = usage_node.body[0]
        self.assertIsInstance(delete_node, DeleteNode)
        self.assertEqual(delete_node.targets, ["id"])

if __name__ == '__main__':
    unittest.main()
