#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Setup compilation context
    CHTL::CHTLLoader loader;
    auto context = std::make_shared<CHTL::ParserContext>();

    // 2. Lex initial file
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 3. Parse all files (starting with the main one)
    CHTL::CHTLParser parser(tokens, loader, argv[1], context);
    std::unique_ptr<CHTL::RootNode> ast = parser.parse();

    // 3. Generator
    CHTL::CHTLGenerator generator;
    std::string html = generator.generate(*ast);

    std::cout << html << std::endl;

    return 0;
}
