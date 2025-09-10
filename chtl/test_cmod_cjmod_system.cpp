#include "chtl/cmod_cjmod_system.h"
#include <iostream>
#include <string>

using namespace chtl::cmod_cjmod;

int main() {
    std::cout << "=== CHTL CMOD/CJMOD 系统测试 ===" << std::endl;
    
    // 测试CJMOD API
    std::cout << "1. 测试CJMOD API..." << std::endl;
    
    // 测试语法分析
    std::string testCode = "function test() { return 'hello'; }";
    std::cout << "  语法分析: " << (CJMODSyntax::analyze(testCode) ? "通过" : "失败") << std::endl;
    std::cout << "  是对象: " << (CJMODSyntax::isObject("{ key: 'value' }") ? "是" : "否") << std::endl;
    std::cout << "  是函数: " << (CJMODSyntax::isFunction(testCode) ? "是" : "否") << std::endl;
    std::cout << "  是数组: " << (CJMODSyntax::isArray("[1, 2, 3]") ? "是" : "否") << std::endl;
    std::cout << "  是CHTL JS函数: " << (CJMODSyntax::isCHTLJSFunction("chtl::function") ? "是" : "否") << std::endl;
    
    // 测试参数处理
    std::cout << "  参数绑定: " << CJMODArg::bind("arg1", "value1") << std::endl;
    std::cout << "  填充值: " << CJMODArg::fillValue("arg2", "value2") << std::endl;
    std::cout << "  转换: " << CJMODArg::transform("arg3", "transform") << std::endl;
    
    // 测试扫描器
    CJMODScanner scanner("function test() { return 'hello'; }");
    auto tokens = scanner.scan();
    std::cout << "  扫描到 " << tokens.size() << " 个token" << std::endl;
    
    // 测试生成器
    CJMODGenerator generator;
    generator.addExport("test", "function");
    std::cout << "  生成代码: " << generator.exportResult("test") << std::endl;
    
    // 测试原子参数
    std::cout << "  原子参数绑定: " << CJMODAtomArg::bind("$", "value") << std::endl;
    std::cout << "  原子参数填充: " << CJMODAtomArg::fillValue("$?", "value") << std::endl;
    
    // 测试CHTL JS函数
    std::cout << "  CHTL JS函数: " << CJMODCHTLJSFunction::CreateCHTLJSFunction("test", "return 'hello';") << std::endl;
    std::cout << "  虚拟对象绑定: " << CJMODCHTLJSFunction::bindVirtualObject("test", "obj") << std::endl;
    
    // 测试CMOD模块
    std::cout << "\n2. 测试CMOD模块..." << std::endl;
    
    auto cmodModule = std::make_shared<CMODModule>("TestCMOD");
    
    ModuleInfo info;
    info.name = "TestCMOD";
    info.version = "1.0.0";
    info.description = "Test CHTL module";
    info.author = "Test Author";
    info.dependencies = {"chtl::core"};
    
    cmodModule->setInfo(info);
    
    // 添加导出
    cmodModule->addExport(ModuleExport("TestClass", "class", "TestClass", true));
    cmodModule->addExport(ModuleExport("testFunction", "function", "testFunction", true));
    
    // 添加源文件
    cmodModule->addSourceFile("src/test.cpp");
    cmodModule->addInfoFile("info/module.info");
    
    std::cout << "  CMOD模块代码:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << cmodModule->generateModuleCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    std::cout << "  CMOD模块信息:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << cmodModule->generateInfoCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    // 测试CJMOD模块
    std::cout << "\n3. 测试CJMOD模块..." << std::endl;
    
    auto cjmodModule = std::make_shared<CJMODModule>("TestCJMOD");
    
    ModuleInfo cjmodInfo;
    cjmodInfo.name = "TestCJMOD";
    cjmodInfo.version = "1.0.0";
    cjmodInfo.description = "Test CHTL JS module";
    cjmodInfo.author = "Test Author";
    cjmodInfo.dependencies = {"chtl::core"};
    
    cjmodModule->setInfo(cjmodInfo);
    
    // 添加CHTL JS函数
    cjmodModule->addCHTLJSFunction("listen", "// 事件监听器实现");
    cjmodModule->addCHTLJSFunction("animate", "// 动画实现");
    
    // 添加虚拟对象
    cjmodModule->addVirtualObject("vir", "// 虚拟对象实现");
    
    // 添加导出
    cjmodModule->addExport(ModuleExport("listen", "function", "listen", true));
    cjmodModule->addExport(ModuleExport("animate", "function", "animate", true));
    
    std::cout << "  CJMOD模块代码:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << cjmodModule->generateModuleCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    std::cout << "  CJMOD模块信息:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << cjmodModule->generateInfoCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    // 测试混合模块
    std::cout << "\n4. 测试混合模块..." << std::endl;
    
    auto mixedModule = std::make_shared<MixedModule>("TestMixed");
    
    ModuleInfo mixedInfo;
    mixedInfo.name = "TestMixed";
    mixedInfo.version = "1.0.0";
    mixedInfo.description = "Test mixed module";
    mixedInfo.author = "Test Author";
    mixedInfo.dependencies = {"chtl::core"};
    
    mixedModule->setInfo(mixedInfo);
    
    // 添加CMOD和CJMOD模块
    mixedModule->addCMODModule(cmodModule);
    mixedModule->addCJMODModule(cjmodModule);
    
    std::cout << "  混合模块代码:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << mixedModule->generateModuleCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    std::cout << "  混合模块信息:" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << mixedModule->generateInfoCode() << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    
    // 测试官方模块
    std::cout << "\n5. 测试官方模块..." << std::endl;
    
    auto chthollyCMOD = OfficialModule::createChthollyCMOD();
    auto chthollyCJMOD = OfficialModule::createChthollyCJMOD();
    auto yuigahamaCMOD = OfficialModule::createYuigahamaCMOD();
    
    std::cout << "  Chtholly CMOD: " << chthollyCMOD->getInfo().name << " v" << chthollyCMOD->getInfo().version << std::endl;
    std::cout << "  Chtholly CJMOD: " << chthollyCJMOD->getInfo().name << " v" << chthollyCJMOD->getInfo().version << std::endl;
    std::cout << "  Yuigahama CMOD: " << yuigahamaCMOD->getInfo().name << " v" << yuigahamaCMOD->getInfo().version << std::endl;
    
    std::cout << "  Chtholly特性: " << OfficialModule::getChthollyFeatures() << std::endl;
    std::cout << "  Yuigahama特性: " << OfficialModule::getYuigahamaFeatures() << std::endl;
    
    // 测试模块管理器
    std::cout << "\n6. 测试模块管理器..." << std::endl;
    
    ModuleManager manager;
    
    // 添加模块
    manager.addModule(cmodModule);
    manager.addModule(cjmodModule);
    manager.addModule(mixedModule);
    manager.addModule(chthollyCMOD);
    manager.addModule(chthollyCJMOD);
    manager.addModule(yuigahamaCMOD);
    
    std::cout << "  总模块数: " << manager.getModuleCount() << std::endl;
    
    // 生成所有模块代码
    std::string allModules = manager.generateAllModules();
    std::cout << "  生成的所有模块代码长度: " << allModules.length() << " 字符" << std::endl;
    
    // 生成模块信息
    std::string moduleInfo = manager.generateModuleInfo();
    std::cout << "  生成的模块信息长度: " << moduleInfo.length() << " 字符" << std::endl;
    
    // 测试模块查找
    auto foundCMOD = manager.getCMODModule("TestCMOD");
    auto foundCJMOD = manager.getCJMODModule("TestCJMOD");
    auto foundMixed = manager.getMixedModule("TestMixed");
    
    std::cout << "  找到CMOD模块: " << (foundCMOD ? "是" : "否") << std::endl;
    std::cout << "  找到CJMOD模块: " << (foundCJMOD ? "是" : "否") << std::endl;
    std::cout << "  找到混合模块: " << (foundMixed ? "是" : "否") << std::endl;
    
    std::cout << "\n=== CMOD/CJMOD 系统测试完成 ===" << std::endl;
    
    return 0;
}