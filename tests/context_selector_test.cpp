#include "Compiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>

// Helper to read a file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Test Error: Could not open file " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    std::cout << "Running context selector (&) test..." << std::endl;

    // 1. Read the CHTL source file
    std::string source = readFile("tests/context_selector.chtl");

    // 2. Compile it
    std::string compiled_output;
    try {
        compiled_output = compile_source(source);
    } catch (const std::exception& e) {
        std::cerr << "Test Error: Compilation failed with exception: " << e.what() << std::endl;
        return 1;
    }

    // 3. Read the golden file
    std::string golden_output = readFile("tests/context_selector.html");

    // 4. Compare and assert
    if (compiled_output != golden_output) {
        std::cerr << "Test Failed: Output does not match golden file." << std::endl;
        std::cerr << "--- EXPECTED ---" << std::endl;
        std::cerr << golden_output << std::endl;
        std::cerr << "--- GOT ---" << std::endl;
        std::cerr << compiled_output << std::endl;
        std::cerr << "----------------" << std::endl;
        return 1; // Indicate failure
    }

    std::cout << "Context Selector Test Passed!" << std::endl;
    return 0; // Indicate success
}
