#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> [output.html]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // 打开输入文件
        std::ifstream input(inputFile);
        if (!input.is_open()) {
            std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
            return 1;
        }
        
        // 创建上下文
        CHTLContext context;
        context.setCurrentFile(inputFile);
        
        // 创建解析器
        CHTLParser parser(input);
        parser.setConfig(context.getConfig());
        
        // 解析文件
        auto root = parser.parse();
        if (!root) {
            std::cerr << "Error: Failed to parse input file" << std::endl;
            return 1;
        }
        
        // 检查解析错误
        const auto& errors = parser.getErrors();
        if (!errors.empty()) {
            std::cerr << "Parse errors:" << std::endl;
            for (const auto& error : errors) {
                std::cerr << "  " << error.position.line << ":" << error.position.column 
                         << " " << error.message << std::endl;
            }
            return 1;
        }
        
        // 设置根节点
        context.setRootNode(root);
        
        // 打开输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot open output file " << outputFile << std::endl;
            return 1;
        }
        
        // 生成 HTML
        HTMLGenerator htmlGenerator(output);
        htmlGenerator.setConfig(context.getConfig());
        htmlGenerator.generate(root);
        
        // 检查生成错误
        const auto& genErrors = htmlGenerator.getErrors();
        if (!genErrors.empty()) {
            std::cerr << "Generation errors:" << std::endl;
            for (const auto& error : genErrors) {
                std::cerr << "  " << error.position.line << ":" << error.position.column 
                         << " " << error.message << std::endl;
            }
            return 1;
        }
        
        std::cout << "Successfully generated " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}