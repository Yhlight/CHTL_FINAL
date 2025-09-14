#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // The intended pipeline: Scanner -> Dispatcher
        // NOTE: The dispatcher is not fully implemented to handle chunks yet.
        // This sets up the structure for future development.
        CHTL::CHTLUnifiedScanner scanner(source);
        std::vector<CHTL::CodeChunk> chunks = scanner.scan();

        // CHTL::CompilerDispatcher dispatcher;
        // std::string html = dispatcher.compile(chunks); // This is the eventual goal

        // For now, we'll just acknowledge that the scan is complete.
        std::cout << "CHTL source file successfully scanned into " << chunks.size() << " chunks." << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
