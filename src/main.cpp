#include <iostream>
#include <memory>
#include "Util/FileSystem/FileSystem.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLContext/CHTLContext.h"
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
        CHTLContext context;
        CHTLParser parser(tokens, source, &context);
        parser.parse();

        // 3. Generate HTML
        ElementNode* astRoot = parser.getRootNode();
        if (astRoot) {
            CHTLGenerator generator(&context); // Pass context to generator
            std::string html = generator.generate(*astRoot);
            std::cout << html;
        } else {
            std::cout << "Failed to parse the source: No root element found." << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
