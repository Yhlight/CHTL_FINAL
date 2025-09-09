import argparse
import sys
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
from CHTL.CHTLContext.context import CompilationContext

def compile_chtl(source_code: str, use_default_structure: bool = False) -> str:
    """
    Runs the full CHTL compilation pipeline.
    """
    context = CompilationContext()
    lexer = Lexer(source_code)
    tokens = lexer.tokenize()
    parser = Parser(tokens, context)
    ast = parser.parse()
    transformer = ASTTransformer(ast, context)
    transformed_ast = transformer.transform()
    generator = HTMLGenerator(transformed_ast)
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

        html_output = compile_chtl(source_code, use_default_structure=args.default_struct)

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
