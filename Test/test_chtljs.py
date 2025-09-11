import unittest
import os
from unittest.mock import MagicMock, patch

# Correctly import the classes and alias them to the names used in the test.
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

    def test_full_pipeline_for_animate_block(self):
        source = """
        div {
            id: my-box;
        }
        script {
            animate {
                target: {{#my-box}},
                duration: 1000,
                callback: () => { console.log("done"); }
            }
        }
        """
        # 1. CHTL pipeline
        tokens = Lexer(source).scan_tokens()
        ast = Parser(tokens).parse()
        html, css, js = Generator().generate(ast)

        # 2. Assert JS output
        expected_js = """
        const anim = document.querySelector('#my-box').animate([], { duration: 1000 });
        anim.onfinish = () => { console.log("done"); };
        """

        self.assertEqual(
            "".join(js.split()),
            "".join(expected_js.split())
        )
        self.assertEqual(html, '<div id="my-box"></div>')


    def test_parser_for_animate_block(self):
        source_for_js_parser = """
            animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                when: [
                    { at: 50, opacity: 0.5 },
                    { at: 100, opacity: 1 }
                ],
                callback: __JS_FRAGMENT_0__
            }
        """
        tokens = CHTLJSLexer(source_for_js_parser).scan_tokens()
        ast = CHTLJSParser(tokens).parse()

        self.assertIsInstance(ast, nodes.CHTLJS_ProgramNode)
        valid_children = [c for c in ast.children if c is not None]
        self.assertEqual(len(valid_children), 1)

        anim_node = valid_children[0]
        self.assertIsInstance(anim_node, nodes.AnimateNode)

        self.assertEqual(anim_node.target, '{{box}}')
        self.assertEqual(anim_node.duration, '1000')
        self.assertEqual(anim_node.callback, '__JS_FRAGMENT_0__')

        self.assertEqual(len(anim_node.begin), 1)
        self.assertEqual(anim_node.begin[0].key, 'opacity')
        self.assertEqual(anim_node.begin[0].value, '0')

        self.assertEqual(len(anim_node.when), 2)
        self.assertIsInstance(anim_node.when[0], nodes.KeyframeNode)
        self.assertEqual(anim_node.when[0].properties[0].key, 'at')
        self.assertEqual(anim_node.when[0].properties[0].value, '50')

    def test_generator_for_animate_block(self):
        # 1. Manually create the AST
        ast = nodes.CHTLJS_ProgramNode(children=[
            nodes.AnimateNode(
                target='{{#my-box}}',
                duration='1000',
                easing='ease-in-out',
                callback='__JS_FRAGMENT_0__',
                when=[
                    nodes.KeyframeNode(properties=[
                        nodes.PropertyNode(key='at', value='0'),
                        nodes.PropertyNode(key='opacity', value='0')
                    ]),
                    nodes.KeyframeNode(properties=[
                        nodes.PropertyNode(key='at', value='100'),
                        nodes.PropertyNode(key='opacity', value='1')
                    ])
                ]
            )
        ])

        # 2. Manually create the fragments dictionary
        js_fragments = {
            "__JS_FRAGMENT_0__": "() => { console.log('Animation finished!'); }"
        }

        # 3. Instantiate and run the generator
        generator = CHTLJSGenerator(js_fragments=js_fragments)
        js_code = generator.generate(ast)

        # 4. Assert the output
        expected_js = """
        const anim = document.querySelector('#my-box').animate([{ offset: 0.0, opacity: '0' }, { offset: 1.0, opacity: '1' }], { duration: 1000, easing: 'ease-in-out' });
        anim.onfinish = () => { console.log('Animation finished!'); };
        """

        # Normalize whitespace for comparison
        self.assertEqual(
            "".join(js_code.split()),
            "".join(expected_js.split())
        )


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
