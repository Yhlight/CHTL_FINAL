#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "基于C++语言实现的超文本语言" << std::endl;
    std::cout << "MIT开源协议" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: chtl <输入文件> [输出文件]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // 创建CHTL上下文
        CHTL::CHTLContext context;
        
        // 词法分析
        CHTL::CHTLLexer lexer(inputFile);
        auto tokens = lexer.tokenize();
        
        // 语法分析
        CHTL::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        // 代码生成
        CHTL::CHTLGenerator generator(ast, context);
        generator.generate(outputFile);
        
        std::cout << "编译成功: " << inputFile << " -> " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}