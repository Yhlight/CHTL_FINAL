#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "Testing attribute debug..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    parser.setDebugMode(true);
    
    // 测试包含属性的CHTL代码
    std::string test_code = R"(div {
    id: "container";
    class: "main-container";
    
    text {
        Welcome to CHTL!
    }
})";
    
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast && !ast->getChildren().empty()) {
        auto first_child = ast->getChildren()[0];
        std::cout << "First child attributes count: " << first_child->getAttributes().size() << std::endl;
        
        for (const auto& attr : first_child->getAttributes()) {
            std::cout << "Attribute: " << attr.first << " = ";
            if (std::holds_alternative<std::string>(attr.second)) {
                std::cout << std::get<std::string>(attr.second);
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}