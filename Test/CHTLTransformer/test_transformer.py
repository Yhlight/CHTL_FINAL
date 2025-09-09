import unittest
import copy
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, TextNode, TemplateUsageNode,
    TemplateDefinitionNode, CustomUsageNode, DeleteNode, AttributeNode,
    InsertNode, StyleNode, CssPropertyNode, ImportNode
)
from CHTL.CHTLTransformer.transformer import ASTTransformer

import os

class TestASTTransformer(unittest.TestCase):

    def setUp(self):
        self.context = CompilationContext()
        box_template_content = [
            ElementNode(tag_name='div', attributes=[
                AttributeNode(name='id', value='test-id'),
                AttributeNode(name='class', value='box')
            ], children=[
                ElementNode(tag_name='p', children=[TextNode(value='hello')])
            ])
        ]
        self.context.add_element_template("MyBox", box_template_content)

        # Create files needed for import test
        with open("imported_component.chtl", "w") as f:
            f.write("""
            [Template] @Element MyImportedButton {
                button {
                    class: "imported-btn";
                    text: "I was imported!";
                }
            }
            """)
        with open("imports.chtl", "w") as f:
            f.write("""
            [Import] @Chtl from "imported_component.chtl";
            body {
                @Element MyImportedButton;
            }
            """)

    def tearDown(self):
        if os.path.exists("imported_component.chtl"):
            os.remove("imported_component.chtl")
        if os.path.exists("imports.chtl"):
            os.remove("imports.chtl")

    def test_custom_delete_attribute(self):
        ast = DocumentNode(children=[
            CustomUsageNode(template_type='Element', name='MyBox', body=[DeleteNode(targets=['id'])])
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        expanded_node = transformed_ast.children[0]
        self.assertEqual(len(expanded_node.attributes), 1)

    def test_custom_insert_at_bottom(self):
        ast = DocumentNode(children=[
            CustomUsageNode(template_type='Element', name='MyBox', body=[
                InsertNode(position='at bottom', target_selector='div', content=[ElementNode(tag_name='span')])
            ])
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        div_node = transformed_ast.children[0]
        self.assertEqual(len(div_node.children), 2)

    def test_style_override(self):
        # The override structure must match the template's structure
        override_rule = ElementNode(tag_name='div', children=[
            ElementNode(tag_name='p', children=[
                StyleNode(children=[CssPropertyNode(name='color', value='red')])
            ])
        ])
        ast = DocumentNode(children=[
            CustomUsageNode(template_type='Element', name='MyBox', body=[override_rule])
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        div_node = transformed_ast.children[0]
        p_node = div_node.children[0]

        style_node = next((c for c in p_node.children if isinstance(c, StyleNode)), None)
        self.assertIsNotNone(style_node)
        self.assertEqual(style_node.children[0].name, 'color')

        text_node = next((c for c in p_node.children if isinstance(c, TextNode)), None)
        self.assertIsNotNone(text_node)

    def test_missing_template_error(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MissingBox')
        ])
        transformer = ASTTransformer(ast, self.context)
        with self.assertRaisesRegex(RuntimeError, "Template 'MissingBox' of type 'Element' not found"):
            transformer.transform()

    def test_import_transformation(self):
        with open('imports.chtl', 'r') as f:
            source = f.read()

        context = CompilationContext()
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens, context)
        ast = parser.parse()

        # Before transformation, the AST should have 2 children: ImportNode and ElementNode
        self.assertEqual(len(ast.children), 2)
        self.assertIsInstance(ast.children[0], ImportNode)
        self.assertIsInstance(ast.children[1], ElementNode)

        transformer = ASTTransformer(ast, context)
        transformed_ast = transformer.transform()

        # After transformation, the import node should be gone, and the template should be expanded
        self.assertEqual(len(transformed_ast.children), 1)
        body_node = transformed_ast.children[0]
        self.assertEqual(body_node.tag_name, 'body')
        self.assertEqual(len(body_node.children), 1)

        button_node = body_node.children[0]
        self.assertIsInstance(button_node, ElementNode)
        self.assertEqual(button_node.tag_name, 'button')
        self.assertEqual(button_node.attributes[0].value, 'imported-btn')


if __name__ == '__main__':
    unittest.main()
