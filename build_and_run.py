import os
import subprocess

def find_cpp_files(directories):
    cpp_files = []
    for directory in directories:
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(".cpp"):
                    cpp_files.append(os.path.join(root, file))
    return cpp_files

def compile_and_run():
    # Directories to search for .cpp files
    source_dirs = ["CHTL", "Util", "Test/FullPipelineTest"]

    # Find all .cpp files
    cpp_files = find_cpp_files(source_dirs)

    # Prepare the g++ command
    output_executable = "full_pipeline_test"
    compile_command = ["g++", "-std=c++17", "-o", output_executable] + cpp_files

    # Add include paths
    compile_command.insert(2, "-I.")

    print("--- Compiling Project ---")
    print("Command:", " ".join(compile_command))

    # Compile the project
    result = subprocess.run(compile_command, capture_output=True, text=True)

    if result.returncode != 0:
        print("--- Compilation Failed ---")
        print("Stdout:")
        print(result.stdout)
        print("Stderr:")
        print(result.stderr)
        return

    print("--- Compilation Successful ---")

    print("\n--- Running Test ---")
    # Run the compiled test
    run_command = ["./" + output_executable]
    run_result = subprocess.run(run_command, capture_output=True, text=True)

    print("--- Test Output ---")
    print(run_result.stdout)
    if run_result.stderr:
        print("--- Test Error ---")
        print(run_result.stderr)

if __name__ == "__main__":
    compile_and_run()
