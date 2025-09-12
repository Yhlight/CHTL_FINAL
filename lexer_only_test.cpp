#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"

using namespace CHTL;

int main() {
    std::cout << "=== CHTL JS Lexer Only Test ===" << std::endl;
    
    // 测试简单的CHTL JS代码
    std::string simple_code = "listen { click: function() {} }";
    
    std::cout << "Source: " << simple_code << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    // 词法分析
    std::cout << "Lexical Analysis:" << std::endl;
    CHTLJSLexer lexer;
    auto tokens = lexer.tokenize(simple_code);
    
    std::cout << "Found " << tokens.size() << " tokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": " << static_cast<int>(token.type) << " '" << token.value << "'" << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Lexer Test Complete!" << std::endl;
    
    return 0;
}