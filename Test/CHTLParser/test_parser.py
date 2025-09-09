import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode, StyleNode,
    CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode, ImportNode
)

class TestParser(unittest.TestCase):

    def _parse_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        context = CompilationContext()
        parser = Parser(tokens, context)
        ast = parser.parse()
        return ast, context

    def test_basic_element_parsing(self):
        source = "div { id: my_div; }"
        ast, _ = self._parse_source(source)
        self.assertEqual(len(ast.children), 1)
        el_node = ast.children[0]
        self.assertIsInstance(el_node, ElementNode)
        self.assertEqual(el_node.tag_name, "div")
        self.assertEqual(len(el_node.attributes), 1)
        attr_node = el_node.attributes[0]
        self.assertIsInstance(attr_node, AttributeNode)
        self.assertEqual(attr_node.name, "id")
        self.assertEqual(attr_node.value, "my_div")

    def test_text_node_parsing(self):
        source = 'p { text: "hello"; }'
        ast, _ = self._parse_source(source)
        el_node = ast.children[0]
        self.assertEqual(len(el_node.children), 1)
        text_node = el_node.children[0]
        self.assertIsInstance(text_node, TextNode)
        self.assertEqual(text_node.value, "hello")

    def test_style_block_parsing(self):
        source = "div { style { color: red; } }"
        ast, _ = self._parse_source(source)
        el_node = ast.children[0]
        style_node = el_node.children[0]
        self.assertIsInstance(style_node, StyleNode)
        prop_node = style_node.children[0]
        self.assertIsInstance(prop_node, CssPropertyNode)
        self.assertEqual(prop_node.name, "color")

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

    def test_parse_style_template(self):
        source = "[Template] @Style DefaultText { color: black; font-size: 16px; }"
        ast, _ = self._parse_source(source)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, TemplateDefinitionNode)
        self.assertEqual(def_node.template_type, "Style")
        self.assertEqual(def_node.name, "DefaultText")
        self.assertEqual(len(def_node.content), 2)
        self.assertIsInstance(def_node.content[0], CssPropertyNode)
        self.assertEqual(def_node.content[0].name, "color")

    def test_parse_var_template(self):
        source = '[Template] @Var Theme { primary: "blue"; }'
        ast, _ = self._parse_source(source)
        def_node = ast.children[0]
        self.assertIsInstance(def_node, TemplateDefinitionNode)
        self.assertEqual(def_node.template_type, "Var")
        self.assertEqual(def_node.name, "Theme")
        self.assertEqual(len(def_node.content), 1)
        self.assertIsInstance(def_node.content[0], AttributeNode)
        self.assertEqual(def_node.content[0].name, "primary")
        self.assertEqual(def_node.content[0].value, "blue")

    def test_parse_simple_import(self):
        source = '[Import] @Chtl from "my_module.chtl";'
        ast, _ = self._parse_source(source)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, ImportNode)
        self.assertEqual(import_node.import_type, "Chtl")
        self.assertEqual(import_node.path, "my_module.chtl")
        self.assertIsNone(import_node.alias)

    def test_parse_import_with_alias(self):
        source = '[Import] @Style from "./styles.css" as MyStyles;'
        ast, _ = self._parse_source(source)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, ImportNode)
        self.assertEqual(import_node.import_type, "Style")
        self.assertEqual(import_node.path, "./styles.css")
        self.assertEqual(import_node.alias, "MyStyles")

    def test_parse_precise_import(self):
        source = '[Import] [Custom] @Element MyButton from "./components.chtl";'
        ast, _ = self._parse_source(source)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, ImportNode)
        self.assertEqual(import_node.construct_type, "Custom")
        self.assertEqual(import_node.import_type, "Element")
        self.assertEqual(import_node.imported_item, "MyButton")
        self.assertEqual(import_node.path, "./components.chtl")


if __name__ == '__main__':
    unittest.main()
