#include "CLI/CLITool.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== CLI调试 ===" << std::endl;
    
    // 创建CLI工具
    CHTL::CLI::CLITool cli;
    
    // 测试直接调用帮助
    std::cout << "\n直接调用帮助:" << std::endl;
    cli.showHelp();
    
    // 测试直接调用版本
    std::cout << "\n直接调用版本:" << std::endl;
    cli.showVersion();
    
    return 0;
}