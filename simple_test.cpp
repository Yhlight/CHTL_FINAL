#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer.h"

int main() {
    std::cout << "Simple CHTL Test" << std::endl;
    
    // 简单的测试代码
    std::string source_code = "div { text { Hello } }";
    
    std::cout << "Source code: " << source_code << std::endl;
    
    // 测试统一扫描器
    CHTL::UnifiedScanner scanner;
    auto fragments = scanner.scan(source_code);
    
    std::cout << "Fragments found: " << fragments.size() << std::endl;
    
    for (const auto& fragment : fragments) {
        std::cout << "Fragment type: " << static_cast<int>(fragment.type) << std::endl;
        std::cout << "Fragment content: " << fragment.content << std::endl;
    }
    
    // 测试词法分析器
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(source_code);
    
    std::cout << "Tokens found: " << tokens.size() << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.value << " (type: " << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}