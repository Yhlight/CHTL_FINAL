#include <iostream>
#include <fstream>
#include <string>
#include <CHTL/CHTLLexer/Lexer.hpp>
#include <CHTL/CHTLParser/Parser.hpp>
#include <CHTL/CHTLGenerator/CodeGenerator.hpp>

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [options] <input.chtl>" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  --default-struct    Generate HTML with default structure (DOCTYPE, html, head, body)" << std::endl;
        std::cerr << "  --help              Show this help message" << std::endl;
        return 1;
    }
    
    bool useDefaultStruct = false;
    std::string inputFile;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--default-struct") {
            useDefaultStruct = true;
        } else if (arg == "--help") {
            std::cerr << "CHTL Compiler v1.0.0" << std::endl;
            std::cerr << "Usage: " << argv[0] << " [options] <input.chtl>" << std::endl;
            std::cerr << "Options:" << std::endl;
            std::cerr << "  --default-struct    Generate HTML with default structure (DOCTYPE, html, head, body)" << std::endl;
            std::cerr << "  --help              Show this help message" << std::endl;
            std::cerr << std::endl;
            std::cerr << "By default, CHTL generates clean content suitable for SPA pages." << std::endl;
            std::cerr << "Use --default-struct to generate complete HTML documents." << std::endl;
            return 0;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        return 1;
    }
    
    std::ifstream file(inputFile);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << inputFile << std::endl;
        return 1;
    }
    
    // 读取文件内容
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 创建词法分析器
    Lexer lexer(source);
    
    // 创建语法分析器
    Parser parser(lexer);
    
    // 语法分析
    auto ast = parser.parse();
    
    // 输出结果
    std::cout << "=== CHTL 语法分析结果 ===" << std::endl;
    std::cout << "文件: " << inputFile << std::endl;
    
    if (parser.hasError()) {
        std::cout << "错误: " << parser.getError() << std::endl;
        return 1;
    }
    
    std::cout << "AST 根节点: " << ast->toString() << std::endl;
    std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
    std::cout << std::endl;
    
    // 创建代码生成器
    CodeGenerator generator(useDefaultStruct);
    
    // 生成输出
    std::cout << generator.generateOutput(ast) << std::endl;
    
    return 0;
}