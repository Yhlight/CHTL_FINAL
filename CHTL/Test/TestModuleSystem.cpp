#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>

#include "CHTL/ModuleSystem/CMODSystem.h"
#include "CHTL/ModuleSystem/CJMODSystem.h"
#include "CHTL/ModuleSystem/HybridModuleSystem.h"

using namespace CHTL;

void testCMODSystem() {
    std::cout << "Testing CMOD System..." << std::endl;
    
    CMODSystem cmodSystem;
    
    // 创建CMOD模块
    auto cmod = std::make_shared<CMODModule>();
    
    CMODInfo info;
    info.name = "TestCMOD";
    info.version = "1.0.0";
    info.description = "Test CMOD module";
    info.author = "Test Author";
    info.license = "MIT";
    cmod->setInfo(info);
    
    cmod->setSource("// Test CMOD source\n[Template] @Style TestStyle { color: red; }");
    
    // 添加导出项
    ExportItem export1;
    export1.type = ExportType::Template;
    export1.name = "TestStyle";
    export1.description = "Test style template";
    export1.isPublic = true;
    cmod->addExport(export1);
    
    // 注册模块
    cmodSystem.registerModule(cmod);
    assert(cmodSystem.hasModule("TestCMOD"));
    
    // 测试搜索
    auto searchResults = cmodSystem.searchModules("Test");
    assert(!searchResults.empty());
    
    // 测试获取模块
    auto retrievedModule = cmodSystem.getModule("TestCMOD");
    assert(retrievedModule != nullptr);
    assert(retrievedModule->getInfo().name == "TestCMOD");
    
    std::cout << "CMOD System tests passed!" << std::endl << std::endl;
}

void testCJMODSystem() {
    std::cout << "Testing CJMOD System..." << std::endl;
    
    CJMODSystem cjmodSystem;
    
    // 创建CJMOD模块
    auto cjmod = std::make_shared<CJMODModule>();
    
    CJMODInfo info;
    info.name = "TestCJMOD";
    info.version = "1.0.0";
    info.description = "Test CJMOD module";
    info.author = "Test Author";
    info.license = "MIT";
    cjmod->setInfo(info);
    
    cjmod->setSource("// Test CJMOD source\nfileloader { load: \"./test.js\"; }");
    
    // 添加函数
    CJMODFunction func1;
    func1.name = "testFunction";
    func1.description = "Test function";
    func1.type = CJMODFunctionType::Utility;
    func1.parameters = {"param1", "param2"};
    func1.returnType = "void";
    func1.isPublic = true;
    func1.implementation = []() { std::cout << "Test function called!" << std::endl; };
    cjmod->addFunction(func1);
    
    // 注册模块
    cjmodSystem.registerModule(cjmod);
    assert(cjmodSystem.hasModule("TestCJMOD"));
    
    // 测试搜索
    auto searchResults = cjmodSystem.searchFunctions("test");
    assert(!searchResults.empty());
    
    // 测试函数调用
    bool callResult = cjmodSystem.callFunction("TestCJMOD", "testFunction");
    assert(callResult);
    
    std::cout << "CJMOD System tests passed!" << std::endl << std::endl;
}

void testHybridModuleSystem() {
    std::cout << "Testing Hybrid Module System..." << std::endl;
    
    HybridModuleSystem hybridSystem;
    
    // 创建混合模块
    auto hybrid = std::make_shared<HybridModule>();
    
    HybridModuleInfo info;
    info.name = "TestHybrid";
    info.version = "1.0.0";
    info.description = "Test hybrid module";
    info.author = "Test Author";
    info.license = "MIT";
    info.type = HybridModuleType::HYBRID;
    hybrid->setInfo(info);
    
    // 添加CMOD部分
    auto cmod = std::make_shared<CMODModule>();
    CMODInfo cmodInfo;
    cmodInfo.name = "TestHybrid_CMOD";
    cmodInfo.version = "1.0.0";
    cmodInfo.description = "CMOD part of hybrid module";
    cmod->setInfo(cmodInfo);
    cmod->setSource("// CMOD part\n[Template] @Style HybridStyle { color: blue; }");
    
    ExportItem export1;
    export1.type = ExportType::Template;
    export1.name = "HybridStyle";
    export1.description = "Hybrid style template";
    export1.isPublic = true;
    cmod->addExport(export1);
    
    hybrid->setCMODModule(cmod);
    
    // 添加CJMOD部分
    auto cjmod = std::make_shared<CJMODModule>();
    CJMODInfo cjmodInfo;
    cjmodInfo.name = "TestHybrid_CJMOD";
    cjmodInfo.version = "1.0.0";
    cjmodInfo.description = "CJMOD part of hybrid module";
    cjmod->setInfo(cjmodInfo);
    cjmod->setSource("// CJMOD part\nlisten { click: () => {}; }");
    
    CJMODFunction func1;
    func1.name = "hybridFunction";
    func1.description = "Hybrid function";
    func1.type = CJMODFunctionType::Utility;
    func1.parameters = {};
    func1.returnType = "void";
    func1.isPublic = true;
    func1.implementation = []() { std::cout << "Hybrid function called!" << std::endl; };
    cjmod->addFunction(func1);
    
    hybrid->setCJMODModule(cjmod);
    
    // 注册混合模块
    hybridSystem.registerModule(hybrid);
    assert(hybridSystem.hasModule("TestHybrid"));
    
    // 测试获取导出项
    auto exports = hybridSystem.getExports("TestHybrid");
    assert(!exports.empty());
    
    // 测试函数调用
    bool callResult = hybridSystem.callFunction("TestHybrid", "hybridFunction");
    assert(callResult);
    
    // 测试模块类型
    assert(hybrid->getInfo().type == HybridModuleType::HYBRID);
    assert(hybrid->hasCMODPart());
    assert(hybrid->hasCJMODPart());
    
    std::cout << "Hybrid Module System tests passed!" << std::endl << std::endl;
}

void testModuleIntegration() {
    std::cout << "Testing Module Integration..." << std::endl;
    
    HybridModuleSystem system;
    
    // 创建依赖关系
    auto module1 = std::make_shared<HybridModule>();
    HybridModuleInfo info1;
    info1.name = "Module1";
    info1.version = "1.0.0";
    info1.description = "Base module";
    module1->setInfo(info1);
    module1->addDependency("Module2", "1.0.0");
    
    auto module2 = std::make_shared<HybridModule>();
    HybridModuleInfo info2;
    info2.name = "Module2";
    info2.version = "1.0.0";
    info2.description = "Dependency module";
    module2->setInfo(info2);
    
    // 注册模块
    system.registerModule(module2);
    system.registerModule(module1);
    
    // 测试依赖解析
    auto resolved = system.resolveDependencies("Module1");
    assert(resolved.size() >= 2); // Module1 和 Module2
    
    // 测试冲突检查
    bool hasConflicts = system.checkDependencyConflicts("Module1");
    assert(!hasConflicts); // 应该没有冲突
    
    std::cout << "Module Integration tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== CHTL Module System Tests ===" << std::endl << std::endl;
    
    testCMODSystem();
    testCJMODSystem();
    testHybridModuleSystem();
    testModuleIntegration();
    
    std::cout << "All module system tests passed!" << std::endl;
    return 0;
}