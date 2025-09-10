import os
from lark import Lark
from .transformer import ChtlTransformer
from .generator import HtmlGenerator

class ChtlCompiler:
    def __init__(self):
        # Construct the path to the grammar file relative to this file
        grammar_path = os.path.join(os.path.dirname(__file__), 'chtl.lark')
        with open(grammar_path, 'r') as f:
            grammar = f.read()

        self.parser = Lark(grammar, start='start', parser='lalr', transformer=ChtlTransformer())
        self.generator = HtmlGenerator()

    def compile(self, chtl_code: str) -> str:
        """
        Compiles a string of CHTL code to an HTML string.
        """
        ast = self.parser.parse(chtl_code)
        html = self.generator.generate(ast)
        # A simple normalization for cleaner output
        return html.strip()

def compile_chtl(chtl_code: str) -> str:
    """
    A convenience function for a one-off compilation.
    """
    compiler = ChtlCompiler()
    return compiler.compile(chtl_code)
