#include "include/CHTL/CHTLLexer.h"
#include <iostream>

int main() {
    // Test basic enum access
    CHTL::TokenType type = CHTL::TokenType::IDENTIFIER;
    std::cout << "IDENTIFIER: " << static_cast<int>(type) << std::endl;
    
    // Test if we can access the enum at all
    std::cout << "Enum values:" << std::endl;
    std::cout << "  IDENTIFIER: " << static_cast<int>(CHTL::TokenType::IDENTIFIER) << std::endl;
    std::cout << "  STRING: " << static_cast<int>(CHTL::TokenType::STRING) << std::endl;
    std::cout << "  LEFT_BRACE: " << static_cast<int>(CHTL::TokenType::LEFT_BRACE) << std::endl;
    
    return 0;
}