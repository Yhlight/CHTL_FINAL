#include "CHTL/CHTLContext.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <输入文件> [输出文件]" << std::endl;
        std::cout << "示例: " << argv[0] << " example.chtl output.html" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argc > 2 ? argv[2] : "";
    
    CHTL::CHTLContext context;
    context.setDebugMode(true);
    
    std::cout << "开始编译 CHTL 文件: " << inputFile << std::endl;
    
    bool success = context.compile(inputFile, outputFile);
    
    if (success) {
        std::cout << "编译成功!" << std::endl;
        if (!outputFile.empty()) {
            std::cout << "输出文件: " << outputFile << std::endl;
        }
    } else {
        std::cout << "编译失败!" << std::endl;
        const auto& errors = context.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
        return 1;
    }
    
    return 0;
}