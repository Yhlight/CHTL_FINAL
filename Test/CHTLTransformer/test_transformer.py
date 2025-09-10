import unittest
import copy
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    DocumentNode, ElementNode, TextNode, TemplateUsageNode,
    TemplateDefinitionNode, CustomUsageNode, DeleteNode, AttributeNode,
    InsertNode, StyleNode, CssPropertyNode, OriginNode
)
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator

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

        # Create a dummy module structure for testing resolution
        self.test_dir = "test_temp_dir"
        self.module_dir = os.path.join(self.test_dir, "module")
        os.makedirs(self.module_dir, exist_ok=True)

        self.imports_file_path = os.path.join(self.test_dir, "imports.chtl")

        # Create a source directory for the component that we will package
        component_source_dir = os.path.join(self.test_dir, "MyComponent")
        os.makedirs(os.path.join(component_source_dir, "src"), exist_ok=True)
        os.makedirs(os.path.join(component_source_dir, "info"), exist_ok=True)

        component_file_path = os.path.join(component_source_dir, "src", "MyComponent.chtl")
        info_file_path = os.path.join(component_source_dir, "info", "MyComponent.chtl")

        with open(component_file_path, "w") as f:
            f.write("""
            [Template] @Element MyImportedButton {
                button {
                    class: "imported-btn";
                    text: "I was imported!";
                }
            }
            [Custom] @Element MyCustomBox {
                div { class: "box"; }
            }
            """)
        with open(self.imports_file_path, "w") as f:
            f.write("""
            [Import] @Chtl from "MyComponent";
            body {
                @Element MyImportedButton from MyComponent;

                @Element MyCustomBox from MyComponent {
                    div {
                        style {
                            border: 1px solid black;
                        }
                    }
                }
            }
            """)

        with open(info_file_path, "w") as f:
            f.write("""
            [Info] { name = "MyComponent"; }
            """)

        # Programmatically create the .cmod file for the test
        from tools.package_cmod import create_cmod_archive
        # Place the .cmod file where the resolver will find it (in the module dir)
        create_cmod_archive(component_source_dir, output_path=self.module_dir)

        # Create files for raw import test
        with open("test_styles.css", "w") as f:
            f.write(".imported-class { font-weight: bold; }")

        with open("test_raw_import.chtl", "w") as f:
            f.write("""
            [Import] @Style from "test_styles.css" as my_styles;
            [Origin] @Style my_styles;
            """)


    def tearDown(self):
        import shutil
        if os.path.exists(self.test_dir):
            shutil.rmtree(self.test_dir)
        if os.path.exists("test_styles.css"):
            os.remove("test_styles.css")
        if os.path.exists("test_raw_import.chtl"):
            os.remove("test_raw_import.chtl")

    def test_custom_delete_attribute(self):
        ast = DocumentNode(children=[
            CustomUsageNode(template_type='Element', name='MyBox', body=[DeleteNode(targets=['id'])])
        ])
        transformer = ASTTransformer(ast, self.context, "test.chtl")
        transformed_ast = transformer.transform()
        expanded_node = transformed_ast.children[0]
        self.assertEqual(len(expanded_node.attributes), 1)

    def test_custom_insert_at_bottom(self):
        ast = DocumentNode(children=[
            CustomUsageNode(template_type='Element', name='MyBox', body=[
                InsertNode(position='at bottom', target_selector='div', content=[ElementNode(tag_name='span')])
            ])
        ])
        transformer = ASTTransformer(ast, self.context, "test.chtl")
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
        transformer = ASTTransformer(ast, self.context, "test.chtl")
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
        transformer = ASTTransformer(ast, self.context, "test.chtl")
        with self.assertRaisesRegex(RuntimeError, "Template 'MissingBox' of type 'Element' not found"):
            transformer.transform()

    def test_import_transformation(self):
        with open(self.imports_file_path, 'r') as f:
            source = f.read()

        context = CompilationContext()
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        parser = Parser(tokens, context)
        ast = parser.parse()

        # Pass the source file path to the transformer
        transformer = ASTTransformer(ast, context, self.imports_file_path)
        transformed_ast = transformer.transform()

        # After transformation, the import node should be gone, and templates should be expanded
        self.assertEqual(len(transformed_ast.children), 1)
        body_node = transformed_ast.children[0]
        self.assertEqual(body_node.tag_name, 'body')
        self.assertEqual(len(body_node.children), 2) # Button and Box

        # Check the imported button
        button_node = body_node.children[0]
        self.assertIsInstance(button_node, ElementNode)
        self.assertEqual(button_node.tag_name, 'button')
        self.assertEqual(button_node.attributes[0].value, 'imported-btn')

        # Check the customized box
        box_node = body_node.children[1]
        self.assertIsInstance(box_node, ElementNode)
        self.assertEqual(box_node.tag_name, 'div')
        self.assertEqual(box_node.attributes[0].value, 'box')
        # Check that the style override was applied
        style_node = box_node.children[0]
        self.assertIsInstance(style_node, StyleNode)
        self.assertEqual(style_node.children[0].name, 'border')

    def test_raw_file_import_and_origin_usage(self):
        with open('test_raw_import.chtl', 'r') as f:
            source = f.read()

        context = CompilationContext()
        parser = Parser(Lexer(source, context).tokenize(), context)
        ast = parser.parse()

        # The transformer needs the path to resolve the import
        transformer = ASTTransformer(ast, context, "test_raw_import.chtl")
        transformed_ast = transformer.transform()

        # After transformation, the AST should contain just one OriginNode with the content.
        self.assertEqual(len(transformed_ast.children), 1)

        origin_node = transformed_ast.children[0]
        self.assertIsInstance(origin_node, OriginNode)
        self.assertIn(".imported-class", origin_node.content)
        self.assertIn("font-weight: bold;", origin_node.content)

        # Now, let's check the final generated output
        generator = HTMLGenerator(transformed_ast, context)
        html = generator.generate()

        self.assertIn(".imported-class", html)
        self.assertIn("font-weight: bold;", html)


if __name__ == '__main__':
    unittest.main()
