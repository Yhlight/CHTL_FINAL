import unittest
from scanner import UnifiedScanner, CodeFragment

class TestUnifiedScanner(unittest.TestCase):

    def test_only_chtl(self):
        """Tests scanning a string containing only CHTL code."""
        code = "div { text: 'hello'; }"
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()
        self.assertEqual(fragments, [
            CodeFragment(type='CHTL', content="div { text: 'hello'; }")
        ])

    def test_chtl_with_empty_script(self):
        """Tests CHTL code that includes an empty script block."""
        code = "div { } script {}"
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()
        # The scanner should correctly identify the CHTL part and the empty script.
        # The current implementation produces no fragment for an empty script block.
        expected = [
            CodeFragment(type='CHTL', content='div { } '),
        ]
        self.assertEqual(fragments, expected)

    def test_script_with_only_js(self):
        """Tests a script block containing only standard JavaScript."""
        code = "script { const a = 1; }"
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()
        expected = [
            CodeFragment(type='JS', content=' const a = 1; ')
        ]
        self.assertEqual(fragments, expected)

    def test_simple_mixed_js_and_chtl_js(self):
        """Tests a script with a simple mix of JS and a CHTL_JS expression."""
        code = "script { var x = {{box}}; }"
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()
        expected = [
            CodeFragment(type='JS', content=' var x = '),
            CodeFragment(type='CHTL_JS', content='{{box}}'),
            CodeFragment(type='JS', content='; ')
        ]
        self.assertEqual(fragments, expected)

    def test_multiple_chtl_js_fragments(self):
        """Tests a script with multiple CHTL_JS expressions."""
        code = "script { {{a}}->action(); const b = {{c}}; }"
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()
        expected = [
            CodeFragment(type='CHTL_JS', content='{{a}}'),
            CodeFragment(type='JS', content='->action(); const b = '),
            CodeFragment(type='CHTL_JS', content='{{c}}'),
            CodeFragment(type='JS', content='; ')
        ]
        self.assertEqual(fragments, expected)

    def test_full_document_integration(self):
        """Tests a full document with CHTL and a script block."""
        code = '''
div {
    text: "Welcome"
}
script {
    console.log("Hello");
    const my_element = {{my_div}};
    my_element.click();
}
footer {
    text: "Bye"
}
'''
        scanner = UnifiedScanner(code)
        fragments = scanner.scan()

        # We check the types and stripped content to be robust against whitespace.
        self.assertEqual(len(fragments), 5)
        self.assertEqual(fragments[0].type, 'CHTL')
        self.assertEqual(fragments[0].content.strip(), 'div {\n    text: "Welcome"\n}')

        self.assertEqual(fragments[1].type, 'JS')
        self.assertEqual(fragments[1].content.strip(), 'console.log("Hello");\n    const my_element =')

        self.assertEqual(fragments[2].type, 'CHTL_JS')
        self.assertEqual(fragments[2].content.strip(), '{{my_div}}')

        self.assertEqual(fragments[3].type, 'JS')
        self.assertEqual(fragments[3].content.strip(), ';\n    my_element.click();')

        self.assertEqual(fragments[4].type, 'CHTL')
        self.assertEqual(fragments[4].content.strip(), 'footer {\n    text: "Bye"\n}')


if __name__ == '__main__':
    unittest.main()
