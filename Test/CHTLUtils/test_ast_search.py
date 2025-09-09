import unittest
from CHTL.CHTLNode.nodes import DocumentNode, ElementNode, AttributeNode, SelectorNode
from CHTL.CHTLParser.selector_parser import SelectorParser
from CHTL.CHTLUtils.ast_search import find_nodes_by_selector

class TestAstSearch(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        # Build a standard mock AST for all tests to use
        # <div id="app">
        #   <p class="content"></p>
        #   <div class="content">
        #     <span></span>
        #   </div>
        # </div>
        cls.doc = DocumentNode()
        div_app = ElementNode(tag_name='div', attributes=[AttributeNode(name='id', value='app')])
        p_content = ElementNode(tag_name='p', attributes=[AttributeNode(name='class', value='content')])
        div_content = ElementNode(tag_name='div', attributes=[AttributeNode(name='class', value='content')])
        span_inner = ElementNode(tag_name='span')

        div_content.add_child(span_inner)
        div_app.add_child(p_content)
        div_app.add_child(div_content)
        cls.doc.add_child(div_app)

    def _find(self, selector_str: str) -> list:
        """Helper to parse a selector and find nodes in the mock doc."""
        parser = SelectorParser(selector_str)
        selector_ast = parser.parse()
        return find_nodes_by_selector(self.doc, selector_ast)

    def test_find_by_id(self):
        nodes = self._find("#app")
        self.assertEqual(len(nodes), 1)
        self.assertEqual(nodes[0].tag_name, 'div')

    def test_find_by_class(self):
        nodes = self._find(".content")
        self.assertEqual(len(nodes), 2)
        self.assertEqual(nodes[0].tag_name, 'p')
        self.assertEqual(nodes[1].tag_name, 'div')

    def test_find_by_tag(self):
        nodes = self._find("span")
        self.assertEqual(len(nodes), 1)
        self.assertEqual(nodes[0].tag_name, 'span')

    def test_find_descendant(self):
        nodes = self._find("#app span")
        self.assertEqual(len(nodes), 1)
        self.assertEqual(nodes[0].tag_name, 'span')

        nodes_div = self._find("#app div")
        self.assertEqual(len(nodes_div), 1) # Only finds descendants, not the #app div itself
        self.assertEqual(nodes_div[0].attributes[0].value, 'content')

    def test_find_with_index(self):
        # There are two '.content' elements, a <p> and a <div>
        nodes = self._find(".content[1]")
        self.assertEqual(len(nodes), 1)
        self.assertEqual(nodes[0].tag_name, 'div') # The second one is the div

    def test_no_match(self):
        nodes = self._find(".nonexistent")
        self.assertEqual(len(nodes), 0)

    def test_no_match_descendant(self):
        nodes = self._find("p span") # The span is not a descendant of the p
        self.assertEqual(len(nodes), 0)

if __name__ == '__main__':
    unittest.main()
