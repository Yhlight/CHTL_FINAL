import sys
from antlr4 import *

# CHTL Compiler components
from .generated.CHTLLexer import CHTLLexer
from .generated.CHTLParser import CHTLParser
from .ast_builder import AstBuilder
from .html_generator import HtmlGenerator
from .scanner.unified_scanner import UnifiedScanner
from .symbol_table import SymbolTable
from .template_expander import TemplateExpander
from .ast.nodes import ScriptNode, AstNode, DocumentNode

# CHTL JS compilation is now handled by the CHTLJS_Compiler class,
# which is invoked by the HtmlGenerator. These old functions are obsolete.

def compile_single_file(filepath: str, symbol_table: SymbolTable, registry_override: dict = None):
    with open(filepath, 'r') as f:
        source_text = f.read()

    scanner = UnifiedScanner()
    modified_source, registry = scanner.scan(source_text)

    if registry_override is not None:
        registry.update(registry_override)

    input_stream = InputStream(modified_source)
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    parse_tree = parser.document()

    ast_builder = AstBuilder(registry=registry, symbol_table=symbol_table)
    ast = ast_builder.visit(parse_tree)

    return ast, registry

# This file contains the core compilation logic for a single CHTL file.
# The project-level compilation is handled by the CompilerDriver.
