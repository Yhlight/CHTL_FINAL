#include <iostream>
#include <string>
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main() {
    std::cout << "Testing UnifiedScanner..." << std::endl;
    
    UnifiedScanner scanner;
    
    // 测试简单CHTL代码
    std::string test_code = "div { text { Hello } }";
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto fragments = scanner.scan(test_code);
    std::cout << "Found " << fragments.size() << " fragments" << std::endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        std::cout << "Fragment " << i << ": type=" << static_cast<int>(fragments[i].type) 
                  << ", length=" << fragments[i].content.length() << std::endl;
    }
    
    return 0;
}