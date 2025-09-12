#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 链式条件表达式测试 ===" << std::endl;
    
    std::string source = R"(
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