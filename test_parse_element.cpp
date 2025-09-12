#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Parse Element ===" << std::endl;
    
    // 测试简单的CHTL代码
    std::string simple_code = "div { text { Hello } }";
    
    std::cout << "Source: " << simple_code << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    try {
        // 词法分析
        std::cout << "1. Lexical Analysis..." << std::endl;
        CHTLLexer lexer;
        auto tokens = lexer.tokenize(simple_code);
        std::cout << "Found " << tokens.size() << " tokens" << std::endl;
        
        // 语法分析
        std::cout << "2. Syntax Analysis..." << std::endl;
        CHTLParser parser;
        
        // 手动设置tokens
        parser.tokens_ = tokens;
        parser.current_token_ = 0;
        
        std::cout << "Current token: " << static_cast<int>(parser.current().type) << " '" << parser.current().value << "'" << std::endl;
        std::cout << "Is at end: " << parser.isAtEnd() << std::endl;
        
        std::cout << "Calling parseElement..." << std::endl;
        auto element = parser.parseElement();
        std::cout << "parseElement completed!" << std::endl;
        
        if (element) {
            std::cout << "Element parsed successfully" << std::endl;
            std::cout << "Element type: " << static_cast<int>(element->getType()) << std::endl;
            std::cout << "Element name: " << element->getName() << std::endl;
        } else {
            std::cout << "Failed to parse element" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Test Parse Element Complete!" << std::endl;
    
    return 0;
}