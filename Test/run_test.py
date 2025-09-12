import sys
import os
import json

# Add the project root to the Python path to allow for absolute imports
# from the 'CHTL' package. This is a common pattern for simple test runners.
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from CHTL.CHTLLexer import Lexer
from CHTL.CHTLParser import Parser
from CHTL.CHTLGenerator import Generator

def run_compiler_pipeline(filepath: str, debug=False):
    """
    Reads a .chtl file and runs it through the full Lexer -> Parser -> Generator pipeline.
    """
    print(f"--- Running Test: {filepath} ---")
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            source_code = f.read()

        # 1. Lexer
        lexer = Lexer(source_code)
        if debug:
            print("\n--- Tokens ---")
            # This consumes the lexer, so we must re-initialize it
            tokens = []
            while (tok := lexer.get_next_token()).type != TokenType.EOF:
                tokens.append(tok)
            print(tokens)
            lexer = Lexer(source_code) # Reset lexer

        # 2. Parser
        parser = Parser(lexer)
        ast = parser.parse_program()

        if parser.errors:
            print("\n!!! Parser Errors: !!!")
            for error in parser.errors:
                print(error)
            return None

        if debug:
            print("\n--- AST (JSON) ---")
            print(json.dumps(ast.to_dict(), indent=2))

        # 3. Generator
        generator = Generator()
        html_output = generator.generate(ast)

        return html_output

    except FileNotFoundError:
        print(f"Error: Test file not found at {filepath}")
        return None
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        # In case of an error, show the traceback for debugging
        import traceback
        traceback.print_exc()
        return None

if __name__ == "__main__":
    test_file = os.path.join(os.path.dirname(__file__), 'basic.chtl')

    # Run the pipeline
    output_html = run_compiler_pipeline(test_file, debug=False)

    if output_html is not None:
        print("\n--- Generated HTML ---")
        print(output_html)

        # Verification
        # Note: html.escape will turn " into &quot;
        expected_html = '<html><body><h1>Welcome to CHTL</h1><div></div><p>This is an unquoted test</p></body></html>'

        print("\n--- Verification ---")
        if output_html == expected_html:
            print("✅ Test Passed: Generated HTML matches the expected output.")
        else:
            print("❌ Test Failed: Generated HTML does not match the expected output.")
            print(f"Expected: {expected_html}")
            print(f"Got:      {output_html}")
