import unittest
from Scanner.CHTLUnifiedScanner import CHTLUnifiedScanner, FragmentType

class TestUnifiedScanner(unittest.TestCase):

    def test_simple_split(self):
        source = """
        div {
            text: "This is CHTL";
        }
        style {
            body {
                color: red;
            }
        }
        p {
            text: "More CHTL";
        }
        """
        scanner = CHTLUnifiedScanner(source)
        fragments = scanner.scan()

        self.assertEqual(len(fragments), 3)

        # Fragment 1: CHTL before the style block
        self.assertEqual(fragments[0].type, FragmentType.CHTL)
        self.assertIn("div", fragments[0].content)
        self.assertNotIn("style", fragments[0].content)

        # Fragment 2: The style block
        self.assertEqual(fragments[1].type, FragmentType.CSS)
        self.assertIn("style", fragments[1].content)
        self.assertIn("body", fragments[1].content)
        self.assertIn("color: red;", fragments[1].content)

        # Fragment 3: CHTL after the style block
        self.assertEqual(fragments[2].type, FragmentType.CHTL)
        self.assertIn("p", fragments[2].content)
        self.assertNotIn("style", fragments[2].content)

    def test_no_special_blocks(self):
        source = "div { p { text: 'hello'; } }"
        scanner = CHTLUnifiedScanner(source)
        fragments = scanner.scan()
        self.assertEqual(len(fragments), 1)
        self.assertEqual(fragments[0].type, FragmentType.CHTL)
        self.assertEqual(fragments[0].content.strip(), source)

    def test_nested_braces_in_style(self):
        source = """
        style {
            .class {
                &:hover {
                    color: blue;
                }
            }
        }
        """
        scanner = CHTLUnifiedScanner(source)
        fragments = scanner.scan()
        self.assertEqual(len(fragments), 1)
        self.assertEqual(fragments[0].type, FragmentType.CSS)
        self.assertIn("&:hover", fragments[0].content)

    def test_local_blocks_are_not_split(self):
        source = """
        body {
            div {
                style { color: red; }
            }
        }
        """
        scanner = CHTLUnifiedScanner(source)
        fragments = scanner.scan()
        # The entire content should be treated as a single CHTL fragment
        # because the style block is not at the top level.
        self.assertEqual(len(fragments), 1)
        self.assertEqual(fragments[0].type, FragmentType.CHTL)

if __name__ == '__main__':
    unittest.main()
