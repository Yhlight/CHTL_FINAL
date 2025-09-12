#include <iostream>
#include <string>
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing generator attributes..." << std::endl;
    
    CHTLParser parser;
    CHTLGenerator generator;
    
    // 测试包含属性的CHTL代码
    std::string test_code = R"(div {
    id: "container";
    class: "main-container";
    
    text {
        Welcome to CHTL!
    }
})";
    
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto ast = parser.parse(test_code);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast && !ast->getChildren().empty()) {
        auto first_child = ast->getChildren()[0];
        std::cout << "First child attributes count: " << first_child->getAttributes().size() << std::endl;
        
        for (const auto& attr : first_child->getAttributes()) {
            std::cout << "Attribute: " << attr.first << " = ";
            if (std::holds_alternative<std::string>(attr.second)) {
                std::cout << std::get<std::string>(attr.second);
            } else if (std::holds_alternative<int>(attr.second)) {
                std::cout << std::get<int>(attr.second);
            } else if (std::holds_alternative<double>(attr.second)) {
                std::cout << std::get<double>(attr.second);
            }
            std::cout << std::endl;
        }
        
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML: " << html << std::endl;
    }
    
    return 0;
}