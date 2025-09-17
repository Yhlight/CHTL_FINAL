#include <iostream>
#include <string>
#include <vector>
#include "Util/FileSystem/FileSystem.h"
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

// A helper function to get the output filename from an input filename.
// e.g., "example.chtl" -> "example.html"
std::string getOutputFilename(const std::string& inputFilename) {
    size_t dot_pos = inputFilename.find_last_of(".");
    if (dot_pos == std::string::npos) {
        return inputFilename + ".html";
    }
    return inputFilename.substr(0, dot_pos) + ".html";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = getOutputFilename(inputFile);

    // 1. Read the source file
    std::string source_code;
    if (!CHTL::FileSystem::ReadFile(inputFile, source_code)) {
        std::cerr << "Error: Could not read file '" << inputFile << "'" << std::endl;
        return 1;
    }

    // 2. Scan the source code to get fragments
    // Note: UnifiedScanner currently has a placeholder implementation
    CHTL::UnifiedScanner scanner(source_code);
    scanner.scan();
    const auto& fragments = scanner.getFragments();

    // 3. Dispatch fragments to their respective compilers
    CHTL::CompilerDispatcher dispatcher;
    std::string final_output = dispatcher.dispatch(fragments);

    // 4. Write the final output to a file
    if (!CHTL::FileSystem::WriteFile(outputFile, final_output)) {
        std::cerr << "Error: Could not write to output file '" << outputFile << "'" << std::endl;
        return 1;
    }

    std::cout << "Compilation successful! Output written to '" << outputFile << "'." << std::endl;

    return 0;
}
