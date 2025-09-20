import os
import subprocess
import sys

def find_cpp_files(directory):
    cpp_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(".cpp"):
                cpp_files.append(os.path.join(root, file))
    return cpp_files

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 build_and_test.py <TestName>")
        print("Available tests: FullPipelineTest, ScannerTest")
        sys.exit(1)

    test_name = sys.argv[1]
    test_main_path = f"Test/{test_name}/main.cpp"

    if not os.path.exists(test_main_path):
        print(f"Error: Test file not found at {test_main_path}")
        sys.exit(1)

    compiler = "g++"
    std = "-std=c++17"
    output_exe = f"test_runner_{test_name}"

    source_dirs = ["CHTL", "Util"]
    source_files = []
    for d in source_dirs:
        source_files.extend(find_cpp_files(d))

    # Remove the main chtl compiler entrypoint
    source_files = [f for f in source_files if "CHTL/main.cpp" not in f]

    all_files_to_compile = source_files + [test_main_path]

    include_paths = ["-I."]

    compile_command = [compiler, std] + include_paths + all_files_to_compile + ["-o", output_exe, "-lstdc++fs"]

    print(f"--- Building Test: {test_name} ---")
    print(" ".join(compile_command))
    print("---------------------------------")

    try:
        # Build the test runner
        subprocess.run(compile_command, check=True, capture_output=True, text=True)
        print(f"Successfully built {output_exe}")

        # Run the test runner
        print(f"\n--- Running Test: {test_name} ---")
        result = subprocess.run([f"./{output_exe}"], check=True, capture_output=True, text=True)
        print("--- Test Output ---")
        print(result.stdout)
        if result.stderr:
            print("--- Test Stderr ---")
            print(result.stderr)
        print("-------------------")
        print("Test ran successfully!")

    except subprocess.CalledProcessError as e:
        print(f"An error occurred during build or test execution for {test_name}.")
        print(f"Return code: {e.returncode}")
        print("--- STDOUT ---")
        print(e.stdout)
        print("--- STDERR ---")
        print(e.stderr)
        exit(1)
    except FileNotFoundError:
        print(f"Error: '{compiler}' not found. Make sure it's installed and in your PATH.")
        exit(1)

if __name__ == "__main__":
    main()
