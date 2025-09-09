#include "include/CHTL/CHTLLexer.h"
#include <iostream>

int main() {
    // Test if we can access the enum
    CHTL::TokenType type = CHTL::TokenType::IDENTIFIER;
    std::cout << "IDENTIFIER: " << static_cast<int>(type) << std::endl;
    
    // Test if we can access LESS_THAN
    try {
        CHTL::TokenType lessThan = CHTL::TokenType::LESS_THAN;
        std::cout << "LESS_THAN: " << static_cast<int>(lessThan) << std::endl;
    } catch (...) {
        std::cout << "LESS_THAN not found" << std::endl;
    }
    
    return 0;
}