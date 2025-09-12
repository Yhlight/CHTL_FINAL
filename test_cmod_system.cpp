#include <iostream>
#include <string>
#include "Module/CMODSystem.h"

using namespace CHTL;

int main() {
    std::cout << "=== CMOD System Test ===" << std::endl;
    
    // 创建CMOD系统
    CMODSystem cmod_system;
    cmod_system.setDebugMode(true);
    
    // 初始化系统
    cmod_system.initialize("./modules", "./user_modules");
    
    std::cout << "CMOD System initialized successfully!" << std::endl;
    
    // 测试模块搜索
    std::cout << "\n1. Testing module search..." << std::endl;
    std::string test_module = "test_module";
    if (cmod_system.moduleExists(test_module)) {
        std::cout << "Module '" << test_module << "' exists" << std::endl;
    } else {
        std::cout << "Module '" << test_module << "' not found" << std::endl;
    }
    
    // 测试加载模块
    std::cout << "\n2. Testing module loading..." << std::endl;
    auto module = cmod_system.loadModule(test_module);
    if (module) {
        std::cout << "Successfully loaded module: " << module->name << std::endl;
        std::cout << "  Version: " << module->info.version << std::endl;
        std::cout << "  Description: " << module->info.description << std::endl;
        std::cout << "  Author: " << module->info.author << std::endl;
        std::cout << "  Exports: " << module->info.exports.size() << " items" << std::endl;
        
        // 显示导出表
        if (!module->info.exports.empty()) {
            std::cout << "  Export table:" << std::endl;
            for (const auto& export_item : module->info.exports) {
                std::cout << "    " << export_item.first << ": " << export_item.second << std::endl;
            }
        }
    } else {
        std::cout << "Failed to load module: " << test_module << std::endl;
    }
    
    // 显示错误信息
    std::cout << "\n3. Error information:" << std::endl;
    auto errors = cmod_system.getErrors();
    if (errors.empty()) {
        std::cout << "No errors" << std::endl;
    } else {
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    // 显示已加载的模块
    std::cout << "\n4. Loaded modules:" << std::endl;
    auto loaded_modules = cmod_system.getLoadedModules();
    std::cout << "Total loaded modules: " << loaded_modules.size() << std::endl;
    
    for (const auto& loaded_module : loaded_modules) {
        std::cout << "  - " << loaded_module->name << " (v" << loaded_module->info.version << ")" << std::endl;
    }
    
    std::cout << "\n=== CMOD System Test Complete ===" << std::endl;
    
    return 0;
}