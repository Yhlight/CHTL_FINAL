#include <iostream>
#include <string>
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "Testing CHTLParser..." << std::endl;
    
    CHTLParser parser;
    
    // 测试简单CHTL代码
    std::string test_code = "div { text { Hello } }";
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto ast = parser.parse(test_code);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "AST node type: " << static_cast<int>(ast->getType()) << std::endl;
        std::cout << "AST children count: " << ast->getChildren().size() << std::endl;
    }
    
    return 0;
}