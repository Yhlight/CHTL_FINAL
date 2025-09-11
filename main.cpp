#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

std::string read_file_content(const std::string& filepath) {
    std::ifstream file_stream(filepath);
    if (!file_stream) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string filepath = "test.chtl"; // Default file
    if (argc > 1) {
        filepath = argv[1];
    }

    std::cout << "Compiling file: " << filepath << std::endl;

    std::string source_code = read_file_content(filepath);
    if (source_code.empty()) {
        return 1;
    }

    CHTLUnifiedScanner scanner;
    auto fragments = scanner.scan(source_code);

    std::cout << "\n>>> Scanner produced " << fragments.size() << " fragments. <<<\n" << std::endl;

    CompilerDispatcher dispatcher;
    dispatcher.dispatch(fragments);

    std::cout << "\nCompilation process finished." << std::endl;

    return 0;
}
