#include <iostream>
#include <string>
#include <fstream>
#include "CompilerDispatcher/CompilerDispatcher.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "基于C++实现的超文本语言编译器" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: chtl <输入文件> [选项]" << std::endl;
        std::cout << "选项:" << std::endl;
        std::cout << "  --default-struct    输出带有默认结构的HTML代码" << std::endl;
        std::cout << "  --inline           生成内联的HTML文件" << std::endl;
        std::cout << "  --inline-css       内联CSS到HTML" << std::endl;
        std::cout << "  --inline-js        内联JS到HTML" << std::endl;
        std::cout << "  --output <文件>    指定输出文件" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "";
    bool defaultStruct = false;
    bool inlineMode = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    
    // 解析命令行参数
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--default-struct") {
            defaultStruct = true;
        } else if (arg == "--inline") {
            inlineMode = true;
        } else if (arg == "--inline-css") {
            inlineCSS = true;
        } else if (arg == "--inline-js") {
            inlineJS = true;
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }
    
    try {
        // 创建编译器调度器
        CHTL::CompilerDispatcher dispatcher;
        
        // 设置编译选项
        dispatcher.setDefaultStruct(defaultStruct);
        dispatcher.setInlineMode(inlineMode);
        dispatcher.setInlineCSS(inlineCSS);
        dispatcher.setInlineJS(inlineJS);
        
        // 编译文件
        std::string result = dispatcher.compile(inputFile);
        
        // 输出结果
        if (outputFile.empty()) {
            std::cout << result << std::endl;
        } else {
            // 写入文件
            std::ofstream out(outputFile);
            if (out.is_open()) {
                out << result;
                out.close();
                std::cout << "编译完成，输出到: " << outputFile << std::endl;
            } else {
                std::cerr << "无法写入输出文件: " << outputFile << std::endl;
                return 1;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}