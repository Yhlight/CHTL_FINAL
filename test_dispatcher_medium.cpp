#include <iostream>
#include <string>
#include "Compiler/CompilerDispatcher.h"

using namespace CHTL;

int main() {
    std::cout << "=== Medium Compiler Dispatcher Test ===" << std::endl;
    
    // 测试中等复杂度的CHTL代码
    std::string chtl_code = "div { text { Hello } style { color: red; } }";
    
    std::cout << "Source: " << chtl_code << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 创建编译器调度器
    CompilerDispatcher dispatcher;
    
    // 编译CHTL代码
    std::cout << "Compiling CHTL code..." << std::endl;
    auto result = dispatcher.compile(chtl_code);
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Generated HTML:" << std::endl;
        std::cout << result.html << std::endl;
        std::cout << "Generated CSS:" << std::endl;
        std::cout << result.css << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    return 0;
}