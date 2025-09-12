#include <iostream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"

int main() {
    std::string source = R"(div
{
    style
    {
        .box
        {
            width: 100px;
            height: 200px;
        }
        
        &:hover
        {
            background-color: red;
        }
    }
})";
    
    std::cout << "=== CSS选择器测试 ===" << std::endl;
    std::cout << "源代码:\n" << source << std::endl;
    
    // 词法分析
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(source);
    
    std::cout << "\nTokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    // 语法分析
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    
    auto ast = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "\n解析错误:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "\n解析成功!" << std::endl;
        std::cout << "AST: " << ast->toString() << std::endl;
    }
    
    return 0;
}