#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Simple CHTL ===" << std::endl;
    
    // 测试简单的CHTL代码
    std::string simple_code = "div { text { Hello } }";
    
    std::cout << "Source: " << simple_code << std::endl;
    std::cout << std::string(40, '=') << std::endl;
    
    try {
        // 1. 词法分析
        std::cout << "1. Lexical Analysis..." << std::endl;
        CHTLLexer lexer;
        auto tokens = lexer.tokenize(simple_code);
        std::cout << "Found " << tokens.size() << " tokens" << std::endl;
        
        // 2. 语法分析
        std::cout << "2. Syntax Analysis..." << std::endl;
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast) {
            std::cout << "Parser completed successfully" << std::endl;
            std::cout << "AST type: " << static_cast<int>(ast->getType()) << std::endl;
            std::cout << "AST name: " << ast->getName() << std::endl;
            
            // 3. 代码生成
            std::cout << "3. Code Generation..." << std::endl;
            CHTLGenerator generator;
            std::string html = generator.generateHTML(ast);
            std::cout << "Generated HTML: " << html << std::endl;
        } else {
            std::cout << "Parser failed" << std::endl;
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "Error: " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(40, '=') << std::endl;
    std::cout << "Test Simple CHTL Complete!" << std::endl;
    
    return 0;
}