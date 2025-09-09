#include <iostream>
#include "Util/FileSystem/FileSystem.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string source;

    try {
        source = FileSystem::readFile(filename);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return 1;
    }

    try {
        // 1. Lex
        CHTLLexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parse
        CHTLParser parser(tokens);
        std::unique_ptr<ElementNode> ast = parser.parse();

        // 3. Generate HTML
        if (ast) {
            CHTLGenerator generator;
            std::string html = generator.generate(*ast);
            std::cout << html;
        } else {
            std::cout << "Failed to parse the source." << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
