#include <iostream>
#include <string>
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing CHTLGenerator..." << std::endl;
    
    CHTLParser parser;
    CHTLGenerator generator;
    
    // 测试简单CHTL代码
    std::string test_code = "div { text { Hello } }";
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto ast = parser.parse(test_code);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML: " << html << std::endl;
    }
    
    return 0;
}