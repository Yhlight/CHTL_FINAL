import argparse
import sys
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
from CHTL.CHTLContext.context import CompilationContext
from Scanner.scanner import UnifiedScanner
from CompilerDispatcher.dispatcher import CompilerDispatcher
from CodeMerger.merger import CodeMerger

def compile_chtl(source_code: str, use_default_structure: bool = False) -> str:
    """
    Runs the full CHTL compilation pipeline on a CHTL code fragment.
    This function is now called by the CompilerDispatcher.
    """
    # If the fragment is empty (e.g., a file with only style/script blocks),
    # return an empty string to avoid parsing errors.
    if not source_code.strip():
        return ""

    context = CompilationContext()
    lexer = Lexer(source_code)
    tokens = lexer.tokenize()
    parser = Parser(tokens, context)
    ast = parser.parse()
    transformer = ASTTransformer(ast, context)
    transformed_ast = transformer.transform()

    # The generator should not be responsible for the full HTML structure anymore.
    # It should only generate the fragment for its part of the AST.
    # The CodeMerger will handle placing fragments into the final document.
    # However, the generator's `generate` method still has the `use_default_structure`
    # flag which creates the full `<html>...</html>` structure.
    # This is a slight architectural inconsistency.
    # For now, we will let the CHTL generator create the main structure if the flag is set,
    # and the merger will inject the other parts into it.
    generator = HTMLGenerator(transformed_ast)
    html_output = generator.generate(use_default_structure=use_default_structure)

    return html_output

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
        dispatcher = CompilerDispatcher(use_default_structure=args.default_struct)
        compiled_outputs = dispatcher.dispatch(chtl_fragment, other_fragments)

        # 3. Merge the compiled outputs
        merger = CodeMerger()
        final_html = merger.merge(compiled_outputs)

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
        # For debugging
        import traceback
        traceback.print_exc()
        sys.exit(1)

if __name__ == "__main__":
    main()
