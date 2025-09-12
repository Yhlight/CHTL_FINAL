#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Debug Parser ===" << std::endl;
    
    // 测试最简单的代码
    std::string simple_code = "listen";
    
    std::cout << "Source: " << simple_code << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    // 词法分析
    std::cout << "1. Lexical Analysis:" << std::endl;
    CHTLJSLexer lexer;
    auto tokens = lexer.tokenize(simple_code);
    
    std::cout << "Found " << tokens.size() << " tokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": " << static_cast<int>(token.type) << " '" << token.value << "'" << std::endl;
    }
    std::cout << std::endl;
    
    // 语法分析
    std::cout << "2. Syntax Analysis:" << std::endl;
    CHTLJSParser parser;
    
    try {
        std::cout << "Starting parse..." << std::endl;
        auto ast = parser.parse(tokens);
        std::cout << "Parse completed!" << std::endl;
        
        if (ast) {
            std::cout << "AST created successfully" << std::endl;
            std::cout << "Root node type: " << static_cast<int>(ast->getType()) << std::endl;
            std::cout << "Root node name: " << ast->getName() << std::endl;
            std::cout << "Children count: " << ast->getChildren().size() << std::endl;
        } else {
            std::cout << "Failed to create AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    auto errors = parser.getErrors();
    if (!errors.empty()) {
        std::cout << "Parser errors:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Debug Parser Complete!" << std::endl;
    
    return 0;
}