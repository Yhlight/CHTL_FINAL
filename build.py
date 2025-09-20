import os
import sys
import subprocess

def main():
    # 1. Determine which test to build
    if len(sys.argv) > 1:
        test_name = sys.argv[1]
    else:
        test_name = "FullPipelineTest"

    print(f"--- Building Test: {test_name} ---")

    test_dir = os.path.join("Test", test_name)
    if not os.path.isdir(test_dir):
        print(f"Error: Test directory '{test_dir}' not found.")
        sys.exit(1)

    # 2. Define source directories
    source_dirs = ["CHTL", "Util"]
    source_files = []

    # 3. Find all .cpp files
    for src_dir in source_dirs:
        for root, _, files in os.walk(src_dir):
            for file in files:
                if file.endswith(".cpp"):
                    source_files.append(os.path.join(root, file))

    # Add the test's main.cpp
    test_main_cpp = os.path.join(test_dir, "main.cpp")
    if not os.path.isfile(test_main_cpp):
        print(f"Error: main.cpp not found in '{test_dir}'")
        sys.exit(1)
    source_files.append(test_main_cpp)


    # 4. Construct g++ command
    output_executable = f"{test_name}_executable"
    compile_command = [
        "g++",
        "-std=c++17",
        "-I.",  # Include root directory for headers
        "-o",
        output_executable,
    ] + source_files

    print(f"Executing command:\n{' '.join(compile_command)}\n")

    # 5. Execute the command
    result = subprocess.run(compile_command, capture_output=True, text=True)

    # 6. Print output
    if result.returncode == 0:
        print("Compilation successful!")
        print(f"Executable '{output_executable}' created.")
    else:
        print("Compilation failed.")
        print("--- STDOUT ---")
        print(result.stdout)
        print("--- STDERR ---")
        print(result.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
