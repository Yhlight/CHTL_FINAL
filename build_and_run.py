import os
import sys
import subprocess

def find_cpp_files(dirs):
    cpp_files = []
    for d in dirs:
        for root, _, files in os.walk(d):
            for f in files:
                if f.endswith(".cpp"):
                    cpp_files.append(os.path.join(root, f))
    return cpp_files

def main():
    if len(sys.argv) != 2:
        print("Usage: python build_and_run.py <TestName>")
        print("Example: python build_and_run.py ConfigTest")
        sys.exit(1)

    test_name = sys.argv[1]
    test_name_snake_case = ''.join(['_' + i.lower() if i.isupper() else i for i in test_name]).lstrip('_')

    source_dirs = ["CHTL", "CHTL JS", "Util"]
    cpp_sources = find_cpp_files(source_dirs)

    test_main_cpp = f"Test/{test_name}/main.cpp"
    if not os.path.exists(test_main_cpp):
        print(f"Error: Test file not found at {test_main_cpp}")
        sys.exit(1)

    output_executable = f"build/{test_name_snake_case}"

    compile_command = [
        "g++",
        "-std=c++17",
        "-I.",
        "-o",
        output_executable,
        test_main_cpp
    ] + cpp_sources

    print(f"--- Compiling {test_name} ---")
    compile_result = subprocess.run(compile_command, capture_output=True, text=True)

    if compile_result.returncode != 0:
        print("Compilation failed!")
        print("Stdout:")
        print(compile_result.stdout)
        print("Stderr:")
        print(compile_result.stderr)
        sys.exit(1)

    print(f"--- Running {test_name} ---")
    run_result = subprocess.run([f"./{output_executable}"], capture_output=True, text=True)

    print(run_result.stdout)
    if run_result.returncode != 0:
        print(f"Test {test_name} FAILED with return code {run_result.returncode}")
        print("Stderr:")
        print(run_result.stderr)
        sys.exit(1)

    print(f"--- {test_name} PASSED ---")

if __name__ == "__main__":
    main()
