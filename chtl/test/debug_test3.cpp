#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

int main() {
    try {
        // 测试词法分析
        CHTL::CHTLLexer lexer("test/simple2.chtl");
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens found: " << tokens.size() << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.toString() << std::endl;
        }
        
        // 测试语法分析
        CHTL::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "AST generated successfully" << std::endl;
            std::cout << ast->toTreeString() << std::endl;
            
            // 检查text节点的内容
            if (ast->getChildren().size() > 0) {
                auto div = ast->getChildren()[0];
                std::cout << "Div children: " << div->getChildren().size() << std::endl;
                if (div->getChildren().size() > 0) {
                    auto text = div->getChildren()[0];
                    std::cout << "Text node type: " << static_cast<int>(text->getType()) << std::endl;
                    std::cout << "Text content: '" << text->getText() << "'" << std::endl;
                }
            }
        } else {
            std::cout << "Failed to generate AST" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}