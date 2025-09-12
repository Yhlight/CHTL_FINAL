#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"

using namespace CHTL;

int main() {
    std::cout << "Testing peek method..." << std::endl;
    
    CHTLLexer lexer;
    std::string test_code = "[Template] @Style ButtonStyle";
    
    std::cout << "Source code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    return 0;
}