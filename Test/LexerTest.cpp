#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/AST.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    std::string filename = "test.chtl";
    std::string source;

    try {
        source = FileSystem::readFile(filename);
        std::cout << "Successfully read " << filename << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return 1;
    }

    // Stage 1: Lexing
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // Stage 2: Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    if (!ast) {
        std::cerr << "Parsing failed, no AST generated." << std::endl;
        return 1;
    }

    // Stage 3: Generation
    std::cout << "\n--- Generated HTML ---\n" << std::endl;
    Generator generator;
    std::string html = generator.generate(*ast);
    std::cout << html;
    std::cout << "\n----------------------" << std::endl;

    return 0;
}
