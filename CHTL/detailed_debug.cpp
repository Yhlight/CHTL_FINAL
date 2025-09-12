#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>

using namespace CHTL;

int main() {
    std::cout << "=== CHTL编译器详细调试 ===" << std::endl;
    
    // 测试源代码
    std::string source = R"(div
{
    id: test;
    class: container;
    
    text
    {
        Hello World
    }
    
    style
    {
        width: 100px;
        height: 200px;
        background-color: red;
    }
})";
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;
    
    try {
        // 1. 词法分析
        std::cout << "1. 词法分析..." << std::endl;
        CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (lexer.hasErrors()) {
            std::cout << "词法分析失败:" << std::endl;
            lexer.printErrors();
            return 1;
        }
        
        std::cout << "   成功! 生成了 " << tokens.size() << " 个tokens" << std::endl;
        std::cout << "   所有tokens:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            std::cout << "     " << i << ": " << tokens[i].toString() << std::endl;
        }
        std::cout << std::endl;
        
        // 2. 语法分析
        std::cout << "2. 语法分析..." << std::endl;
        CHTLParser parser(tokens);
        
        std::cout << "   开始解析..." << std::endl;
        auto ast = parser.parse();
        
        if (parser.hasErrors()) {
            std::cout << "语法分析失败:" << std::endl;
            parser.printErrors();
            return 1;
        }
        
        std::cout << "   成功! 生成了AST" << std::endl;
        std::cout << "   AST结构:" << std::endl;
        parser.printAST(ast);
        std::cout << std::endl;
        
        std::cout << "详细调试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}