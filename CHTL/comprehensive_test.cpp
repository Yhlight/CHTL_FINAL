#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== CHTL综合功能测试 ===" << std::endl;
    
    // 测试包含多种CHTL功能的综合代码
    std::string source = R"(
// 这是一个综合测试文件
[Template] @Style DefaultText
{
    color: black;
    line-height: 1.6;
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
            padding: 10px 20px;
        }
    }
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
    
    @Element Button;
}

import html "header.html" as Header;
import css "styles.css";
import javascript "script.js" as Script;
)";
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    
    // 词法分析
    std::cout << "\n=== 词法分析 ===" << std::endl;
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Tokens数量: " << tokens.size() << std::endl;
    std::cout << "前10个Tokens:" << std::endl;
    for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); ++i) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    if (tokens.size() > 10) {
        std::cout << "  ... (省略 " << (tokens.size() - 10) << " 个tokens)" << std::endl;
    }
    
    // 语法分析
    std::cout << "\n=== 语法分析 ===" << std::endl;
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    
    auto root = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "解析错误 (" << parser.getErrors().size() << " 个):" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "解析成功!" << std::endl;
        if (root) {
            std::cout << "AST根节点: " << root->toString() << std::endl;
        }
    }
    
    if (!parser.getWarnings().empty()) {
        std::cout << "\n警告 (" << parser.getWarnings().size() << " 个):" << std::endl;
        for (const auto& warning : parser.getWarnings()) {
            std::cout << "  " << warning << std::endl;
        }
    }
    
    return 0;
}