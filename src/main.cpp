#include <iostream>
#include <vector>
#include <string>
#include "Util/FileSystem/FileSystem.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

void runFile(const std::string& path) {
    std::string content;
    try {
        content = CHTL::Util::FileSystem::readFile(path);
    } catch (const std::runtime_error& e) {
        std::cerr << "File Error: " << e.what() << std::endl;
        return;
    }

    try {
        // 1. Lexer
        CHTL::CHTLLexer lexer(content);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 2. Parser
        CHTL::CHTLParser parser(std::move(tokens));
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 3. Generator
        if (ast) {
            CHTL::CHTLGenerator generator;
            std::string html = generator.generate(ast.get());
            std::cout << html << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Usage: chtl_compiler [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        // Default to test.chtl if no file is provided
        runFile("test.chtl");
    }

    return 0;
}
