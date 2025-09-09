import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode, StyleNode,
    CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode
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

        # Test AST structure
        self.assertEqual(len(ast.children), 1)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, TemplateDefinitionNode)
        self.assertEqual(def_node.name, "MyBox")
        self.assertEqual(def_node.template_type, "Element")
        self.assertIsInstance(def_node.content[0], ElementNode)

        # Test context registration
        self.assertIsNotNone(context.get_element_template("MyBox"))
        self.assertEqual(len(context.get_element_template("MyBox")), 1)

    def test_template_usage_parsing(self):
        source = "body { @Element MyBox; }"
        ast, context = self._parse_source(source)

        body_node = ast.children[0]
        self.assertEqual(len(body_node.children), 1)
        usage_node = body_node.children[0]
        self.assertIsInstance(usage_node, TemplateUsageNode)
        self.assertEqual(usage_node.name, "MyBox")
        self.assertEqual(usage_node.template_type, "Element")

    def test_style_auto_attribute_addition(self):
        source = 'div { class: "old"; style { .box {} #main {} } }'
        ast, context = self._parse_source(source)
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
