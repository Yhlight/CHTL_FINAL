import os
import subprocess
import sys

def main():
    test_dir = "Test"
    chtl_executable = "./chtl"

    # First, build the executable
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
        sys.exit(1)
    print("Build successful.")

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
            print(process.stdout)
            print(process.stderr)
            failures += 1
        else:
            print(f"PASSED: {test_file}")

    print("\n--- Test Summary ---")
    if failures == 0:
        print("All tests passed!")
        sys.exit(0)
    else:
        print(f"{failures} test(s) failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
