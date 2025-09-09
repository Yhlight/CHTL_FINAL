#include "include/CHTL/CHTLLexerFixed.h"
#include <iostream>

int main() {
    std::cout << "Testing IDENTIFIER: " << static_cast<int>(CHTL::TokenType::IDENTIFIER) << std::endl;
    std::cout << "Testing STRING: " << static_cast<int>(CHTL::TokenType::STRING) << std::endl;
    std::cout << "Testing LEFT_BRACE: " << static_cast<int>(CHTL::TokenType::LEFT_BRACE) << std::endl;
    
    // Try to access LESS_THAN
    std::cout << "Testing LESS_THAN: " << static_cast<int>(CHTL::TokenType::LESS_THAN) << std::endl;
    
    return 0;
}