import sys
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLGenerator.generator import Generator

def main():
    """
    The main entry point for the CHTL compiler.
    Reads a CHTL file, compiles it, and prints the HTML output.
    """
    if len(sys.argv) != 2:
        print("Usage: python main.py <input_file.chtl>")
        sys.exit(1)

    input_file = sys.argv[1]

    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            source = f.read()
    except FileNotFoundError:
        print(f"Error: Input file not found at '{input_file}'")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}")
        sys.exit(1)

    try:
        # 1. Lexing
        lexer = Lexer(source)
        tokens = lexer.scan_tokens()

        # 2. Parsing
        parser = Parser(tokens)
        ast = parser.parse()

        # 3. Generation
        generator = Generator()
        html_output = generator.generate(ast)

        print(html_output)

    except Exception as e:
        print(f"An error occurred during compilation: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
