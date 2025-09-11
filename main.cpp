#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::string source = readFile(argv[1]);

    // 1. Scan
    UnifiedScanner scanner(source);
    ScannedContent scanned_content = scanner.scan();

    // 2. Dispatch to compilers
    CompilerDispatcher dispatcher(scanned_content);
    CompilationResult compilation_result = dispatcher.dispatch();

    // 3. Merge the results
    CodeMerger merger(compilation_result);
    std::string final_html = merger.merge();

    // 4. Print final output
    std::cout << final_html << std::endl;

    return 0;
}
