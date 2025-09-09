#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    
    // 解析命令行参数
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[i + 1];
            i++; // 跳过输出文件名
        }
    }
    
    try {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
            return 1;
        }
        
        std::string source((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        file.close();
        
        // 解析CHTL代码
        CHTL::CHTLParser parser(source);
        auto ast = parser.parse();
        
        // 生成代码
        CHTL::CHTLGenerator generator;
        std::string html = generator.generateDocument(ast);
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file: " << outputFile << std::endl;
            return 1;
        }
        
        outFile << html;
        outFile.close();
        
        std::cout << "Successfully generated " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}