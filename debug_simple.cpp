#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Debug Simple Test ===" << std::endl;
    
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
        
        // 手动设置tokens
        parser.tokens_ = tokens;
        parser.current_token_ = 0;
        
        std::cout << "Current token type: " << static_cast<int>(parser.current().type) << std::endl;
        std::cout << "Current token value: '" << parser.current().value << "'" << std::endl;
        std::cout << "Is at end: " << parser.isAtEnd() << std::endl;
        
        std::cout << "Calling parseStatement..." << std::endl;
        auto node = parser.parseStatement();
        std::cout << "parseStatement completed!" << std::endl;
        
        if (node) {
            std::cout << "Statement parsed successfully" << std::endl;
            std::cout << "Node type: " << static_cast<int>(node->getType()) << std::endl;
            std::cout << "Node name: " << node->getName() << std::endl;
        } else {
            std::cout << "Failed to parse statement" << std::endl;
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
    std::cout << "Debug Simple Test Complete!" << std::endl;
    
    return 0;
}