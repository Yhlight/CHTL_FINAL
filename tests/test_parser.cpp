#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../src/CHTLJS/CHTLJSLexer/CHTLJSLexer.h"
#include "../src/CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "../src/CHTLJS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "ast_printer.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.cjjs>" << std::endl;
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

    try {
        // 1. Lex the source
        CHTLJS::CHTLJSLexer lexer(source);
        std::vector<CHTLJS::CHTLJSToken> tokens = lexer.scanTokens();

        // 2. Parse the tokens
        auto context = std::make_shared<CHTLJS::CHTLJSContext>();
        CHTLJS::CHTLJSParser parser(source, tokens, context);
        std::unique_ptr<CHTLJS::SequenceNode> ast = parser.parse();

        // 3. Generate JS from the AST
        if (ast) {
            CHTLJS::CHTLJSGenerator generator;
            std::string js_code = generator.generate(*ast);
            std::cout << "--- GENERATED JS ---" << std::endl;
            std::cout << js_code << std::endl;
        } else {
            std::cout << "Parsing failed, no AST produced." << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
