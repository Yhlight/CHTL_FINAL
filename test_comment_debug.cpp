#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "Testing comment debug..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    parser.setDebugMode(true);
    
    // 测试包含注释的CHTL代码
    std::string test_code = R"(// 基本CHTL示例
div
{
    id: "container";
    class: "main-container";
    
    text
    {
        Welcome to CHTL!
    }
})";
    
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < std::min(tokens.size(), size_t(15)); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast && !ast->getChildren().empty()) {
        auto first_child = ast->getChildren()[0];
        std::cout << "First child name: " << first_child->getName() << std::endl;
        std::cout << "First child attributes count: " << first_child->getAttributes().size() << std::endl;
    }
    
    return 0;
}