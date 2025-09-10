import argparse
import sys
from CHTL.CHTLParser.config_pre_parser import ConfigPreParser
from CHTL.CHTLContext.context import CompilationContext
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
# I am reverting to a simple pipeline for now to fix the core features.
# The Scanner/Dispatcher architecture will be re-introduced later.

def compile_chtl(source_code: str, source_file_path: str, use_default_structure: bool = False) -> str:
    """
    Runs the full CHTL compilation pipeline.
    """
    # Config pre-parsing is not part of this simplified pipeline yet.
    # We will create a default context.
    context = CompilationContext()

    # The lexer does not need the context in this simplified version.
    lexer = Lexer(source_code)
    tokens = lexer.tokenize()

    # The parser needs the source code to handle raw [Origin] blocks.
    parser = Parser(tokens, source_code, context)
    ast = parser.parse()

    # The transformer needs the file path to resolve modules.
    transformer = ASTTransformer(ast, context, current_file_path=source_file_path)
    transformed_ast = transformer.transform()

    # The generator needs the context to evaluate @Var templates.
    generator = HTMLGenerator(transformed_ast, context)
    html_output = generator.generate(use_default_structure=use_default_structure)

    return html_output

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
        "--default-struct",
        action="store_true",
        help="Generate a full HTML document structure (<html>, <head>, <body>)."
    )

    args = parser.parse_args()

    try:
        with open(args.input_file, 'r', encoding='utf-8') as f:
            source_code = f.read()

        html_output = compile_chtl(source_code, source_file_path=args.input_file, use_default_structure=args.default_struct)

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
