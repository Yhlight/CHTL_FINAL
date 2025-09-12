#include <iostream>
#include <string>
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing simple complex CHTL..." << std::endl;
    
    CHTLParser parser;
    CHTLGenerator generator;
    
    // 测试包含模板的CHTL代码
    std::string test_code = R"(div {
    id: "container";
    class: "main-container";
    
    text {
        Welcome to CHTL!
    }
})";
    
    std::cout << "Testing code length: " << test_code.length() << std::endl;
    
    std::cout << "Parsing..." << std::endl;
    auto ast = parser.parse(test_code);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML: " << html << std::endl;
    }
    
    return 0;
}