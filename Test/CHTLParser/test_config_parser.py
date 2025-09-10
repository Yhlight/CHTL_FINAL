import unittest
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLNode.nodes import ConfigNode
from CHTL.CHTLLexer.keywords import TokenType

class TestConfigParser(unittest.TestCase):

    def test_parse_configuration_block(self):
        source = """
[Configuration] {
    style: "css",
    Template: "Component"
}

css {
    h1 {
        color: red;
    }
}
"""
        context = CompilationContext()
        # First, we need to parse the configuration.
        # In a real compiler, this would be a pre-pass.
        # For the test, we can manually set the config after a first parse
        # or create a dedicated pre-parser.

        # Let's simulate a pre-pass for simplicity in the test
        pre_lexer = Lexer(source, context)
        pre_tokens = pre_lexer.tokenize()
        pre_parser = Parser(pre_tokens, source, context)
        pre_parser.parse() # This will call set_keyword_config on the context

        # Now, the context should have the remapped keywords.
        # Let's re-tokenize with the updated context.
        lexer = Lexer(source, context)
        tokens = lexer.tokenize()

        # Check if 'css' is tokenized as STYLE
        style_token_found = False
        for token in tokens:
            if token.value == "css":
                self.assertEqual(token.type, TokenType.STYLE)
                style_token_found = True
                break
        self.assertTrue(style_token_found, "The keyword 'css' was not tokenized as TokenType.STYLE")

        # Check the ConfigNode in the AST
        parser = Parser(tokens, source, context)
        ast = parser.parse()
        config_node = ast.children[0]
        self.assertIsInstance(config_node, ConfigNode)
        expected_config = {
            "style": "css",
            "Template": "Component"
        }
        self.assertEqual(config_node.config, expected_config)


if __name__ == '__main__':
    unittest.main()
