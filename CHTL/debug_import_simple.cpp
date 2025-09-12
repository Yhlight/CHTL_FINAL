#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== 简单导入调试 ===" << std::endl;
    
    // 测试简单的导入语法
    std::string source = "import html \"header.html\";";
    std::cout << "源代码: " << source << std::endl;
    
    // 词法分析
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "\nTokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    // 语法分析
    std::cout << "\n开始语法分析..." << std::endl;
    CHTL::CHTLParser parser;
    parser.setTokens(tokens);
    
    auto root = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "解析错误:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "解析成功!" << std::endl;
        if (root) {
            std::cout << "AST根节点: " << root->toString() << std::endl;
        }
    }
    
    return 0;
}