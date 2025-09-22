#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/BaseNode.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- CHTL Compiler ---" << std::endl;
    std::cout << "Compiling: " << filename << std::endl;
    std::cout << "---------------------" << std::endl;

    try {
        // 1. Lexer: Tokenize the source code
        CHTL::Lexer lexer(source);

        // 2. Parser: Build the Abstract Syntax Tree (AST)
        CHTL::Parser parser(lexer);
        std::vector<std::unique_ptr<CHTL::BaseNode>> ast = parser.ParseProgram();

        // 3. Generator: Convert the AST to an HTML string
        CHTL::Generator generator;
        std::string html_output = generator.Generate(ast);

        // 4. Print the final output
        std::cout << "\n--- Generated HTML ---" << std::endl;
        std::cout << html_output << std::endl;
        std::cout << "----------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n--- Compilation Error ---" << std::endl;
        std::cerr << e.what() << std::endl;
        std::cout << "-------------------------" << std::endl;
        return 1;
    }

    return 0;
}
