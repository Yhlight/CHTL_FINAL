import sys
from antlr4 import *
from CHTL.generated.CHTLLexer import CHTLLexer
from CHTL.generated.CHTLParser import CHTLParser
from CHTL.ast_builder import AstBuilder
from CHTL.html_generator import HtmlGenerator

def main(argv):
    input_stream = FileStream(argv[1])
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    parse_tree = parser.document()

    ast_builder = AstBuilder()
    ast = ast_builder.visit(parse_tree)

    html_generator = HtmlGenerator()
    html_output = html_generator.visit(ast)

    print(html_output)

if __name__ == '__main__':
    main(sys.argv)
