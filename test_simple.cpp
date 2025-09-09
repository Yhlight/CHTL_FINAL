#include "test_simple_header.h"
#include <iostream>

int main() {
    std::cout << "LESS_THAN: " << static_cast<int>(CHTL::TokenType::LESS_THAN) << std::endl;
    return 0;
}