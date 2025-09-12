#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "Testing debug simple..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    parser.setDebugMode(true);
    CHTLGenerator generator;
    
    // 测试简单的CHTL代码
    std::string test_code = R"(div {
    id: "test";
    text {
        Hello World
    }
})";
    
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    if (ast) {
        std::cout << "Generating HTML..." << std::endl;
        std::string html = generator.generateHTML(ast);
        std::cout << "Generated HTML: " << html << std::endl;
    }
    
    return 0;
}