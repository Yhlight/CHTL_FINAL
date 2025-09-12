#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== CHTL规范严格合规性测试 ===" << std::endl;
    
    // 测试1: 基本语法 - 无修饰字面量
    std::cout << "\n1. 测试无修饰字面量:" << std::endl;
    std::string test1 = R"(
text
{
    这是一段文本
}

div
{
    class: welcome;
    text: 这是另一段文本;
}
)";
    
    CHTL::CHTLLexer lexer1(test1);
    auto tokens1 = lexer1.tokenize();
    CHTL::CHTLParser parser1;
    parser1.setTokens(tokens1);
    auto root1 = parser1.parse();
    
    if (parser1.hasErrors()) {
        std::cout << "❌ 无修饰字面量测试失败:" << std::endl;
        for (const auto& error : parser1.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 无修饰字面量测试通过" << std::endl;
    }
    
    // 测试2: 属性运算
    std::cout << "\n2. 测试属性运算:" << std::endl;
    std::string test2 = R"(
div
{
    style
    {
        width: 100px + 50px;
        height: 200px * 2;
        margin: 10px - 5px;
    }
}
)";
    
    CHTL::CHTLLexer lexer2(test2);
    auto tokens2 = lexer2.tokenize();
    CHTL::CHTLParser parser2;
    parser2.setTokens(tokens2);
    auto root2 = parser2.parse();
    
    if (parser2.hasErrors()) {
        std::cout << "❌ 属性运算测试失败:" << std::endl;
        for (const auto& error : parser2.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 属性运算测试通过" << std::endl;
    }
    
    // 测试3: 属性条件表达式
    std::cout << "\n3. 测试属性条件表达式:" << std::endl;
    std::string test3 = R"(
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: width > 50px ? red : blue;
    }
}
)";
    
    CHTL::CHTLLexer lexer3(test3);
    auto tokens3 = lexer3.tokenize();
    CHTL::CHTLParser parser3;
    parser3.setTokens(tokens3);
    auto root3 = parser3.parse();
    
    if (parser3.hasErrors()) {
        std::cout << "❌ 属性条件表达式测试失败:" << std::endl;
        for (const auto& error : parser3.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 属性条件表达式测试通过" << std::endl;
    }
    
    // 测试4: 模板组合继承
    std::cout << "\n4. 测试模板组合继承:" << std::endl;
    std::string test4 = R"(
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
}

[Template] @Style ButtonStyle
{
    background-color: yellow;
    @Style ThemeColor;
}
)";
    
    CHTL::CHTLLexer lexer4(test4);
    auto tokens4 = lexer4.tokenize();
    CHTL::CHTLParser parser4;
    parser4.setTokens(tokens4);
    auto root4 = parser4.parse();
    
    if (parser4.hasErrors()) {
        std::cout << "❌ 模板组合继承测试失败:" << std::endl;
        for (const auto& error : parser4.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 模板组合继承测试通过" << std::endl;
    }
    
    // 测试5: 自定义系统
    std::cout << "\n5. 测试自定义系统:" << std::endl;
    std::string test5 = R"(
[Custom] @Style ButtonStyle
{
    padding: 10px;
    border: 1px solid black;
}
)";
    
    CHTL::CHTLLexer lexer5(test5);
    auto tokens5 = lexer5.tokenize();
    CHTL::CHTLParser parser5;
    parser5.setTokens(tokens5);
    auto root5 = parser5.parse();
    
    if (parser5.hasErrors()) {
        std::cout << "❌ 自定义系统测试失败:" << std::endl;
        for (const auto& error : parser5.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 自定义系统测试通过" << std::endl;
    }
    
    return 0;
}