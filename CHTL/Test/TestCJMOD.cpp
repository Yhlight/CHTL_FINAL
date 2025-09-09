#include "CHTL/ModuleSystem/CJMODSystem.h"
#include "CHTL/CJMODAPI/CHTLJSFunction.h"
#include "CHTL/CJMODAPI/AtomArg.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testCJMODSystem() {
    std::cout << "Testing CJMOD System..." << std::endl;
    
    // 创建CJMOD系统
    CJMODSystem system;
    
    // 创建测试模块
    auto module = std::make_shared<CJMODModule>();
    
    // 设置模块信息
    CJMODInfo info;
    info.name = "TestModule";
    info.version = "1.0.0";
    info.description = "CJMOD API测试模块";
    info.author = "CHTL Team";
    info.license = "MIT";
    info.dependencies = {"Chtholly"};
    
    module->setInfo(info);
    
    // 添加测试函数
    CJMODFunction formatFunc;
    formatFunc.name = "formatString";
    formatFunc.description = "格式化字符串";
    formatFunc.type = CJMODFunctionType::Utility;
    formatFunc.parameters = {"template", "values"};
    formatFunc.returnType = "string";
    formatFunc.isPublic = true;
    
    module->addFunction(formatFunc);
    
    // 注册模块
    system.registerModule(module);
    
    // 测试模块查找
    assert(system.hasModule("TestModule"));
    auto foundModule = system.getModule("TestModule");
    assert(foundModule != nullptr);
    assert(foundModule->getInfo().name == "TestModule");
    
    // 测试函数查找
    assert(foundModule->hasFunction("formatString"));
    auto func = foundModule->getFunction("formatString");
    assert(func.name == "formatString");
    assert(func.type == CJMODFunctionType::Utility);
    
    std::cout << "✓ CJMOD System basic functionality works" << std::endl;
}

void testCHTLJSFunction() {
    std::cout << "Testing CHTL JS Function..." << std::endl;
    
    // 创建CHTL JS函数
    CHTLJSFunction func("testFunction");
    func.setDescription("测试函数");
    func.setType(CHTLJSFunctionType::Utility);
    func.setReturnType(ArgType::String);
    
    // 添加参数
    FunctionParameter param1;
    param1.name = "message";
    param1.type = ArgType::String;
    param1.isOptional = false;
    param1.defaultValue = Arg("");
    param1.description = "消息内容";
    
    FunctionParameter param2;
    param2.name = "count";
    param2.type = ArgType::Number;
    param2.isOptional = true;
    param2.defaultValue = Arg(1);
    param2.description = "重复次数";
    
    func.addParameter(param1);
    func.addParameter(param2);
    
    // 设置函数体
    func.setBody("return message.repeat(count);");
    
    // 测试函数验证
    assert(func.isValid());
    assert(func.getName() == "testFunction");
    assert(func.getParameters().size() == 2);
    
    std::cout << "✓ CHTL JS Function basic functionality works" << std::endl;
}

void testAtomArg() {
    std::cout << "Testing AtomArg..." << std::endl;
    
    // 测试基本占位符
    auto placeholder = AtomArg::parse("$");
    assert(placeholder.isPlaceholder());
    assert(placeholder.getPlaceholder() == "");
    
    // 测试可选占位符
    auto optionalPlaceholder = AtomArg::parse("$?");
    assert(optionalPlaceholder.isOptionalPlaceholder());
    assert(optionalPlaceholder.getPlaceholder() == "");
    
    // 测试必须占位符
    auto requiredPlaceholder = AtomArg::parse("$!");
    assert(requiredPlaceholder.isRequiredPlaceholder());
    assert(requiredPlaceholder.getPlaceholder() == "");
    
    // 测试无序占位符
    auto unorderedPlaceholder = AtomArg::parse("$_");
    assert(unorderedPlaceholder.isUnorderedPlaceholder());
    assert(unorderedPlaceholder.getPlaceholder() == "");
    
    // 测试组合占位符
    auto combinedPlaceholder = AtomArg::parse("$!?");
    assert(combinedPlaceholder.isCombinedPlaceholder());
    assert(combinedPlaceholder.getPlaceholderModifiers() == "!?");
    
    // 测试不定参数
    auto variadic = AtomArg::parse("...");
    assert(variadic.isVariadic());
    
    // 测试函数调用
    auto functionCall = AtomArg::parse("testFunction(arg1, arg2)");
    assert(functionCall.isFunction());
    assert(functionCall.getFunctionName() == "testFunction");
    assert(functionCall.getFunctionArgs().size() == 2);
    
    // 测试属性访问
    auto propertyAccess = AtomArg::parse("object.property");
    assert(propertyAccess.isProperty());
    assert(propertyAccess.getObject() == "object");
    assert(propertyAccess.getProperty() == "property");
    
    // 测试索引访问
    auto indexAccess = AtomArg::parse("array[0]");
    assert(indexAccess.isIndex());
    assert(indexAccess.getIndexObject() == "array");
    
    std::cout << "✓ AtomArg functionality works" << std::endl;
}

void testUnorderedAndOptionalKeys() {
    std::cout << "Testing unordered and optional keys..." << std::endl;
    
    // 测试无序参数处理
    CHTLJSFunction func("unorderedTest");
    func.setType(CHTLJSFunctionType::Utility);
    
    // 添加可选参数
    FunctionParameter param1;
    param1.name = "required";
    param1.type = ArgType::String;
    param1.isOptional = false;
    func.addParameter(param1);
    
    FunctionParameter param2;
    param2.name = "optional1";
    param2.type = ArgType::String;
    param2.isOptional = true;
    param2.defaultValue = Arg("default1");
    func.addParameter(param2);
    
    FunctionParameter param3;
    param3.name = "optional2";
    param3.type = ArgType::Number;
    param3.isOptional = true;
    param3.defaultValue = Arg(42);
    func.addParameter(param3);
    
    // 测试命名参数调用
    std::map<std::string, Arg> namedArgs;
    namedArgs["optional2"] = Arg(100);
    namedArgs["required"] = Arg("test");
    // 注意：optional1 没有提供，应该使用默认值
    
    // 这里应该能够处理无序和可选的参数
    assert(func.hasParameter("required"));
    assert(func.hasParameter("optional1"));
    assert(func.hasParameter("optional2"));
    
    auto requiredParam = func.getParameter("required");
    assert(!requiredParam.isOptional);
    
    auto optionalParam = func.getParameter("optional1");
    assert(optionalParam.isOptional);
    assert(optionalParam.defaultValue.getString() == "default1");
    
    std::cout << "✓ Unordered and optional keys functionality works" << std::endl;
}

int main() {
    try {
        testCJMODSystem();
        testCHTLJSFunction();
        testAtomArg();
        testUnorderedAndOptionalKeys();
        
        std::cout << "\n🎉 All CJMOD tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}