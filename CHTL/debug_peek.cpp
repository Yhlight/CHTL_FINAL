#include <iostream>
#include <vector>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"

int main() {
    std::string source = "div\n{\n    id: test;\n}";
    
    std::cout << "=== 调试peek方法 ===" << std::endl;
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
    
    std::cout << "\n开始解析..." << std::endl;
    
    // 手动调试解析过程
    size_t currentToken = 0;
    while (currentToken < tokens.size()) {
        auto& token = tokens[currentToken];
        std::cout << "当前token: " << token.toString() << std::endl;
        
        if (currentToken + 1 < tokens.size()) {
            auto& nextToken = tokens[currentToken + 1];
            std::cout << "下一个token: " << nextToken.toString() << std::endl;
        } else {
            std::cout << "下一个token: EOF" << std::endl;
        }
        
        if (token.type == CHTL::CHTLTokenType::IDENTIFIER && 
            currentToken + 1 < tokens.size() && 
            tokens[currentToken + 1].type == CHTL::CHTLTokenType::COLON) {
            std::cout << "  -> 这是属性!" << std::endl;
        }
        
        currentToken++;
        
        if (currentToken > 10) break; // 防止无限循环
    }
    
    return 0;
}