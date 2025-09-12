#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing complex CHTL debug..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    CHTLGenerator generator;
    
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
    
    // 只测试前200个字符
    std::string short_code = test_code.substr(0, 200);
    std::cout << "Testing short code: " << short_code << std::endl;
    
    std::cout << "Lexing..." << std::endl;
    auto tokens = lexer.tokenize(short_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < std::min(tokens.size(), size_t(20)); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    std::cout << "Parsing..." << std::endl;
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML: " << html << std::endl;
    }
    
    return 0;
}