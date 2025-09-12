#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Consume ===" << std::endl;
    
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
        
        std::cout << "Calling consume..." << std::endl;
        auto token = parser.consume();
        std::cout << "Consumed token: " << static_cast<int>(token.type) << " '" << token.value << "'" << std::endl;
        
        std::cout << "Current token after consume: " << static_cast<int>(parser.current().type) << std::endl;
        std::cout << "Is at end after consume: " << parser.isAtEnd() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Test Consume Complete!" << std::endl;
    
    return 0;
}