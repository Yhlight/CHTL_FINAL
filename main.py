import sys
from lexer import Lexer
from parser import Parser
from generator import Generator

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 main.py <input_file.chtl>")
        sys.exit(1)

    filepath = sys.argv[1]
    try:
        with open(filepath, 'r') as f:
            source = f.read()
    except FileNotFoundError:
        print(f"Error: File not found at {filepath}")
        sys.exit(1)

    try:
        lexer = Lexer(source)
        parser = Parser(lexer)
        ast = parser.parse()
        generator = Generator()
        html = generator.generate(ast)
        print(html)

    except RuntimeError as e:
        print(f"Compilation Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
