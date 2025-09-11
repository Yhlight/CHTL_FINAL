import unittest
import sys
import os
import re

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestStyleFeatures(unittest.TestCase):

    def normalize(self, text: str) -> str:
        """Removes all whitespace for resilient comparison."""
        return re.sub(r'\s', '', text)

    def compile_chtl(self, source: str) -> str:
        """Helper to run the full pipeline."""
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        return Generator().generate(ast)

    def test_inline_styles(self):
        source = """
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
        """
        expected = '<div style="color:red; font-size:16px;"></div>'
        self.assertEqual(self.normalize(self.compile_chtl(source)), self.normalize(expected))

    def test_global_styles(self):
        source = """
        div {
            class: "main";
            style {
                .main { color: blue; }
                &:hover { color: green; }
            }
        }
        """
        html = self.compile_chtl(source)
        # Check for the style block in the head
        self.assertIn('<style>', html)
        self.assertIn('.main{color:blue;}', self.normalize(html))
        self.assertIn('.main:hover{color:green;}', self.normalize(html))
        # Check that the class was added to the element
        self.assertIn('<divclass="main"></div>', self.normalize(html))

    def test_style_arithmetic(self):
        source = "div { style { width: 100px + 50; height: 10 * 2em; } }"
        expected = '<div style="height:20em; width:150px;"></div>'
        self.assertEqual(self.normalize(self.compile_chtl(source)), self.normalize(expected))

    def test_style_references(self):
        source = """
        div {
            id: header;
            style { height: 100px; }
        }
        p {
            style {
                font-size: #header.height / 10; // 100px / 10 = 10px
            }
        }
        """
        html = self.compile_chtl(source)
        self.assertIn('style="font-size:10px;"', self.normalize(html))

    def test_style_conditionals(self):
        source = """
        div {
            style {
                width: 100px;
                color: width > 50 ? "red" : "blue";
                background-color: width < 50 ? "green" : "yellow";
            }
        }
        """
        html = self.compile_chtl(source)
        self.assertIn('style="background-color:yellow;color:red;width:100px;"', self.normalize(html))

if __name__ == '__main__':
    unittest.main()
