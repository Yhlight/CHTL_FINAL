#include "include/CHTL/CHTLLexerFixed.h"
#include <iostream>

int main() {
    std::cout << "Testing LESS_THAN: " << static_cast<int>(CHTL::TokenType::LESS_THAN) << std::endl;
    std::cout << "Testing GREATER_THAN: " << static_cast<int>(CHTL::TokenType::GREATER_THAN) << std::endl;
    std::cout << "Testing IDENTIFIER: " << static_cast<int>(CHTL::TokenType::IDENTIFIER) << std::endl;
    return 0;
}