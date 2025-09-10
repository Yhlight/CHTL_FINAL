import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

import copy
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, TextNode, TemplateUsageNode,
    TemplateDefinitionNode, CustomUsageNode, DeleteNode, AttributeNode,
    InsertNode, StyleNode, CssPropertyNode
)
from CHTL.CHTLTransformer.transformer import ASTTransformer

class TestASTTransformer(unittest.TestCase):

    def setUp(self):
        self.context = CompilationContext()
        box_template_content = [
            ElementNode(tag_name='div', children=[TextNode(value='hello')])
        ]
        self.context.add_element_template("MyBox", box_template_content)

        # Add a namespaced template for testing
        self.context.push_namespace("components")
        self.context.add_element_template("Button", [ElementNode(tag_name='button')])
        self.context.pop_namespace()

    def test_template_resolution_with_namespace(self):
        # AST represents: @Element Button from components;
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='Button', namespace_from=['components'])
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        # The TemplateUsageNode should be replaced by the button ElementNode
        self.assertEqual(len(transformed_ast.children), 1)
        expanded_node = transformed_ast.children[0]
        self.assertIsInstance(expanded_node, ElementNode)
        self.assertEqual(expanded_node.tag_name, 'button')

    def test_missing_template_in_namespace_error(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MissingButton', namespace_from=['components'])
        ])
        transformer = ASTTransformer(ast, self.context)
        with self.assertRaisesRegex(RuntimeError, "Template 'MissingButton' not found in namespace 'components'"):
            transformer.transform()

    def test_simple_template_expansion(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MyBox')
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        self.assertEqual(len(transformed_ast.children), 1)
        self.assertEqual(transformed_ast.children[0].tag_name, 'div')


if __name__ == '__main__':
    unittest.main()
