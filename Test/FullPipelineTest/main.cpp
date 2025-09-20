#include "../../CHTL/CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::ifstream file("Test/unified_scanner_test.chtl");
    if (!file.is_open()) {
        std::cerr << "Failed to open test file: Test/unified_scanner_test.chtl" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Running Full Pipeline Test ---" << std::endl;

    try {
        CHTL::CompilerDispatcher dispatcher(source);
        CHTL::FinalOutput compiled_output = dispatcher.dispatch();

        std::cout << "\n--- Final HTML Output ---\n" << compiled_output.html << "\n-------------------------\n";
        std::cout << "\n--- Final CSS Output ---\n" << compiled_output.css << "\n------------------------\n";
        std::cout << "\n--- Final JS Output ---\n" << compiled_output.js << "\n-----------------------\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Full Pipeline Test Finished ---" << std::endl;
    return 0;
}
