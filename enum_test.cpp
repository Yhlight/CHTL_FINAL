#include <iostream>

// Test if we can define the enum directly
enum class TestTokenType {
    IDENTIFIER,
    STRING,
    LESS_THAN,
    GREATER_THAN
};

int main() {
    std::cout << "LESS_THAN: " << static_cast<int>(TestTokenType::LESS_THAN) << std::endl;
    return 0;
}