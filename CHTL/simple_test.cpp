#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace CHTL;

int main() {
    std::cout << "=== CHTL编译器简单测试 ===" << std::endl;
    
    // 测试源代码
    std::string source = R"(
div
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
}
)";
    
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
        std::cout << "   前5个tokens:" << std::endl;
        for (size_t i = 0; i < std::min(tokens.size(), size_t(5)); ++i) {
            std::cout << "     " << tokens[i].toString() << std::endl;
        }
        std::cout << std::endl;
        
        // 2. 语法分析
        std::cout << "2. 语法分析..." << std::endl;
        CHTLParser parser(tokens);
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
        
        // 3. 代码生成
        std::cout << "3. 代码生成..." << std::endl;
        CHTLGenerator generator;
        std::string html = generator.generate(ast);
        
        if (generator.hasErrors()) {
            std::cout << "代码生成失败:" << std::endl;
            generator.printErrors();
            return 1;
        }
        
        std::cout << "   成功! 生成了HTML" << std::endl;
        std::cout << std::endl;
        
        // 输出结果
        std::cout << "生成的HTML:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << html << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        // 输出CSS
        std::string css = generator.generateCSS();
        if (!css.empty()) {
            std::cout << std::endl;
            std::cout << "生成的CSS:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << css << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
        
        // 输出JavaScript
        std::string js = generator.generateJavaScript();
        if (!js.empty()) {
            std::cout << std::endl;
            std::cout << "生成的JavaScript:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << js << std::endl;
            std::cout << "----------------------------------------" << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "CHTL编译器测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}