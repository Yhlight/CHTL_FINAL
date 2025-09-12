#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing complex CHTL..." << std::endl;
    
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
    
    std::cout << "Parsing..." << std::endl;
    auto ast = parser.parse(test_code);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML length: " << html.length() << std::endl;
        std::cout << "First 200 chars: " << html.substr(0, 200) << std::endl;
    }
    
    return 0;
}