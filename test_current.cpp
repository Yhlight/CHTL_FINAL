#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Current ===" << std::endl;
    
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
        
        std::cout << "Tokens size: " << parser.tokens_.size() << std::endl;
        std::cout << "Current token index: " << parser.current_token_ << std::endl;
        
        std::cout << "Calling current()..." << std::endl;
        auto current_token = parser.current();
        std::cout << "Current token type: " << static_cast<int>(current_token.type) << std::endl;
        std::cout << "Current token value: '" << current_token.value << "'" << std::endl;
        
        std::cout << "Calling isAtEnd()..." << std::endl;
        bool at_end = parser.isAtEnd();
        std::cout << "Is at end: " << at_end << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Test Current Complete!" << std::endl;
    
    return 0;
}