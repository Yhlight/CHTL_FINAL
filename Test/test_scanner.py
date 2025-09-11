import unittest
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from Scanner.unified_scanner import CHTLUnifiedScanner

class TestUnifiedScanner(unittest.TestCase):

    def test_script_block_separation(self):
        source = """
        div {
            text { "Some CHTL" }
            script {
                {{button}}->listen {
                    click: () => { console.log("Clicked!"); }
                };
            }
        }
        """

        scanner = CHTLUnifiedScanner(source)
        modified_source, fragments = scanner.scan()

        # Check that a placeholder was created
        self.assertEqual(len(fragments), 1)
        placeholder = list(fragments.keys())[0]

        # Check that the source was modified
        self.assertIn(placeholder, modified_source)
        self.assertNotIn("{{button}}", modified_source)

        # Check that the fragment content is correct
        expected_fragment = """
                {{button}}->listen {
                    click: () => { console.log("Clicked!"); }
                };
        """.strip()
        self.assertEqual(fragments[placeholder], expected_fragment)

if __name__ == '__main__':
    unittest.main()
