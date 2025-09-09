#include <iostream>
#include <sstream>
#include "Scanner/UnifiedScanner.h"

int main() {
    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    std::string input = buffer.str();

    auto slices = chtl::UnifiedScanner::scan(input);
    std::cout << "Slices: " << slices.size() << "\n";
    for (const auto& s : slices) {
        std::cout << static_cast<int>(s.type) << "\n" << s.content << "\n";
    }
    return 0;
}
