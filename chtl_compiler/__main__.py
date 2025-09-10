import argparse
import sys
from .compiler import ChtlCompiler

def main():
    """The main function for the CHTL CLI."""
    parser = argparse.ArgumentParser(description="Compile a .chtl file to HTML.")
    parser.add_argument("input_file", help="The path to the input .chtl file.")
    parser.add_argument("-o", "--output", help="The path to the output HTML file. If not provided, output will be printed to stdout.")

    args = parser.parse_args()

    try:
        with open(args.input_file, 'r') as f:
            chtl_code = f.read()
    except FileNotFoundError:
        print(f"Error: Input file not found at '{args.input_file}'", file=sys.stderr)
        sys.exit(1)

    compiler = ChtlCompiler()

    try:
        html_output = compiler.compile(chtl_code)
    except Exception as e:
        print(f"An error occurred during compilation: {e}", file=sys.stderr)
        sys.exit(1)

    if args.output:
        try:
            with open(args.output, 'w') as f:
                f.write(html_output)
            print(f"Successfully compiled '{args.input_file}' to '{args.output}'")
        except IOError as e:
            print(f"Error writing to output file '{args.output}': {e}", file=sys.stderr)
            sys.exit(1)
    else:
        print(html_output)

if __name__ == "__main__":
    main()
