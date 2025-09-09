import unittest
import copy
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, TextNode, TemplateUsageNode,
    TemplateDefinitionNode, CustomUsageNode, DeleteNode, AttributeNode
)
from CHTL.CHTLTransformer.transformer import ASTTransformer

class TestASTTransformer(unittest.TestCase):

    def setUp(self):
        self.context = CompilationContext()
        template_content = [
            ElementNode(tag_name='div', attributes=[
                AttributeNode(name='id', value='test-id'),
                AttributeNode(name='class', value='box')
            ])
        ]
        self.context.add_element_template("MyBox", template_content)

    def test_template_expansion(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MyBox')
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        expanded_node = transformed_ast.children[0]
        self.assertEqual(expanded_node.tag_name, 'div')

    def test_custom_delete_attribute(self):
        ast = DocumentNode(children=[
            CustomUsageNode(
                template_type='Element',
                name='MyBox',
                body=[DeleteNode(targets=['id'])]
            )
        ])

        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        expanded_node = transformed_ast.children[0]
        self.assertIsInstance(expanded_node, ElementNode)
        self.assertEqual(len(expanded_node.attributes), 1)
        self.assertEqual(expanded_node.attributes[0].name, 'class')

    def test_template_definition_removal(self):
        ast = DocumentNode(children=[
            TemplateDefinitionNode(name='Unused', template_type='Element', content=[])
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        self.assertEqual(len(transformed_ast.children), 0)

    def test_multiple_usage_deepcopy(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MyBox'),
            TemplateUsageNode(template_type='Element', name='MyBox')
        ])
        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()
        self.assertIsNot(transformed_ast.children[0], transformed_ast.children[1])

    def test_missing_template_error(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MissingBox')
        ])
        transformer = ASTTransformer(ast, self.context)
        with self.assertRaisesRegex(RuntimeError, "Template 'MissingBox' of type 'Element' not found"):
            transformer.transform()

if __name__ == '__main__':
    unittest.main()
