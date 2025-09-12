#include <iostream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main() {
    std::cout << "Testing template debug..." << std::endl;
    
    CHTLLexer lexer;
    CHTLParser parser;
    parser.setDebugMode(true);
    
    // 测试模板CHTL代码
    std::string test_code = R"([Template] @Style ButtonStyle
{
    padding: 10px 20px;
    background-color: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
})";
    
    std::cout << "Testing code: " << test_code << std::endl;
    
    auto tokens = lexer.tokenize(test_code);
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    for (size_t i = 0; i < std::min(tokens.size(), size_t(20)); ++i) {
        std::cout << "Token " << i << ": type=" << static_cast<int>(tokens[i].type) 
                  << ", value='" << tokens[i].value << "'" << std::endl;
    }
    
    auto ast = parser.parse(tokens);
    std::cout << "Parsing completed, AST: " << (ast ? "success" : "failed") << std::endl;
    
    return 0;
}