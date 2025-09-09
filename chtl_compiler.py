import argparse
import sys
from Scanner.CHTLUnifiedScanner import CHTLUnifiedScanner
from CompilerDispatcher.dispatcher import CompilerDispatcher


def compile_chtl(source_code: str, use_default_structure: bool = True) -> str:
    """
    Runs the full CHTL compilation pipeline using the scanner and dispatcher.
    """
    scanner = CHTLUnifiedScanner(source_code)
    fragments = scanner.scan()
    dispatcher = CompilerDispatcher(fragments)
    dispatcher.dispatch()
    final_html = dispatcher.merge_outputs(use_default_structure=use_default_structure)
    return final_html

def main():
    """
    Main function for the command-line interface.
    """
    parser = argparse.ArgumentParser(
        description="Compile a CHTL file to HTML.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "input_file",
        help="The path to the input .chtl file."
    )
    parser.add_argument(
        "-o", "--output",
        help="The path to the output .html file.\nIf not provided, the output will be printed to the console."
    )
    parser.add_argument(
        "--no-struct",
        action="store_true",
        help="Generate only the HTML fragment, not a full document structure."
    )

    args = parser.parse_args()

    try:
        with open(args.input_file, 'r', encoding='utf-8') as f:
            source_code = f.read()

        html_output = compile_chtl(source_code, use_default_structure=not args.no_struct)

        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(html_output)
            print(f"Successfully compiled {args.input_file} to {args.output}")
        else:
            print(html_output)

    except FileNotFoundError:
        print(f"Error: Input file not found at '{args.input_file}'", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred during compilation: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
