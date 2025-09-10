import unittest
import sys
import os
import re

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

class TestAdvancedStyles(unittest.TestCase):

    def normalize_html(self, html: str) -> str:
        """Removes whitespace and newlines to make comparison easier."""
        return re.sub(r'\s+', '', html)

    def test_ampersand_selector_resolution_with_class(self):
        source = """
        html {
            head {}
            body {
                div {
                    class: "box";
                    style {
                        &:hover {
                            color: red;
                        }
                    }
                }
            }
        }
        """

        expected_css = ".box:hover{color:red;}"

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        # Extract the content of the style tag for comparison
        style_content = re.search(r'<style>(.*?)</style>', generated_html, re.DOTALL)
        self.assertIsNotNone(style_content, "Style tag not found in output")

        normalized_generated_css = self.normalize_html(style_content.group(1))
        self.assertEqual(normalized_generated_css, expected_css)

    def test_ampersand_selector_resolution_with_id(self):
        source = """
        html {
            head {}
            body {
                div {
                    id: "main";
                    style {
                        &::before {
                            content: "";
                        }
                    }
                }
            }
        }
        """

        expected_css = "#main::before{content:\"\";}"

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        style_content = re.search(r'<style>(.*?)</style>', generated_html, re.DOTALL)
        self.assertIsNotNone(style_content, "Style tag not found in output")

        normalized_generated_css = self.normalize_html(style_content.group(1))
        self.assertEqual(normalized_generated_css, expected_css)


    def test_auto_attribute_injection(self):
        source = """
        html {
            head {}
            body {
                div {
                    style {
                        .box { color: blue; }
                        #main-content { border: 1px solid black; }
                    }
                }
            }
        }
        """

        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        generated_html = Generator().generate(ast)

        # Check that the div now has the injected class and id
        self.assertIn('class="box"', generated_html)
        self.assertIn('id="main-content"', generated_html)


if __name__ == '__main__':
    unittest.main()
