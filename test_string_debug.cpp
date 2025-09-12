#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"

using namespace CHTL;

int main() {
    std::cout << "Testing string parsing with debug..." << std::endl;
    
    CHTLLexer lexer;
    lexer.setDebugMode(true);
    
    // 测试字符串解析
    std::string test_code = R"(id: "container";)";
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    return 0;
}