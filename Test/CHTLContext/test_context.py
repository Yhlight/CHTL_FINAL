import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from CHTL.CHTLContext.context import CompilationContext

class TestCompilationContext(unittest.TestCase):

    def setUp(self):
        self.ctx = CompilationContext()

    def test_add_and_get_symbol_in_root(self):
        self.ctx.add_element_template("MyBox", ["div_node"])
        template = self.ctx.get_element_template("MyBox")
        self.assertEqual(template, ["div_node"])

    def test_push_pop_namespace(self):
        self.ctx.push_namespace("ns1")
        self.ctx.add_element_template("InnerBox", ["p_node"])
        self.ctx.pop_namespace()

        # Symbol should not be in root
        template = self.ctx.get_element_template("InnerBox")
        self.assertIsNone(template)

    def test_get_symbol_from_nested_namespace(self):
        self.ctx.push_namespace("ns1")
        self.ctx.push_namespace("ns2")
        self.ctx.add_element_template("DeepBox", ["span_node"])
        self.ctx.pop_namespace()
        self.ctx.pop_namespace()

        # Get with absolute path
        template = self.ctx.get_element_template("DeepBox", namespace_path=["ns1", "ns2"])
        self.assertEqual(template, ["span_node"])

    def test_get_symbol_from_parent_namespace(self):
        self.ctx.add_element_template("RootBox", ["root_div"])
        self.ctx.push_namespace("ns1")

        # Should find symbol in parent (root) scope
        template = self.ctx.get_element_template("RootBox")
        self.assertEqual(template, ["root_div"])

        self.ctx.pop_namespace()

    def test_symbol_shadowing(self):
        self.ctx.add_element_template("MySymbol", ["root_version"])
        self.ctx.push_namespace("ns1")
        self.ctx.add_element_template("MySymbol", ["ns1_version"])

        # Should get the version from the current (ns1) scope
        template = self.ctx.get_element_template("MySymbol")
        self.assertEqual(template, ["ns1_version"])

        # Pop scope and should get the root version
        self.ctx.pop_namespace()
        template = self.ctx.get_element_template("MySymbol")
        self.assertEqual(template, ["root_version"])

if __name__ == '__main__':
    unittest.main()
