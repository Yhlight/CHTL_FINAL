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

    // Stages 1 & 2: Lexing and Parsing
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast = parser.parse();

    if (!ast) {
        std::cerr << "Parsing failed, no AST generated." << std::endl;
        return 1;
    }

    // Stage 3: Generation
    Generator generator;
    std::string html = generator.generateHTML(*ast);
    std::string css = generator.getCSS();

    std::cout << "\n--- Generated HTML ---\n" << std::endl;
    std::cout << html;
    std::cout << "----------------------" << std::endl;

    if (!css.empty()) {
        std::cout << "\n--- Generated CSS ---\n" << std::endl;
        std::cout << css;
        std::cout << "---------------------" << std::endl;
    }

    return 0;
}
