#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"

using namespace CHTL;

int main() {
    std::cout << "Testing CHTLLexer..." << std::endl;
    
    CHTLLexer lexer;
    
    // 测试简单CHTL代码
    std::string test_code = "div { text { Hello } }";
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    return 0;
}