#include <iostream>
#include <string>
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main() {
    std::cout << "=== Unified Scanner Debug Test ===" << std::endl;
    
    std::string source = "div { text { Hello } }";
    
    std::cout << "Source: " << source << std::endl;
    std::cout << "========================================" << std::endl;
    
    UnifiedScanner scanner;
    auto fragments = scanner.scan(source);
    
    std::cout << "Found " << fragments.size() << " fragments" << std::endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        std::cout << "Fragment " << i << ":" << std::endl;
        std::cout << "  Type: " << static_cast<int>(fragments[i].type) << std::endl;
        std::cout << "  Content: '" << fragments[i].content << "'" << std::endl;
        std::cout << "  Start: " << fragments[i].start_pos << std::endl;
        std::cout << "  End: " << fragments[i].end_pos << std::endl;
        std::cout << "  Placeholder: '" << fragments[i].placeholder << "'" << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}