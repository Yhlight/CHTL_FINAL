#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 生成器注释测试 ===" << std::endl;
    
    std::string source = R"(
// 单行注释
/* 多行注释 */
-- 生成器注释
div
{
    text: "内容";
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