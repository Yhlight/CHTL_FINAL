#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"
#include "Token.h"

// Function to read a whole file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string chtlSource = readFile(sourcePath);

    // 1. Lexing
    Lexer lexer(chtlSource);
    std::vector<Token> tokens = lexer.scanTokens();

    // 2. Parsing
    std::unique_ptr<BaseNode> ast;
    try {
        Parser parser(tokens);
        ast = parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    // 3. Generating HTML
    Generator generator;
    std::string html = generator.generate(ast.get());

    // 4. Print the final output to standard output
    std::cout << html << std::endl;

    return 0;
}
