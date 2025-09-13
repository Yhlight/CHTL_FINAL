#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner/UnifiedScanner.h"
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

    // 1. Scan the source for fragments
    CHTL::UnifiedScanner scanner(source);
    CHTL::FragmentList fragments = scanner.scan();

    // 2. Dispatch fragments to appropriate compilers
    CHTL::CompilerDispatcher dispatcher;
    std::string final_output = dispatcher.compile(fragments, argv[1]);

    std::cout << final_output << std::endl;

    return 0;
}
