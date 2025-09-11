import unittest
import os
import re
from unittest.mock import MagicMock, patch

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator
from Scanner.unified_scanner import CHTLUnifiedScanner
from CHTL_JS.CHTLJSLexer.token_type import CHTLJSTokenType
from CHTL_JS.CHTLJSParser.parser import CHTLJSParseError
from CHTL_JS.CHTLJSLexer.lexer import CHTLJSLexer
from CHTL_JS.CHTLJSParser.parser import CHTLJSParser
from CHTL_JS.CHTLJSNode import nodes
from CHTL_JS.CHTLJSGenerator.generator import CHTLJSGenerator


class TestCHTLJSPipeline(unittest.TestCase):

    def _get_script_content(self, html: str) -> str:
        """Extracts content from the first <script> tag."""
        match = re.search(r'<script>(.*?)</script>', html, re.DOTALL)
        return match.group(1).strip() if match else ""

    def test_full_pipeline_for_animate_block(self):
        source = """
        html {
            body {
                div { id: my-box; }
                script {
                    animate {
                        target: {{#my-box}},
                        duration: 1000,
                        callback: () => { console.log("done"); }
                    }
                }
            }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        html_output = Generator().generate(ast)
        js_code = self._get_script_content(html_output)

        expected_js = """
        const anim = document.querySelector('#my-box').animate([], { duration: 1000 });
        anim.onfinish = () => { console.log("done"); };
        """

        self.assertEqual("".join(js_code.split()), "".join(expected_js.split()))
        self.assertIn('<div id="my-box"></div>', html_output)

    def test_full_pipeline_for_delegate_block(self):
        source = """
        html {
            body {
                ul {
                    id: my-list;
                    li { text { Item 1 } }
                    li { class: special; text { Item 2 } }
                }
                script {
                    {{#my-list}}->delegate {
                        target: {{li}},
                        click: () => { console.log("Li clicked!"); }
                    }

                    {{#my-list}}->delegate {
                        target: {{.special}},
                        mouseenter: () => { console.log("Special item hovered!"); }
                    }
                }
            }
        }
        """
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        html_output = Generator().generate(ast)
        js_code = self._get_script_content(html_output)

        norm_js = re.sub(r'\s+', ' ', js_code).strip()
        self.assertIn("document.querySelector('#my-list')", norm_js)
        self.assertIn(".addEventListener('click', (event) => {", norm_js)
        self.assertIn("if (event.target.matches('li'))", norm_js)
        self.assertIn('console.log("Li clicked!")', norm_js)
        self.assertIn(".addEventListener('mouseenter', (event) => {", norm_js)
        self.assertIn("if (event.target.matches('.special'))", norm_js)
        self.assertIn('console.log("Special item hovered!")', norm_js)
        self.assertEqual(norm_js.count("addEventListener('click'"), 1)
        self.assertEqual(norm_js.count("addEventListener('mouseenter'"), 1)

    def test_parser_for_virtual_object(self):
        source = """
            vir myListener = {{#my-btn}}->listen {
                click: __JS_FRAGMENT_0__
            };
            myListener->click;
        """
        tokens = CHTLJSLexer(source).scan_tokens()
        ast = CHTLJSParser(tokens).parse()

        self.assertEqual(len(ast.children), 2)
        vir_node = ast.children[0]
        self.assertIsInstance(vir_node, nodes.VirtualObjectNode)
        self.assertEqual(vir_node.name, "myListener")
        self.assertIsInstance(vir_node.value, nodes.ListenBlockNode)

        expr_stmt_node = ast.children[1]
        self.assertIsInstance(expr_stmt_node, nodes.ExpressionStatementNode)
        self.assertIsInstance(expr_stmt_node.expression, nodes.MemberAccessNode)

        member_access = expr_stmt_node.expression
        self.assertEqual(member_access.object_name, "myListener")
        self.assertEqual(member_access.member_name, "click")

    def test_parser_for_animate_block(self):
        source_for_js_parser = """
            animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                when: [ { at: 50, opacity: 0.5 }, { at: 100, opacity: 1 } ],
                callback: __JS_FRAGMENT_0__
            }
        """
        tokens = CHTLJSLexer(source_for_js_parser).scan_tokens()
        ast = CHTLJSParser(tokens).parse()
        valid_children = [c for c in ast.children if c is not None]
        self.assertEqual(len(valid_children), 1)
        anim_node = valid_children[0]
        self.assertIsInstance(anim_node, nodes.AnimateNode)
        self.assertEqual(anim_node.target, '{{box}}')
        self.assertEqual(anim_node.callback, '__JS_FRAGMENT_0__')
        self.assertEqual(len(anim_node.when), 2)

    def test_generator_for_animate_block(self):
        ast = nodes.CHTLJS_ProgramNode(children=[
            nodes.AnimateNode(
                target='{{#my-box}}', duration='1000', easing='ease-in-out',
                callback='__JS_FRAGMENT_0__',
                when=[
                    nodes.KeyframeNode(properties=[nodes.PropertyNode(key='at', value='0'), nodes.PropertyNode(key='opacity', value='0')]),
                    nodes.KeyframeNode(properties=[nodes.PropertyNode(key='at', value='100'), nodes.PropertyNode(key='opacity', value='1')])
                ]
            )
        ])
        js_fragments = { "__JS_FRAGMENT_0__": "() => { console.log('Animation finished!'); }" }
        generator = CHTLJSGenerator(js_fragments=js_fragments)
        js_code = generator.generate(ast)
        expected_js = """
        const anim = document.querySelector('#my-box').animate([{ offset: 0.0, opacity: '0' }, { offset: 1.0, opacity: '1' }], { duration: 1000, easing: 'ease-in-out' });
        anim.onfinish = () => { console.log('Animation finished!'); };
        """
        self.assertEqual("".join(js_code.split()), "".join(expected_js.split()))

    def test_scanner_for_animate_block(self):
        source_fragment = """
            animate {
                target: {{box}},
                duration: 1000,
                callback: function() { console.log("done"); }
            }
        """
        scanner = CHTLUnifiedScanner()
        processed_fragment, js_fragments = scanner.scan_script_fragment(source_fragment)
        self.assertEqual(len(js_fragments), 1)
        placeholder = list(js_fragments.keys())[0]
        self.assertIn(placeholder, processed_fragment)
        self.assertNotIn("function()", processed_fragment)
        self.assertEqual(js_fragments[placeholder], 'function() { console.log("done"); }')

if __name__ == '__main__':
    unittest.main()
