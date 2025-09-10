import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from Scanner.scanner import UnifiedScanner

class TestUnifiedScanner(unittest.TestCase):

    def test_scan_separation(self):
        source = """
        // CHTL comment
        div {
            p { text: "Hello CHTL"; }
        }

        style {
            body { color: blue; }
        }

        [Origin] @JavaScript {
            console.log("raw js");
        }

        script {
            let x = 1;
        }
        """
        scanner = UnifiedScanner(source)
        chtl_fragment, fragments = scanner.scan()

        # Should have 3 fragments: style, origin, and script
        self.assertEqual(len(fragments), 3)

        # Check that the CHTL fragment has placeholders
        self.assertIn("__UNIFIED_SCANNER_PLACEHOLDER_", chtl_fragment)
        self.assertNotIn("body { color: blue; }", chtl_fragment)
        self.assertNotIn('console.log("raw js");', chtl_fragment)
        self.assertNotIn('let x = 1;', chtl_fragment)
        self.assertIn('p { text: "Hello CHTL"; }', chtl_fragment)

        # Check the content of the fragments
        placeholders = list(fragments.keys())

        # Note: The order depends on the order of regex execution in the scanner
        # The order is origin, style, script.

        # Origin block
        origin_placeholder = placeholders[0]
        self.assertEqual(fragments[origin_placeholder].type, 'js')
        self.assertEqual(fragments[origin_placeholder].content, 'console.log("raw js");')

        # Style block
        style_placeholder = placeholders[1]
        self.assertEqual(fragments[style_placeholder].type, 'css')
        self.assertEqual(fragments[style_placeholder].content, 'body { color: blue; }')

        # Script block
        script_placeholder = placeholders[2]
        self.assertEqual(fragments[script_placeholder].type, 'js')
        self.assertEqual(fragments[script_placeholder].content, 'let x = 1;')

if __name__ == '__main__':
    unittest.main()
