#include "CHTL/CHTLPreprocessor/Preprocessor.h"
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file for writing: " + path);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: chtl_compiler <input_file.chtl> <output_file.html>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    try {
        // 1. Read source file
        std::string source = readFile(inputFile);

        // 2. Preprocess to handle imports
        Preprocessor preprocessor;
        std::string processedSource = preprocessor.process(source, inputFile);

        // 3. Scan the source into typed snippets
        UnifiedScanner scanner;
        std::vector<CodeSnippet> snippets = scanner.scan(processedSource);

        // 4. Dispatch snippets to appropriate compilers and merge results
        CompilerDispatcher dispatcher;
        std::string finalHtml = dispatcher.dispatch(snippets);

        // 5. Write to output file
        writeFile(outputFile, finalHtml);

        std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
