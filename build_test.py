import os
import subprocess
import sys

def find_cpp_files(directories):
    cpp_files = []
    for directory in directories:
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(".cpp"):
                    cpp_files.append(os.path.join(root, file))
    return cpp_files

def compile_test(test_name):
    print(f"--- Compiling test: {test_name} ---")

    source_dirs = ["CHTL", "Scanner", "Util", "CHTL JS"]

    cpp_files = find_cpp_files(source_dirs)

    test_main_file = f"Test/{test_name}/main.cpp"
    if not os.path.exists(test_main_file):
        print(f"Error: Test file not found at {test_main_file}")
        return False

    cpp_files.append(test_main_file)

    output_executable = f"build/{test_name.lower()}_test"

    command = [
        "g++",
        "-std=c++17",
        "-I.",
        "-o",
        output_executable,
    ] + cpp_files

    result = subprocess.run(command, capture_output=True, text=True)

    if result.returncode != 0:
        print("Compilation failed!")
        print("stdout:")
        print(result.stdout)
        print("stderr:")
        print(result.stderr)
        return False

    print(f"Compilation successful. Executable created at: {output_executable}")
    return True

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python build_test.py <TestName>")
        print("Example: python build_test.py IfBlockTest")
        sys.exit(1)

    test_to_build = sys.argv[1]

    if compile_test(test_to_build):
        print(f"\\n--- Running test: {test_to_build} ---")
        result = subprocess.run([f"./build/{test_to_build.lower()}_test"], capture_output=True, text=True)
        print(result.stdout, end='')
        print(result.stderr, end='')
        if result.returncode != 0:
            print("--- Test FAILED ---")
        else:
            print("--- Test PASSED ---")
