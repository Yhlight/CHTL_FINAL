import argparse
import sys
from CHTL.CHTLLexer.lexer import Lexer
from CHTL.CHTLParser.parser import Parser
from CHTL.CHTLTransformer.transformer import ASTTransformer
from CHTL.CHTLGenerator.generator import HTMLGenerator
from CHTL.CHTLContext.context import CompilationContext

def compile_chtl(source_code: str) -> str:
    """
    Runs the full CHTL compilation pipeline.
    """
    context = CompilationContext()

    # 1. Lexing
    lexer = Lexer(source_code)
    tokens = lexer.tokenize()

    # 2. Parsing
    parser = Parser(tokens, context)
    ast = parser.parse()

    # 3. Transformation (New Step)
    transformer = ASTTransformer(ast, context)
    transformed_ast = transformer.transform()

    # 4. Generation
    generator = HTMLGenerator(transformed_ast)
    html_output = generator.generate()

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

    args = parser.parse_args()

    try:
        with open(args.input_file, 'r', encoding='utf-8') as f:
            source_code = f.read()

        html_output = compile_chtl(source_code)

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
