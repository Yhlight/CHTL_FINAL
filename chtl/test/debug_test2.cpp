#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLContext.h"

int main() {
    try {
        // 测试词法分析
        CHTL::CHTLLexer lexer("test/simple2.chtl");
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens found: " << tokens.size() << std::endl;
        
        // 测试语法分析
        CHTL::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "AST generated successfully" << std::endl;
            std::cout << ast->toTreeString() << std::endl;
            
            // 测试代码生成
            CHTL::CHTLContext context;
            CHTL::CHTLGenerator generator(ast, context);
            generator.generate("debug_output.html");
            
            std::cout << "Generated HTML:" << std::endl;
            std::cout << generator.getHTML() << std::endl;
        } else {
            std::cout << "Failed to generate AST" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}