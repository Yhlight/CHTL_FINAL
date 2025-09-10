import unittest
import sys
import os

# Add the project root to the Python path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import (
    ElementNode, TextNode, AttributeNode, CommentNode, StyleNode,
    CssRuleNode, CssPropertyNode, TemplateDefinitionNode, TemplateUsageNode,
    CustomDefinitionNode, CustomUsageNode, DeleteNode, InsertNode, OriginNode,
    ImportNode, NamespaceNode
)

class TestParser(unittest.TestCase):

    def _parse_source(self, source):
        lexer = Lexer(source)
        tokens = lexer.tokenize()
        context = CompilationContext()
        parser = Parser(tokens, context)
        ast = parser.parse()
        return ast, context

    def test_import_statement_parsing(self):
        source = '[Import] [Custom] @Element MyBox from "./components.chtl" as LocalBox'
        ast, context = self._parse_source(source)
        import_node = ast.children[0]
        self.assertIsInstance(import_node, ImportNode)
        self.assertEqual(import_node.import_type_parts, ['[Custom]', '@Element'])
        self.assertEqual(import_node.imported_item_name, 'MyBox')
        self.assertEqual(import_node.file_path, './components.chtl')
        self.assertEqual(import_node.alias, 'LocalBox')

    def test_namespace_block_parsing(self):
        source = '[Namespace] MyComponents { div {} }'
        ast, context = self._parse_source(source)
        namespace_node = ast.children[0]
        self.assertIsInstance(namespace_node, NamespaceNode)
        self.assertEqual(namespace_node.name, 'MyComponents')
        self.assertEqual(len(namespace_node.children), 1)
        self.assertIsInstance(namespace_node.children[0], ElementNode)

    def test_at_usage_with_namespace(self):
        source = 'div { @Element Box from space.room; }'
        ast, context = self._parse_source(source)
        div_node = ast.children[0]
        usage_node = div_node.children[0]
        self.assertIsInstance(usage_node, TemplateUsageNode)
        self.assertEqual(usage_node.name, 'Box')
        self.assertEqual(usage_node.namespace_from, ['space', 'room'])


if __name__ == '__main__':
    unittest.main()
