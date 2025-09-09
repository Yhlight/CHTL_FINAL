import unittest
import copy
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import DocumentNode, ElementNode, TextNode, TemplateUsageNode, TemplateDefinitionNode
from CHTL.CHTLTransformer.transformer import ASTTransformer

class TestASTTransformer(unittest.TestCase):

    def setUp(self):
        self.context = CompilationContext()
        template_content = [ElementNode(tag_name='div', children=[TextNode(value='template content')])]
        self.context.add_element_template("MyBox", template_content)

    def test_template_expansion(self):
        ast = DocumentNode(children=[
            ElementNode(tag_name='body', children=[
                TemplateUsageNode(template_type='Element', name='MyBox', lineno=1)
            ])
        ])

        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        body_node = transformed_ast.children[0]
        self.assertEqual(len(body_node.children), 1)

        expanded_node = body_node.children[0]
        self.assertIsInstance(expanded_node, ElementNode)
        self.assertEqual(expanded_node.tag_name, 'div')
        self.assertEqual(expanded_node.children[0].value, 'template content')

    def test_template_definition_removal(self):
        ast = DocumentNode(children=[
            TemplateDefinitionNode(template_type='Element', name='Unused', content=[]),
            ElementNode(tag_name='body')
        ])

        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        self.assertEqual(len(transformed_ast.children), 1)
        self.assertIsInstance(transformed_ast.children[0], ElementNode)

    def test_multiple_usage_deepcopy(self):
        ast = DocumentNode(children=[
            ElementNode(tag_name='body', children=[
                TemplateUsageNode(template_type='Element', name='MyBox', lineno=1),
                TemplateUsageNode(template_type='Element', name='MyBox', lineno=2)
            ])
        ])

        transformer = ASTTransformer(ast, self.context)
        transformed_ast = transformer.transform()

        body_node = transformed_ast.children[0]
        self.assertEqual(len(body_node.children), 2)

        self.assertIsNot(body_node.children[0], body_node.children[1])

    def test_missing_template_error(self):
        ast = DocumentNode(children=[
            TemplateUsageNode(template_type='Element', name='MissingBox', lineno=1)
        ])

        transformer = ASTTransformer(ast, self.context)
        # Update the expected regex to match the more descriptive error message
        with self.assertRaisesRegex(RuntimeError, "Template 'MissingBox' of type 'Element' not found"):
            transformer.transform()

if __name__ == '__main__':
    unittest.main()
