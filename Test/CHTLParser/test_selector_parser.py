import unittest
from CHTL.CHTLParser.selector_parser import SelectorParser
from CHTL.CHTLNode.nodes import SelectorType

class TestSelectorParser(unittest.TestCase):

    def test_simple_tag_selector(self):
        parser = SelectorParser("div")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 1)
        self.assertEqual(ast.parts[0].type, SelectorType.TAG)
        self.assertEqual(ast.parts[0].value, "div")
        self.assertIsNone(ast.parts[0].index)

    def test_simple_class_selector(self):
        parser = SelectorParser(".box")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 1)
        self.assertEqual(ast.parts[0].type, SelectorType.CLASS)
        self.assertEqual(ast.parts[0].value, "box")

    def test_simple_id_selector(self):
        parser = SelectorParser("#main")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 1)
        self.assertEqual(ast.parts[0].type, SelectorType.ID)
        self.assertEqual(ast.parts[0].value, "main")

    def test_descendant_selector(self):
        parser = SelectorParser(".container div")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 2)
        self.assertEqual(ast.parts[0].type, SelectorType.CLASS)
        self.assertEqual(ast.parts[0].value, "container")
        self.assertEqual(ast.parts[1].type, SelectorType.TAG)
        self.assertEqual(ast.parts[1].value, "div")

    def test_indexed_selector(self):
        parser = SelectorParser("p[2]")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 1)
        self.assertEqual(ast.parts[0].type, SelectorType.TAG)
        self.assertEqual(ast.parts[0].value, "p")
        self.assertEqual(ast.parts[0].index, 2)

    def test_complex_selector(self):
        parser = SelectorParser("#app .content p[0]")
        ast = parser.parse()
        self.assertEqual(len(ast.parts), 3)

        self.assertEqual(ast.parts[0].type, SelectorType.ID)
        self.assertEqual(ast.parts[0].value, "app")

        self.assertEqual(ast.parts[1].type, SelectorType.CLASS)
        self.assertEqual(ast.parts[1].value, "content")

        self.assertEqual(ast.parts[2].type, SelectorType.TAG)
        self.assertEqual(ast.parts[2].value, "p")
        self.assertEqual(ast.parts[2].index, 0)

if __name__ == '__main__':
    unittest.main()
