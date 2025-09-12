#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Lexer Only ===" << std::endl;
    
    // 测试简单的CHTL代码
    std::string simple_code = "div { text { Hello } }";
    
    std::cout << "Source: " << simple_code << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    try {
        // 词法分析
        std::cout << "Lexical Analysis..." << std::endl;
        CHTLLexer lexer;
        auto tokens = lexer.tokenize(simple_code);
        std::cout << "Found " << tokens.size() << " tokens" << std::endl;
        
        for (size_t i = 0; i < tokens.size(); i++) {
            const auto& token = tokens[i];
            std::cout << "  " << i << ": " << static_cast<int>(token.type) << " '" << token.value << "'" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Test Lexer Only Complete!" << std::endl;
    
    return 0;
}