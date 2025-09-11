import sys
from antlr4 import *
from CHTL.generated.CHTLLexer import CHTLLexer
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast_builder import AstBuilder
from CHTL.html_generator import HtmlGenerator
from CHTL.scanner.unified_scanner import UnifiedScanner
from CHTL.symbol_table import SymbolTable
from CHTL.template_expander import TemplateExpander
import json

def main(argv):
    if len(argv) < 2:
        print("Usage: python -m CHTL.compiler <filename>")
        return

    with open(argv[1], 'r') as f:
        source_text = f.read()

    # --- Scanner ---
    scanner = UnifiedScanner()
    modified_source, registry = scanner.scan(source_text)

    # --- Parser ---
    input_stream = InputStream(modified_source)
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    parse_tree = parser.document()

    # --- Symbol Table & AST Building ---
    symbol_table = SymbolTable()
    ast_builder = AstBuilder(registry=registry, symbol_table=symbol_table)
    ast = ast_builder.visit(parse_tree)

    # --- Template Expansion ---
    expander = TemplateExpander(symbol_table=symbol_table)
    expander.expand(ast)

    # --- HTML Generation ---
    html_generator = HtmlGenerator()
    html_output = html_generator.visit(ast)

    print(html_output)


if __name__ == '__main__':
    main(sys.argv)
