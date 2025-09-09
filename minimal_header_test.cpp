#include <iostream>

// Test if we can include the header and access basic enums
#include "include/CHTL/CHTLLexer.h"

int main() {
    // Test basic enum access
    CHTL::TokenType type = CHTL::TokenType::IDENTIFIER;
    std::cout << "IDENTIFIER: " << static_cast<int>(type) << std::endl;
    
    return 0;
}