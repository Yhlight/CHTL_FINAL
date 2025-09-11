import sys
from antlr4 import *
from CHTL.generated.CHTLLexer import CHTLLexer
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast_builder import AstBuilder
from CHTL.html_generator import HtmlGenerator
from CHTL.scanner.unified_scanner import UnifiedScanner
import json

def main(argv):
    if len(argv) < 2:
        print("Usage: python -m CHTL.compiler <filename>")
        return

    with open(argv[1], 'r') as f:
        source_text = f.read()

    # --- Phase 1: Unified Scanner ---
    scanner = UnifiedScanner()
    modified_source, registry = scanner.scan(source_text)

    # --- Phase 2: ANTLR Parsing (on modified source) ---
    input_stream = InputStream(modified_source)
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    parse_tree = parser.document()

    # --- Phase 3: AST Building ---
    # Pass the registry to the builder so it can look up style block data
    ast_builder = AstBuilder(registry=registry)
    ast = ast_builder.visit(parse_tree)

    # --- Phase 4: HTML Generation ---
    html_generator = HtmlGenerator()
    html_output = html_generator.visit(ast)

    print(html_output)


if __name__ == '__main__':
    main(sys.argv)
