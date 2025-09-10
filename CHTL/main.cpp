#include <iostream>
#include <string>
#include <vector>
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL_JS/CHTLJSContext/CHTLJSContext.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "基于C++语言实现的超文本语言编译器" << std::endl;
    std::cout << "MIT开源协议" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: chtl_compiler <输入文件> [输出文件]" << std::endl;
        std::cout << "示例: chtl_compiler input.chtl output.html" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // 创建编译器调度器
        CHTL::CompilerDispatcher dispatcher;
        
        // 创建统一扫描器
        CHTL::CHTLUnifiedScanner scanner;
        
        // 扫描输入文件
        std::cout << "正在扫描文件: " << inputFile << std::endl;
        auto scanResult = scanner.scan(inputFile);
        
        // 分发到各编译器
        std::cout << "正在编译..." << std::endl;
        auto compileResult = dispatcher.compile(scanResult);
        
        // 合并代码
        std::cout << "正在合并代码..." << std::endl;
        auto finalResult = dispatcher.merge(compileResult);
        
        // 输出结果
        std::cout << "正在输出到: " << outputFile << std::endl;
        dispatcher.output(finalResult, outputFile);
        
        std::cout << "编译完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "编译错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}