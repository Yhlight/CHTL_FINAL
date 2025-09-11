import sys
from antlr4 import *
from CHTL.generated.CHTLLexer import CHTLLexer
from CHTL.generated.CHTLParser import CHTLParser

def main(argv):
    input_stream = FileStream(argv[1])
    lexer = CHTLLexer(input_stream)
    stream = CommonTokenStream(lexer)
    parser = CHTLParser(stream)
    tree = parser.document()
    print(tree.toStringTree(recog=parser))

if __name__ == '__main__':
    main(sys.argv)
