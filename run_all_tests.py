import os
import subprocess
import sys

def build_compiler():
    print("--- Building CHTL compiler ---")
    build_command = [
        "g++", "-std=c++17", "-I.", "-o", "chtl",
        "CHTL/CHTLGenerator/CHTLGenerator.cpp",
        "CHTL/CompilerDispatcher/CompilerDispatcher.cpp",
        "CHTL/CHTLLexer/CHTLLexer.cpp",
        "CHTL/CHTLLexer/Token.cpp",
        "CHTL/CHTLParser/CHTLParser.cpp",
        "CHTL/Expression/ExpressionEvaluator.cpp",
        "CHTL/Scanner/CHTLUnifiedScanner.cpp",
        "CHTL/CodeMerger/CodeMerger.cpp",
        "CHTL/main.cpp",
        "CHTL/CHTLNode/ElementNode.cpp",
        "CHTL/CHTLNode/OriginNode.cpp",
        "CHTL/CHTLNode/StyleNode.cpp",
        "CHTL/CHTLNode/TextNode.cpp",
        "CHTL/CHTLNode/TemplateDefinitionNode.cpp",
        "CHTL/CHTLNode/CustomDeclarationNode.cpp",
        "CHTL/CHTLNode/ReactiveValueNode.cpp",
        "CHTL/CHTLNode/TemplateDeclarationNode.cpp",
        "CHTL/CHTLNode/ImportNode.cpp",
        "Util/FileSystem/FileSystem.cpp",
        "Util/StringUtil/StringUtil.cpp"
    ]
    build_process = subprocess.run(build_command, capture_output=True, text=True)
    if build_process.returncode != 0:
        print("Build failed!")
        print(build_process.stderr)
        return False
    print("Build successful.")
    return True

def run_single_test(test_file):
    print(f"--- Running single test: {test_file} ---")
    chtl_executable = "./chtl"
    process = subprocess.run([chtl_executable, test_file], capture_output=True, text=True)
    if process.returncode != 0:
        print(f"FAILED: {test_file}")
        print(process.stdout)
        print(process.stderr)
        return False
    else:
        print(f"PASSED: {test_file}")
        # Print the generated files
        try:
            with open("output.html", "r") as f:
                print("\n--- output.html ---")
                print(f.read())
            if os.path.exists("output.css"):
                with open("output.css", "r") as f:
                    print("\n--- output.css ---")
                    print(f.read())
        except FileNotFoundError:
            print("\n--- No output files generated ---")
        return True

def run_all_tests():
    test_dir = "Test"
    chtl_executable = "./chtl"
    test_files = []
    for root, _, files in os.walk(test_dir):
        for file in files:
            if file.endswith(".chtl"):
                test_files.append(os.path.join(root, file))

    failures = 0
    for test_file in test_files:
        print(f"--- Running test: {test_file} ---")
        process = subprocess.run([chtl_executable, test_file], capture_output=True, text=True)
        if process.returncode != 0:
            print(f"FAILED: {test_file}")
            # print(process.stdout) # Don't print stdout for failing tests in batch mode
            # print(process.stderr)
            failures += 1
        else:
            print(f"PASSED: {test_file}")

    print("\n--- Test Summary ---")
    if failures == 0:
        print("All tests passed!")
        return True
    else:
        print(f"{failures} test(s) failed.")
        return False

def main():
    if not build_compiler():
        sys.exit(1)

    if len(sys.argv) > 1:
        test_file = sys.argv[1]
        if not run_single_test(test_file):
            sys.exit(1)
    else:
        if not run_all_tests():
            sys.exit(1)

if __name__ == "__main__":
    main()
