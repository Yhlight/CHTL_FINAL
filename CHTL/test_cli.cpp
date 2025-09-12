#include "CLI/CLITool.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== CLI工具测试 ===" << std::endl;
    
    // 创建CLI工具
    CHTL::CLI::CLITool cli;
    
    // 测试帮助命令
    std::cout << "\n测试帮助命令:" << std::endl;
    std::vector<std::string> helpArgs = {"chtl", "help"};
    int result = cli.execute(helpArgs);
    std::cout << "帮助命令结果: " << result << std::endl;
    
    // 测试版本命令
    std::cout << "\n测试版本命令:" << std::endl;
    std::vector<std::string> versionArgs = {"chtl", "version"};
    result = cli.execute(versionArgs);
    std::cout << "版本命令结果: " << result << std::endl;
    
    // 测试编译命令（使用一个简单的CHTL文件）
    std::cout << "\n测试编译命令:" << std::endl;
    std::vector<std::string> compileArgs = {"chtl", "compile", "test.chtl"};
    result = cli.execute(compileArgs);
    std::cout << "编译命令结果: " << result << std::endl;
    
    return 0;
}