#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/Token.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    // 读取测试文件
    std::ifstream file("test.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open test.chtl" << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Source code:" << std::endl;
    std::cout << source << std::endl;
    std::cout << "\n" << std::string(50, '=') << std::endl;
    
    // 创建词法分析器
    CHTL::Lexer lexer(source);
    
    std::cout << "Tokens:" << std::endl;
    while (lexer.hasMoreTokens()) {
        auto token = lexer.nextToken();
        std::cout << token->toString() << std::endl;
        
        if (token->isType(CHTL::TokenType::END_OF_FILE)) {
            break;
        }
    }
    
    return 0;
}