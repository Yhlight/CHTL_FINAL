import argparse
import sys
from Scanner.scanner import UnifiedScanner
from CompilerDispatcher.dispatcher import CompilerDispatcher
from CodeMerger.merger import CodeMerger

def main():
    """
    Main function for the command-line interface.
    Orchestrates the full scanning, dispatching, and merging pipeline.
    """
    parser = argparse.ArgumentParser(
        description="Compile a CHTL file to HTML.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("input_file", help="The path to the input .chtl file.")
    parser.add_argument("-o", "--output", help="The path to the output .html file.\nIf not provided, the output will be printed to the console.")
    parser.add_argument("--default-struct", action="store_true", help="Generate a full HTML document structure (<html>, <head>, <body>).")

    args = parser.parse_args()

    try:
        with open(args.input_file, 'r', encoding='utf-8') as f:
            source_code = f.read()

        # 1. Scan the source code to separate fragments
        scanner = UnifiedScanner(source_code)
        chtl_fragment, other_fragments = scanner.scan()

        # 2. Dispatch fragments to their respective compilers
        dispatcher = CompilerDispatcher()
        compiled_outputs = dispatcher.dispatch(chtl_fragment, other_fragments)

        # 3. Merge the compiled outputs, passing the structure flag here
        merger = CodeMerger()
        final_html = merger.merge(compiled_outputs, use_default_structure=args.default_struct)

        if args.output:
            with open(args.output, 'w', encoding='utf-8') as f:
                f.write(final_html)
            print(f"Successfully compiled {args.input_file} to {args.output}")
        else:
            print(final_html)

    except FileNotFoundError:
        print(f"Error: Input file not found at '{args.input_file}'", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred during compilation: {e}", file=sys.stderr)
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
