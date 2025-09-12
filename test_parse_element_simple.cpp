#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "=== Simple Parse Element Test ===" << std::endl;
    
    // 测试最简单的输入
    std::string source = "div";
    
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
    
    // 2. 语法分析 - 只测试parseElement
    std::cout << "2. Testing parseElement..." << std::endl;
    CHTLParser parser;
    
    // 设置tokens到parser
    parser.tokens_ = tokens;
    parser.current_token_ = 0;
    
    try {
        auto element = parser.parseElement();
        if (element) {
            std::cout << "Successfully parsed element: " << element->getName() << std::endl;
        } else {
            std::cout << "Failed to parse element" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Exception in parseElement: " << e.what() << std::endl;
    }
    
    return 0;
}