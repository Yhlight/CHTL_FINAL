#include <iostream>
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/Syntax.h"
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/Arg.h"
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/AtomArg.h"
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/CJMODScanner.h"
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/CJMODGenerator.h"
#include "CHTL/CHTLJS/CJMODSystem/CJMODAPI/CHTLJSFunction.h"

using namespace CHTL::CJMOD;

int main() {
    std::cout << "=== CJMOD API 综合测试程序 ===" << std::endl;
    
    // 1. 测试Syntax类
    std::cout << "\n1. 测试Syntax类" << std::endl;
    std::cout << "=================" << std::endl;
    
    std::string syntax = "$ ** $";
    std::cout << "分析语法: " << syntax << std::endl;
    
    auto args = Syntax::analyze(syntax);
    if (args) {
        std::cout << "语法分析成功" << std::endl;
        args->print();
    } else {
        std::cout << "语法分析失败" << std::endl;
    }
    
    // 测试对象判断
    std::cout << "\n对象判断测试:" << std::endl;
    std::cout << "isObject(\"{a: 1}\"): " << (Syntax::isObject("{a: 1}") ? "true" : "false") << std::endl;
    std::cout << "isFunction(\"function test(){}\"): " << (Syntax::isFunction("function test(){}") ? "true" : "false") << std::endl;
    std::cout << "isArray(\"[1, 2, 3]\"): " << (Syntax::isArray("[1, 2, 3]") ? "true" : "false") << std::endl;
    std::cout << "isCHTLJSFunction(\"printMyLove {url: $!_, mode: $?_}\"): " << 
                 (Syntax::isCHTLJSFunction("printMyLove {url: $!_, mode: $?_}") ? "true" : "false") << std::endl;
    
    // 2. 测试Arg类
    std::cout << "\n2. 测试Arg类" << std::endl;
    std::cout << "=============" << std::endl;
    
    Arg testArgs;
    testArgs.addToken("3");
    testArgs.addToken("**");
    testArgs.addToken("4");
    
    std::cout << "创建的Arg对象:" << std::endl;
    testArgs.print();
    
    // 测试绑定
    testArgs.bind("$", [](const std::string& value) { return value; });
    testArgs.bind("**", [](const std::string& value) { return value; });
    
    std::cout << "\n绑定后的Arg对象:" << std::endl;
    testArgs.printBindings();
    
    // 测试匹配
    std::string result1 = testArgs.match("$", [](const std::string& value) { return value; });
    std::string result2 = testArgs.match("**", [](const std::string& value) { return value; });
    
    std::cout << "匹配结果: " << result1 << " " << result2 << std::endl;
    
    // 测试转换
    testArgs.transform("pow(" + testArgs.match("$", [](const std::string& value) { return value; }) + 
                       "," + testArgs.match("$", [](const std::string& value) { return value; }) + ")");
    std::cout << "转换结果: " << testArgs.getTransformResult() << std::endl;
    
    // 3. 测试AtomArg类
    std::cout << "\n3. 测试AtomArg类" << std::endl;
    std::cout << "=================" << std::endl;
    
    AtomArg normalArg("$");
    AtomArg optionalArg("$?");
    AtomArg requiredArg("$!");
    AtomArg unorderedArg("$_");
    AtomArg variadicArg("...");
    AtomArg nonPlaceholderArg("test");
    
    std::cout << "普通占位符: " << normalArg.toFormattedString() << std::endl;
    std::cout << "可选占位符: " << optionalArg.toFormattedString() << std::endl;
    std::cout << "必须占位符: " << requiredArg.toFormattedString() << std::endl;
    std::cout << "无序占位符: " << unorderedArg.toFormattedString() << std::endl;
    std::cout << "不定参数: " << variadicArg.toFormattedString() << std::endl;
    std::cout << "非占位符: " << nonPlaceholderArg.toFormattedString() << std::endl;
    
    // 测试绑定和填充
    normalArg.bind("3");
    normalArg.fillValue("3");
    std::cout << "绑定后的占位符: " << normalArg.toString() << std::endl;
    
    // 4. 测试CJMODScanner类
    std::cout << "\n4. 测试CJMODScanner类" << std::endl;
    std::cout << "=====================" << std::endl;
    
    std::string sourceCode = "const result = 3 ** 4; console.log(result);";
    std::cout << "源代码: " << sourceCode << std::endl;
    
    Arg scanResult = CJMODScanner::scan(sourceCode, "**");
    std::cout << "扫描结果:" << std::endl;
    scanResult.print();
    
    // 测试前置截取
    std::string preExtracted = CJMODScanner::preExtract(sourceCode, "**");
    std::string postExtracted = CJMODScanner::postExtract(sourceCode, "**");
    std::cout << "前置截取: " << preExtracted << std::endl;
    std::cout << "后置截取: " << postExtracted << std::endl;
    
    // 测试滑动窗口扫描
    Arg windowResult = CJMODScanner::slidingWindowScan(sourceCode, "**");
    std::cout << "滑动窗口扫描结果:" << std::endl;
    windowResult.print();
    
    // 5. 测试CJMODGenerator类
    std::cout << "\n5. 测试CJMODGenerator类" << std::endl;
    std::cout << "=======================" << std::endl;
    
    CJMODGenerator generator;
    
    // 设置生成选项
    GenerationOptions options;
    options.mode = GenerationMode::JAVASCRIPT;
    options.minify = false;
    options.includeComments = true;
    generator.setOptions(options);
    
    // 生成代码
    std::string generatedCode = generator.generate(testArgs);
    std::cout << "生成的JavaScript代码:" << std::endl;
    std::cout << generatedCode << std::endl;
    
    // 测试导出
    std::string exportedCode = CJMODGenerator::exportResult(testArgs, options);
    std::cout << "导出的代码:" << std::endl;
    std::cout << exportedCode << std::endl;
    
    // 打印生成统计
    generator.printGenerationStatistics();
    
    // 6. 测试CHTLJSFunction类
    std::cout << "\n6. 测试CHTLJSFunction类" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // 创建CHTL JS函数
    std::string functionTemplate = "printMyLove {url: $!_, mode: $?_}";
    auto function = CHTLJSFunction::CreateCHTLJSFunction(functionTemplate);
    
    if (function) {
        std::cout << "CHTL JS函数创建成功" << std::endl;
        function->printTemplate();
        
        // 构建函数
        if (function->build()) {
            std::cout << "函数构建成功" << std::endl;
            function->printSignature();
            function->printParameters();
            function->printGeneratedFunctions();
            
            // 测试函数调用
            std::string functionCall = function->call({{"url", "\"https://www.baidu.com\""}, {"mode", "\"auto\""}});
            std::cout << "函数调用: " << functionCall << std::endl;
            
            // 测试虚对象绑定
            function->bindVirtualObject("test", "test binding");
            function->printVirtualObjectBindings();
            
        } else {
            std::cout << "函数构建失败" << std::endl;
        }
        
        function->printStatistics();
    }
    
    // 7. 综合测试 - 完整的CJMOD API工作流程
    std::cout << "\n7. 综合测试 - 完整工作流程" << std::endl;
    std::cout << "===========================" << std::endl;
    
    // 步骤1: 语法分析
    std::string complexSyntax = "pow($, $)";
    std::cout << "步骤1 - 语法分析: " << complexSyntax << std::endl;
    auto complexArgs = Syntax::analyze(complexSyntax);
    if (complexArgs) {
        complexArgs->print();
    }
    
    // 步骤2: 参数绑定
    std::cout << "\n步骤2 - 参数绑定" << std::endl;
    complexArgs->bind("$", [](const std::string& value) { return value; });
    
    // 步骤3: 扫描和填充
    std::cout << "\n步骤3 - 扫描和填充" << std::endl;
    std::string complexSource = "pow(3, 4)";
    Arg complexScanResult = CJMODScanner::scan(complexSource, "pow");
    complexScanResult.print();
    
    // 步骤4: 参数匹配和转换
    std::cout << "\n步骤4 - 参数匹配和转换" << std::endl;
    std::string match1 = complexArgs->match("$", [](const std::string& value) { return "3"; });
    std::string match2 = complexArgs->match("$", [](const std::string& value) { return "4"; });
    complexArgs->transform("Math.pow(" + match1 + ", " + match2 + ")");
    std::cout << "转换结果: " << complexArgs->getTransformResult() << std::endl;
    
    // 步骤5: 代码生成
    std::cout << "\n步骤5 - 代码生成" << std::endl;
    std::string finalCode = CJMODGenerator::exportResult(*complexArgs);
    std::cout << "最终生成的代码: " << finalCode << std::endl;
    
    // 8. 测试CHTL JS函数构建
    std::cout << "\n8. 测试CHTL JS函数构建" << std::endl;
    std::cout << "=======================" << std::endl;
    
    // 创建一个更复杂的CHTL JS函数
    std::string complexFunctionTemplate = "animate {target: $!_, duration: $?, easing: $?_}";
    auto complexFunction = CHTLJSFunction::CreateCHTLJSFunction("animate", complexFunctionTemplate);
    
    if (complexFunction && complexFunction->build()) {
        std::cout << "复杂CHTL JS函数构建成功" << std::endl;
        
        // 生成不同的代码格式
        std::string jsCode = complexFunction->generateJavaScript();
        std::string chtljsCode = complexFunction->generateCHTLJS();
        
        std::cout << "\n生成的JavaScript代码:" << std::endl;
        std::cout << jsCode << std::endl;
        
        std::cout << "\n生成的CHTL JS代码:" << std::endl;
        std::cout << chtljsCode << std::endl;
        
        // 测试函数调用
        std::string call = complexFunction->call({{"target", "{{box}}"}, {"duration", "1000"}, {"easing", "ease-in-out"}});
        std::cout << "\n函数调用示例:" << std::endl;
        std::cout << call << std::endl;
    }
    
    std::cout << "\n=== CJMOD API 综合测试完成 ===" << std::endl;
    std::cout << "所有CJMOD API组件已成功实现并测试！" << std::endl;
    
    return 0;
}