#include <iostream>

namespace CHTL {
    enum class TokenType {
        IDENTIFIER,
        STRING,
        LESS_THAN,
        GREATER_THAN
    };
}

int main() {
    std::cout << "LESS_THAN: " << static_cast<int>(CHTL::TokenType::LESS_THAN) << std::endl;
    return 0;
}