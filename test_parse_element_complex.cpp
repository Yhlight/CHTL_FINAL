#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Complex Parse Element Test ===" << std::endl;
    
    // 测试更复杂的输入
    std::string source = "div { text { Hello } }";
    
    std::cout << "Source: " << source << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 1. 词法分析
    std::cout << "1. Lexical Analysis..." << std::endl;
    CHTLLexer lexer;
    auto tokens = lexer.tokenize(source);
    std::cout << "Found " << tokens.size() << " tokens" << std::endl;
    
    // 打印tokens
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": " << static_cast<int>(tokens[i].type) << " = '" << tokens[i].value << "'" << std::endl;
    }
    
    // 2. 语法分析 - 测试完整的parse方法
    std::cout << "2. Testing parse method..." << std::endl;
    CHTLParser parser;
    
    try {
        auto ast = parser.parse(tokens);
        if (ast) {
            std::cout << "Successfully parsed AST: " << ast->getName() << std::endl;
            std::cout << "Children count: " << ast->getChildren().size() << std::endl;
        } else {
            std::cout << "Failed to parse AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception in parse: " << e.what() << std::endl;
    }
    
    return 0;
}