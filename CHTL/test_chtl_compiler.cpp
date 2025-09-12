#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace CHTL;

void testLexer() {
    std::cout << "=== 测试CHTL词法分析器 ===" << std::endl;
    
    std::string source = R"(
// 这是一个测试文件
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
    
    CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    
    if (lexer.hasErrors()) {
        std::cout << "词法分析错误:" << std::endl;
        lexer.printErrors();
    } else {
        std::cout << "词法分析成功!" << std::endl;
        std::cout << "前10个tokens:" << std::endl;
        for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
            std::cout << "  " << tokens[i].toString() << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testParser() {
    std::cout << "=== 测试CHTL解析器 ===" << std::endl;
    
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
    
    CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.hasErrors()) {
        std::cout << "词法分析失败，无法继续解析" << std::endl;
        return;
    }
    
    CHTLParser parser(tokens);
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "解析错误:" << std::endl;
        parser.printErrors();
    } else {
        std::cout << "解析成功!" << std::endl;
        std::cout << "AST结构:" << std::endl;
        parser.printAST(ast);
    }
    
    std::cout << std::endl;
}

void testGenerator() {
    std::cout << "=== 测试CHTL生成器 ===" << std::endl;
    
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
    
    CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.hasErrors()) {
        std::cout << "词法分析失败，无法继续生成" << std::endl;
        return;
    }
    
    CHTLParser parser(tokens);
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "解析失败，无法继续生成" << std::endl;
        return;
    }
    
    CHTLGenerator generator;
    std::string html = generator.generate(ast);
    
    if (generator.hasErrors()) {
        std::cout << "生成错误:" << std::endl;
        generator.printErrors();
    } else {
        std::cout << "生成成功!" << std::endl;
        std::cout << "生成的HTML:" << std::endl;
        std::cout << html << std::endl;
    }
    
    std::cout << std::endl;
}

void testCompletePipeline() {
    std::cout << "=== 测试完整编译流程 ===" << std::endl;
    
    std::string source = R"(
html
{
    head
    {
        title
        {
            CHTL测试页面
        }
    }
    
    body
    {
        div
        {
            id: main;
            class: container;
            
            text
            {
                欢迎使用CHTL!
            }
            
            style
            {
                width: 100%;
                height: 100vh;
                background-color: #f0f0f0;
                display: flex;
                justify-content: center;
                align-items: center;
            }
        }
    }
}
)";
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;
    
    // 词法分析
    std::cout << "1. 词法分析..." << std::endl;
    CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    if (lexer.hasErrors()) {
        std::cout << "词法分析失败:" << std::endl;
        lexer.printErrors();
        return;
    }
    std::cout << "   成功! 生成了 " << tokens.size() << " 个tokens" << std::endl;
    
    // 语法分析
    std::cout << "2. 语法分析..." << std::endl;
    CHTLParser parser(tokens);
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "语法分析失败:" << std::endl;
        parser.printErrors();
        return;
    }
    std::cout << "   成功! 生成了AST" << std::endl;
    
    // 代码生成
    std::cout << "3. 代码生成..." << std::endl;
    CHTLGenerator generator;
    std::string html = generator.generate(ast);
    
    if (generator.hasErrors()) {
        std::cout << "代码生成失败:" << std::endl;
        generator.printErrors();
        return;
    }
    std::cout << "   成功! 生成了HTML" << std::endl;
    
    // 输出结果
    std::cout << std::endl;
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
}

int main() {
    std::cout << "CHTL编译器测试程序" << std::endl;
    std::cout << "==================" << std::endl;
    std::cout << std::endl;
    
    try {
        testLexer();
        testParser();
        testGenerator();
        testCompletePipeline();
        
        std::cout << "所有测试完成!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}