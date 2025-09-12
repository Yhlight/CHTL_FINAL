#include <iostream>
#include <string>
#include <vector>
#include "Module/CJMODSystem.h"

using namespace CHTL;

int main() {
    std::cout << "=== CJMOD System Test ===" << std::endl;
    
    // 创建CJMOD系统
    CJMODSystem cjmod_system;
    cjmod_system.setDebugMode(true);
    
    // 初始化系统
    cjmod_system.initialize("./modules", "./user_modules");
    
    std::cout << "CJMOD System initialized successfully!" << std::endl;
    
    // 测试CJMOD API
    std::cout << "\n1. Testing CJMOD API..." << std::endl;
    
    // 测试语法分析
    std::cout << "Testing Syntax::analyze..." << std::endl;
    Arg args = Syntax::analyze("$ ** $");
    args.print();
    
    // 测试扫描器
    std::cout << "Testing CJMODScanner::scan..." << std::endl;
    Arg result = CJMODScanner::scan(args, "**");
    result.print();
    
    // 测试生成器
    std::cout << "Testing CJMODGenerator::exportResult..." << std::endl;
    std::string generated = CJMODGenerator::exportResult(result);
    std::cout << "Generated: " << generated << std::endl;
    
    // 测试CHTL JS函数创建
    std::cout << "Testing CHTLJSFunction::createFunction..." << std::endl;
    std::vector<std::string> params = {"a", "b"};
    std::string function = CHTLJSFunction::createFunction("add", params, "return a + b;");
    std::cout << "Created function:\n" << function << std::endl;
    
    // 测试模块搜索
    std::cout << "\n2. Testing module search..." << std::endl;
    std::string test_module = "test";
    if (cjmod_system.moduleExists(test_module)) {
        std::cout << "Module '" << test_module << "' exists" << std::endl;
    } else {
        std::cout << "Module '" << test_module << "' not found" << std::endl;
    }
    
    // 测试加载模块
    std::cout << "\n3. Testing module loading..." << std::endl;
    auto module = cjmod_system.loadModule(test_module);
    if (module) {
        std::cout << "Successfully loaded module: " << module->name << std::endl;
        std::cout << "  Version: " << module->info.version << std::endl;
        std::cout << "  Description: " << module->info.description << std::endl;
        std::cout << "  Author: " << module->info.author << std::endl;
        std::cout << "  Functions: " << module->functions.size() << " items" << std::endl;
    } else {
        std::cout << "Failed to load module: " << test_module << std::endl;
    }
    
    // 测试函数注册和执行
    std::cout << "\n4. Testing function registration and execution..." << std::endl;
    
    // 注册一个测试函数
    cjmod_system.registerFunction("test", "hello", [](const std::vector<std::string>& args) -> std::string {
        return "Hello from CJMOD function! Args: " + std::to_string(args.size());
    });
    
    // 执行函数
    std::vector<std::string> test_args = {"arg1", "arg2"};
    std::string result_str = cjmod_system.executeFunction("test", "hello", test_args);
    std::cout << "Function execution result: " << result_str << std::endl;
    
    // 显示错误信息
    std::cout << "\n5. Error information:" << std::endl;
    auto errors = cjmod_system.getErrors();
    if (errors.empty()) {
        std::cout << "No errors" << std::endl;
    } else {
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    // 显示已加载的模块
    std::cout << "\n6. Loaded modules:" << std::endl;
    auto loaded_modules = cjmod_system.getLoadedModules();
    std::cout << "Total loaded modules: " << loaded_modules.size() << std::endl;
    
    for (const auto& loaded_module : loaded_modules) {
        std::cout << "  - " << loaded_module->name << " (v" << loaded_module->info.version << ")" << std::endl;
    }
    
    std::cout << "\n=== CJMOD System Test Complete ===" << std::endl;
    
    return 0;
}