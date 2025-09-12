#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 代码生成测试 ===" << std::endl;
    
    // 测试代码生成
    std::string source = R"(
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
}

div
{
    id: container;
    class: main-container;
    
    style
    {
        @Style DefaultText;
        background: white;
        margin: 20px;
        
        .highlight
        {
            color: red;
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
)";
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    
    // 词法分析
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "\n词法分析完成，tokens数量: " << tokens.size() << std::endl;
    
    // 语法分析
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    auto root = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "解析错误:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
        return 1;
    }
    
    std::cout << "语法分析完成" << std::endl;
    
    // 代码生成
    CHTL::CHTLGenerator generator;
    std::string html = generator.generate(root);
    
    if (generator.hasErrors()) {
        std::cout << "生成错误:" << std::endl;
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
        return 1;
    }
    
    std::cout << "\n生成的HTML:" << std::endl;
    std::cout << html << std::endl;
    
    return 0;
}