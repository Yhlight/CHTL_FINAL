#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 变量组模板测试 ===" << std::endl;
    
    std::string source = R"(
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
)";
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "\nTokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    auto root = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "\n解析错误:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "\n解析成功!" << std::endl;
    }
    
    return 0;
}