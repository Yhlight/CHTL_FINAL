#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Function to read a whole file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to write a string to a file
void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + path);
    }
    file << content;
}

// Function to derive output filename from input filename
std::string getOutputFilename(const std::string& inputPath) {
    size_t dotPos = inputPath.rfind(".chtl");
    if (dotPos != std::string::npos && inputPath.substr(dotPos) == ".chtl") {
        return inputPath.substr(0, dotPos) + ".html";
    }
    // If it doesn't end with .chtl, just append .html
    return inputPath + ".html";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chtlcompiler <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = getOutputFilename(inputPath);

    std::cout << "Compiling " << inputPath << " to " << outputPath << "..." << std::endl;

    try {
        // 1. Read source file
        std::string source = readFile(inputPath);

        // 2. Lexing
        Lexer lexer(source);

        // 3. Parsing
        Parser parser(lexer);
        std::shared_ptr<RootNode> ast = parser.parse();

        // 4. Generation
        Generator generator;
        std::string htmlOutput = generator.generate(ast);

        // 5. Write to output file
        writeFile(outputPath, htmlOutput);

        std::cout << "Compilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
