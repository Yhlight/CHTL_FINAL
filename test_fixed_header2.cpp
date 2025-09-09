#include "include/CHTL/CHTLLexerFixed.h"
#include <iostream>

int main() {
    std::cout << "Testing IDENTIFIER: " << static_cast<int>(CHTL::TokenType::IDENTIFIER) << std::endl;
    std::cout << "Testing STRING: " << static_cast<int>(CHTL::TokenType::STRING) << std::endl;
    std::cout << "Testing LEFT_BRACE: " << static_cast<int>(CHTL::TokenType::LEFT_BRACE) << std::endl;
    return 0;
}