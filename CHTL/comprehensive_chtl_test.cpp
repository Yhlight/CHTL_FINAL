#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>

void testFeature(const std::string& name, const std::string& source) {
    std::cout << "\n=== 测试: " << name << " ===" << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    auto root = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "❌ 失败: " << name << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 成功: " << name << std::endl;
    }
}

int main() {
    std::cout << "=== CHTL规范全面实现检查 ===" << std::endl;
    
    // 1. 基本语法测试
    testFeature("基本元素和属性", R"(
div
{
    id: container;
    class: main;
    text: "Hello World";
}
)");
    
    // 2. 注释系统测试
    testFeature("注释系统", R"(
// 单行注释
/* 多行注释 */
-- 生成器注释
div
{
    text: "内容";
}
)");
    
    // 3. 无修饰字面量测试
    testFeature("无修饰字面量", R"(
text
{
    这是一段文本
}

div
{
    class: welcome;
    text: 这是另一段文本;
}
)");
    
    // 4. CE对等式测试
    testFeature("CE对等式", R"(
div
{
    id = container;
    class = main;
}
)");
    
    // 5. 局部样式块测试
    testFeature("局部样式块", R"(
div
{
    style
    {
        width: 100px;
        height: 200px;
        .highlight
        {
            color: red;
        }
        &:hover
        {
            background: #f0f0f0;
        }
    }
}
)");
    
    // 6. 属性运算测试
    testFeature("属性运算", R"(
div
{
    style
    {
        width: 100px + 50px;
        height: 200px * 2;
        margin: 10px - 5px;
    }
}
)");
    
    // 7. 属性条件表达式测试
    testFeature("属性条件表达式", R"(
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: width > 50px ? red : blue;
    }
}
)");
    
    // 8. 模板系统测试
    testFeature("模板系统", R"(
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
}

[Template] @Element Button
{
    button
    {
        class: btn;
        style
        {
            @Style DefaultText;
            background: blue;
        }
    }
}
)");
    
    // 9. 模板继承测试
    testFeature("模板继承", R"(
[Template] @Style BaseText
{
    font-family: Arial;
    font-size: 14px;
}

[Template] @Style HighlightText
{
    inherit BaseText;
    color: red;
    font-weight: bold;
}
)");
    
    // 10. 模板组合测试
    testFeature("模板组合", R"(
[Template] @Style ThemeColor
{
    color: rgba(255, 192, 203, 1);
}

[Template] @Style ButtonStyle
{
    background-color: yellow;
    @Style ThemeColor;
}
)");
    
    // 11. 自定义系统测试
    testFeature("自定义系统", R"(
[Custom] @Style ButtonStyle
{
    padding: 10px;
    border: 1px solid black;
}

[Custom] @Element Card
{
    div
    {
        class: card;
        style
        {
            @Style ButtonStyle;
            background: white;
        }
    }
}
)");
    
    // 12. 导入系统测试
    testFeature("导入系统", R"(
import html "header.html" as Header;
import css "styles.css";
import javascript "script.js" as Script;
import chtl "components.chtl" { Button, Card };
)");
    
    // 13. 变量组模板测试
    testFeature("变量组模板", R"(
[Template] @Var ThemeColor
{
    primaryColor: rgb(255, 192, 203);
    secondaryColor: rgb(253, 144, 162);
}

div
{
    style
    {
        color: ThemeColor(primaryColor);
        background: ThemeColor(secondaryColor);
    }
}
)");
    
    // 14. 链式条件表达式测试
    testFeature("链式条件表达式", R"(
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px ? red : blue,
                width > 100px ? green : yellow, 
                height < 100px ? purple : pink;
    }
}
)");
    
    // 15. 逻辑运算符测试
    testFeature("逻辑运算符", R"(
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: 
                width > 50px && width < 100px ? red,
                width > 100px || height < 50px ? green, 
                height < 100px ? purple : pink;
    }
}
)");
    
    // 16. 函数调用测试
    testFeature("函数调用", R"(
div
{
    style
    {
        color: rgba(255, 192, 203, 1);
        transform: translate(10px, 20px);
        box-shadow: 0 0 10px rgba(0, 0, 0, 0.5);
    }
}
)");
    
    // 17. 复杂混合测试
    testFeature("复杂混合功能", R"(
[Template] @Style BaseTheme
{
    font-family: Arial;
    font-size: 16px;
    color: black;
}

[Custom] @Style ButtonTheme
{
    @Style BaseTheme;
    background: blue;
    padding: 10px 20px;
    border: 1px solid darkblue;
}

div
{
    id: container;
    class: main-container;
    
    style
    {
        @Style ButtonTheme;
        width: 100px + 50px;
        height: 200px;
        background-color: width > 100px ? red : blue;
        
        .highlight
        {
            color: yellow;
            font-weight: bold;
        }
        
        &:hover
        {
            background: #f0f0f0;
        }
    }
    
    h1
    {
        text: "欢迎使用CHTL";
    }
    
    p
    {
        text: "这是一个功能强大的模板语言";
    }
}

import html "header.html" as Header;
import css "styles.css";
)");
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}