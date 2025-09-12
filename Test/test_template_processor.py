import sys
import os
import unittest
import textwrap

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.parser import Parser
from CHTL.template_registry import TemplateRegistry
from CHTL.template_processor import TemplateProcessor
from CHTL.CHTLNode.nodes import (
    StyleNode, StylePropertyNode, ElementNode, TextNode, TemplateUsageNode
)

class TestTemplateProcessor(unittest.TestCase):

    def test_style_template_expansion(self):
        source = textwrap.dedent("""
            [Template] @Style MyStyles {
                color: red;
            }
            div { style { @Style MyStyles; } }
        """)
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        registry = TemplateRegistry(ast)
        processor = TemplateProcessor(ast, registry)
        processed_ast = processor.process()

        div_node = processed_ast.children[1]
        style_node = div_node.children[0]
        self.assertIsInstance(style_node, StyleNode)
        self.assertEqual(len(style_node.rules), 1)
        self.assertIsInstance(style_node.rules[0], StylePropertyNode)
        self.assertEqual(style_node.rules[0].property_name, "color")

    def test_element_template_expansion(self):
        source = textwrap.dedent("""
            [Template] @Element MyPara {
                p { text: "Hello"; }
            }
            div { @Element MyPara; }
        """)
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        registry = TemplateRegistry(ast)
        processor = TemplateProcessor(ast, registry)
        processed_ast = processor.process()

        div_node = processed_ast.children[1]
        self.assertEqual(len(div_node.children), 1)
        p1 = div_node.children[0]
        self.assertIsInstance(p1, ElementNode)
        self.assertEqual(p1.tag_name, "p")

    def test_custom_style_delete_processing(self):
        source = textwrap.dedent("""
            [Custom] @Style MyCustom {
                color: red;
                font-size: 16px;
            }
            div {
                style {
                    @Style MyCustom {
                        delete font-size;
                    }
                }
            }
        """)
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()
        parser = Parser(tokens)
        ast = parser.parse()
        registry = TemplateRegistry(ast)
        processor = TemplateProcessor(ast, registry)
        processed_ast = processor.process()

        # Verify the processed AST
        style_node = processed_ast.children[1].children[0]
        self.assertEqual(len(style_node.rules), 1)
        self.assertEqual(style_node.rules[0].property_name, "color")


if __name__ == '__main__':
    unittest.main()
