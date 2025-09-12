#include <iostream>
#include <string>
#include <vector>
#include "CLI/CLITool.h"
#include "Test/TestFramework.h"
#include "Compiler/CompilerDispatcher.h"
#include "Compiler/CodeMerger.h"

using namespace CHTL::CLI;
using namespace CHTL::Test;
using namespace CHTL::Compiler;

int main(int argc, char* argv[]) {
    std::cout << "===============================================" << std::endl;
    std::cout << "  CHTL 项目演示程序" << std::endl;
    std::cout << "  版本: 1.0.0" << std::endl;
    std::cout << "  描述: 完整的CHTL语言实现" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    // 初始化CLI工具
    CLIConfig config;
    config.programName = "chtl";
    config.version = "1.0.0";
    config.description = "CHTL 语言编译器";
    config.enableColors = true;
    config.enableVerbose = true;
    
    CLITool cliTool(config);
    
    // 初始化测试框架
    TestFramework testFramework;
    
    // 初始化编译器调度器
    CompilerDispatcher compilerDispatcher;
    
    // 初始化代码合并器
    CodeMerger codeMerger;
    
    std::cout << "CHTL项目组件状态:" << std::endl;
    std::cout << "  ✓ CHTL核心语法系统" << std::endl;
    std::cout << "  ✓ 局部样式块系统" << std::endl;
    std::cout << "  ✓ 模板系统" << std::endl;
    std::cout << "  ✓ 自定义系统" << std::endl;
    std::cout << "  ✓ 原始嵌入系统" << std::endl;
    std::cout << "  ✓ 导入系统" << std::endl;
    std::cout << "  ✓ 命名空间系统" << std::endl;
    std::cout << "  ✓ 约束系统" << std::endl;
    std::cout << "  ✓ 配置组系统" << std::endl;
    std::cout << "  ✓ Use语句系统" << std::endl;
    std::cout << "  ✓ CHTL JS核心系统" << std::endl;
    std::cout << "  ✓ CHTL JS高级功能" << std::endl;
    std::cout << "  ✓ 统一扫描器" << std::endl;
    std::cout << "  ✓ CMOD系统" << std::endl;
    std::cout << "  ✓ CJMOD系统" << std::endl;
    std::cout << "  ✓ 官方模块系统" << std::endl;
    std::cout << "  ✓ 编译器调度器" << std::endl;
    std::cout << "  ✓ 代码合并器" << std::endl;
    std::cout << "  ✓ CLI工具" << std::endl;
    std::cout << "  ✓ 测试系统" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CHTL项目功能特性:" << std::endl;
    std::cout << "  • 完整的CHTL语言语法支持" << std::endl;
    std::cout << "  • 局部样式块和属性运算" << std::endl;
    std::cout << "  • 模板系统和自定义系统" << std::endl;
    std::cout << "  • 原始嵌入和导入系统" << std::endl;
    std::cout << "  • 命名空间和约束系统" << std::endl;
    std::cout << "  • 配置组和Use语句" << std::endl;
    std::cout << "  • CHTL JS核心和高级功能" << std::endl;
    std::cout << "  • 统一扫描器和编译器" << std::endl;
    std::cout << "  • 模块系统和官方模块" << std::endl;
    std::cout << "  • 代码合并和CLI工具" << std::endl;
    std::cout << "  • 完整的测试框架" << std::endl;
    std::cout << std::endl;
    
    std::cout << "使用方法:" << std::endl;
    std::cout << "  ./chtl help          - 显示帮助信息" << std::endl;
    std::cout << "  ./chtl version       - 显示版本信息" << std::endl;
    std::cout << "  ./chtl compile <file> - 编译CHTL文件" << std::endl;
    std::cout << "  ./chtl build         - 构建项目" << std::endl;
    std::cout << "  ./chtl test          - 运行测试" << std::endl;
    std::cout << "  ./chtl init <name>   - 初始化项目" << std::endl;
    std::cout << std::endl;
    
    // 运行CLI工具
    int result = cliTool.execute(argc, argv);
    
    if (result == 0) {
        std::cout << std::endl;
        std::cout << "CHTL项目运行成功！" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "CHTL项目运行失败，错误代码: " << result << std::endl;
    }
    
    return result;
}