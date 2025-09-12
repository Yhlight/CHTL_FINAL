#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "Testing step by step..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    
    // 读取复杂的CHTL文件
    std::ifstream file("examples/basic.chtl");
    if (!file.is_open()) {
        std::cerr << "Cannot open file" << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string test_code = buffer.str();
    file.close();
    
    std::cout << "Source code length: " << test_code.length() << std::endl;
    
    // 只测试前100个字符
    std::string short_code = test_code.substr(0, 100);
    std::cout << "Testing short code: " << short_code << std::endl;
    
    std::cout << "Lexing..." << std::endl;
    auto tokens = lexer.tokenize(short_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    std::cout << "Parsing..." << std::endl;
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    return 0;
}