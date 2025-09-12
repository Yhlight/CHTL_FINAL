#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 调试减号token ===" << std::endl;
    
    // 测试简单的减号
    std::string source = "10px - 5px";
    
    std::cout << "源代码: " << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "\nTokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    return 0;
}