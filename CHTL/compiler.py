import sys
from antlr4 import *

# CHTL Compiler components
from CHTL.generated.CHTLLexer import CHTLLexer
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast_builder import AstBuilder
from CHTL.html_generator import HtmlGenerator
from CHTL.scanner.unified_scanner import UnifiedScanner
from CHTL.symbol_table import SymbolTable
from CHTL.template_expander import TemplateExpander
from CHTL.ast.nodes import ScriptNode, AstNode, DocumentNode

# CHTL JS Compiler components (new paths)
from CHTL.js.generated.CHTLJSLexer import CHTLJSLexer
from CHTL.js.generated.CHTLJSParser import CHTLJSParser
from CHTL.js.ast_builder import ChtlJsAstBuilder
from CHTL.js.js_generator import JavaScriptGenerator

def compile_chtl_js(source_text: str) -> str:
    input_stream = InputStream(source_text)
    lexer = CHTLJSLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLJSParser(stream)
    parse_tree = parser.program()
    ast_builder = ChtlJsAstBuilder()
    ast = ast_builder.visit(parse_tree)
    js_generator = JavaScriptGenerator()
    js_code = js_generator.visit(ast)
    return js_code

def process_scripts(node: AstNode, registry: dict, doc_node: DocumentNode):
    if isinstance(node, ScriptNode):
        script_content = registry.get(node.script_id, {}).get('content', '')
        if script_content:
            generated_js = compile_chtl_js(script_content)
            doc_node.scripts[node.script_id] = generated_js
    if hasattr(node, 'children'):
        for child in node.children:
            process_scripts(child, registry, doc_node)

def main(argv):
    if len(argv) < 2:
        print("Usage: python -m CHTL.compiler <filename>")
        return
    with open(argv[1], 'r') as f:
        source_text = f.read()
    scanner = UnifiedScanner()
    modified_source, registry = scanner.scan(source_text)
    input_stream = InputStream(modified_source)
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    parse_tree = parser.document()
    symbol_table = SymbolTable()
    ast_builder = AstBuilder(registry=registry, symbol_table=symbol_table)
    ast = ast_builder.visit(parse_tree)
    expander = TemplateExpander(symbol_table=symbol_table)
    expander.expand(ast)
    if isinstance(ast, DocumentNode):
        process_scripts(ast, registry, ast)
    html_generator = HtmlGenerator()
    html_output = html_generator.visit(ast)
    print(html_output)

if __name__ == '__main__':
    main(sys.argv)
