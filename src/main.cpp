#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    // 读取输入文件
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 创建编译上下文
    CHTLContext context;
    context.setInputFile(argv[1]);
    context.setDebugMode(true);
    
    // 创建词法分析器
    Lexer lexer(source);
    
    // 进行词法分析
    TokenList tokens = lexer.tokenize();
    
    // 输出词法分析结果
    std::cout << "=== CHTL Lexer Output ===" << std::endl;
    std::cout << "Total tokens: " << tokens.size() << std::endl;
    std::cout << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << token << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== CHTL Parser Output ===" << std::endl;
    
    // 创建语法分析器
    CHTLParser parser(context);
    
    // 进行语法分析
    auto ast = parser.parse(tokens);
    
    if (context.hasErrors()) {
        std::cout << "Parse errors:" << std::endl;
        for (const auto& error : context.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    if (!context.getWarnings().empty()) {
        std::cout << "Parse warnings:" << std::endl;
        for (const auto& warning : context.getWarnings()) {
            std::cout << "  " << warning << std::endl;
        }
    }
    
    if (ast) {
        std::cout << "AST generated successfully!" << std::endl;
        std::cout << "Generated HTML:" << std::endl;
        std::cout << ast->generate() << std::endl;
    } else {
        std::cout << "Failed to generate AST!" << std::endl;
        return 1;
    }
    
    return 0;
}