#include <iostream>

enum class TestEnum {
    VALUE1,
    VALUE2,
    LESS_THAN,
    GREATER_THAN
};

int main() {
    std::cout << "LESS_THAN: " << static_cast<int>(TestEnum::LESS_THAN) << std::endl;
    return 0;
}