import sys
import os

# Add the project directory to the path to allow CHTL package import
sys.path.append(os.path.abspath(os.path.dirname(__file__)))

from CHTL.driver import CompilerDriver

def main():
    """Main entry point for the CHTL compiler CLI."""
    if len(sys.argv) < 2:
        print("Usage: python chtl.py <input_file.chtl>")
        sys.exit(1)

    input_file = sys.argv[1]

    if not os.path.exists(input_file):
        print(f"Error: Input file not found at '{input_file}'")
        sys.exit(1)

    driver = CompilerDriver(entry_filepath=input_file)
    try:
        html_output = driver.compile_project()
        print(html_output)
    except Exception as e:
        print(f"An error occurred during compilation: {e}")
        # In a real compiler, add more detailed error reporting and traceback
        sys.exit(1)

if __name__ == "__main__":
    main()
