#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/Common.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argc > 2 ? argv[2] : "output.html";
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "File size: " << content.length() << " bytes" << std::endl;
    
    // 创建词法分析器
    CHTLLexer lexer(content);
    
    // 进行词法分析
    std::cout << "Starting lexical analysis..." << std::endl;
    TokenList tokens = lexer.tokenize();
    
    std::cout << "Tokens found: " << tokens.size() << std::endl;
    
    // 检查错误
    if (lexer.hasErrors()) {
        std::cout << "Lexical errors found:" << std::endl;
        lexer.printErrors();
        return 1;
    }
    
    // 打印前几个token作为示例
    std::cout << "First 10 tokens:" << std::endl;
    for (size_t i = 0; i < std::min(tokens.size(), size_t(10)); i++) {
        const Token& token = tokens[i];
        std::cout << "  " << i << ": " << static_cast<int>(token.type) 
                  << " - " << token.value << std::endl;
    }
    
    // 生成输出文件
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
        return 1;
    }
    
    // 简单的HTML输出（目前只是示例）
    outFile << "<!DOCTYPE html>" << std::endl;
    outFile << "<html>" << std::endl;
    outFile << "<head>" << std::endl;
    outFile << "  <title>CHTL Output</title>" << std::endl;
    outFile << "</head>" << std::endl;
    outFile << "<body>" << std::endl;
    outFile << "  <h1>CHTL Compiler Output</h1>" << std::endl;
    outFile << "  <p>This is a basic output from CHTL compiler.</p>" << std::endl;
    outFile << "  <p>Input file: " << inputFile << "</p>" << std::endl;
    outFile << "  <p>Tokens processed: " << tokens.size() << "</p>" << std::endl;
    outFile << "</body>" << std::endl;
    outFile << "</html>" << std::endl;
    
    outFile.close();
    
    std::cout << "Output file generated: " << outputFile << std::endl;
    std::cout << "Compilation completed successfully!" << std::endl;
    
    return 0;
}