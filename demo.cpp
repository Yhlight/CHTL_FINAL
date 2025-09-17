#include <iostream>
#include "CHTL/CHTLJS/CHTLJS.h"
#include "CHTL/CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTL/CMODSystem/CMODSystem.h"
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"

int main() {
    std::cout << "=== CHTL 项目综合演示 ===" << std::endl;
    
    // 演示CHTL JS编译器
    std::cout << "\n1. CHTL JS 编译器演示" << std::endl;
    std::cout << "=====================" << std::endl;
    
    CHTLJS::CHTLJS chtljs;
    chtljs.setDebugMode(true);
    
    std::string chtljsCode = R"(
Vir test = Listen {
    click: () => {
        console.log("Hello from CHTL JS!");
        {{box}}->textContent = "Clicked!";
    }
};

const anim = Animate {
    target: {{box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0,
        transform: scale(0.8)
    },
    end: {
        opacity: 1,
        transform: scale(1.0)
    }
};
)";
    
    std::string jsOutput = chtljs.compile(chtljsCode);
    std::cout << "CHTL JS 输入:" << std::endl;
    std::cout << chtljsCode << std::endl;
    std::cout << "\n编译后的 JavaScript:" << std::endl;
    std::cout << jsOutput << std::endl;
    
    // 演示CHTL词法分析器
    std::cout << "\n2. CHTL 词法分析器演示" << std::endl;
    std::cout << "=====================" << std::endl;
    
    std::string chtlCode = R"(
html {
    head {
        title {
            text: "CHTL Demo";
        }
    }
    
    body {
        div {
            id: main;
            class: container;
            
            style {
                .container {
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }
        }
    }
}
)";
    
    CHTL::CHTLLexer chtlLexer(chtlCode);
    CHTL::CHTLTokenList tokens = chtlLexer.tokenize();
    
    std::cout << "CHTL 输入:" << std::endl;
    std::cout << chtlCode << std::endl;
    std::cout << "\n词法分析结果:" << std::endl;
    chtlLexer.printTokens();
    chtlLexer.printStatistics();
    
    // 演示CHTL语法分析器
    std::cout << "\n3. CHTL 语法分析器演示" << std::endl;
    std::cout << "=====================" << std::endl;
    
    CHTL::CHTLParser parser(chtlCode);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "语法分析成功，AST根节点类型: " << static_cast<int>(ast->type) << std::endl;
        std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
    } else {
        std::cout << "语法分析失败" << std::endl;
    }
    
    // 演示CHTL代码生成器
    std::cout << "\n4. CHTL 代码生成器演示" << std::endl;
    std::cout << "=====================" << std::endl;
    
    CHTL::CHTLGenerator generator;
    generator.setDebugMode(true);
    
    if (ast) {
        std::string htmlOutput = generator.generate(ast);
        std::cout << "生成的 HTML:" << std::endl;
        std::cout << htmlOutput << std::endl;
    }
    
    // 演示CHTL上下文管理器
    std::cout << "\n5. CHTL 上下文管理器演示" << std::endl;
    std::cout << "=======================" << std::endl;
    
    CHTL::CHTLContext context(CHTL::ContextType::GLOBAL, "demo");
    
    // 添加模板
    CHTL::TemplateInfo templateInfo;
    templateInfo.name = "Button";
    templateInfo.type = "@Element";
    templateInfo.isPublic = true;
    context.addTemplate(templateInfo);
    
    // 添加变量
    context.setVariable("appName", "CHTL Demo", "string");
    context.setVariable("version", "1.0.0", "string");
    
    // 添加命名空间
    CHTL::NamespaceInfo namespaceInfo;
    namespaceInfo.name = "ui";
    namespaceInfo.prefix = "ui";
    context.addNamespace(namespaceInfo);
    
    std::cout << "上下文创建完成" << std::endl;
    context.printTemplates();
    context.printVariables();
    context.printNamespaces();
    
    // 演示CMOD模块系统
    std::cout << "\n6. CMOD 模块系统演示" << std::endl;
    std::cout << "===================" << std::endl;
    
    CHTL::CMODSystem cmodSystem;
    cmodSystem.setDebugMode(true);
    
    // 创建模块信息
    CHTL::CMODInfo moduleInfo;
    moduleInfo.name = "demo-module";
    moduleInfo.version = "1.0.0";
    moduleInfo.description = "CHTL演示模块";
    moduleInfo.author = "CHTL Team";
    moduleInfo.license = "MIT";
    moduleInfo.type = CHTL::CMODType::CHTL_MODULE;
    moduleInfo.mainFile = "main.chtl";
    
    // 注册模块
    if (cmodSystem.registerModule(moduleInfo)) {
        std::cout << "模块注册成功: " << moduleInfo.name << std::endl;
    }
    
    cmodSystem.printModules();
    cmodSystem.printStatistics();
    
    // 演示统一扫描器
    std::cout << "\n7. 统一扫描器演示" << std::endl;
    std::cout << "=================" << std::endl;
    
    std::string mixedCode = R"(
html {
    head {
        title {
            text: "Mixed Content Demo";
        }
        
        style {
            .container {
                width: 100%;
                max-width: 1200px;
                margin: 0 auto;
                padding: 20px;
            }
        }
    }
    
    body {
        div {
            id: main;
            class: container;
            
            script {
                console.log("Hello from JavaScript!");
                document.getElementById('main').style.backgroundColor = 'lightblue';
            }
        }
    }
}
)";
    
    CHTL::UnifiedScanner scanner;
    scanner.setDebugMode(true);
    
    std::vector<CHTL::CodeFragment> fragments = scanner.scan(mixedCode);
    
    std::cout << "混合代码输入:" << std::endl;
    std::cout << mixedCode << std::endl;
    std::cout << "\n代码片段分离结果:" << std::endl;
    scanner.printFragments();
    scanner.printStatistics();
    
    // 演示编译器调度器
    std::cout << "\n8. 编译器调度器演示" << std::endl;
    std::cout << "=================" << std::endl;
    
    CHTL::CompilerDispatcher dispatcher;
    dispatcher.setDebugMode(true);
    
    CHTL::CompilationContext compContext;
    compContext.debugMode = true;
    compContext.minifyOutput = false;
    dispatcher.setContext(compContext);
    
    std::vector<CHTL::CompilationResult> results = dispatcher.compile(mixedCode);
    
    std::cout << "编译结果数量: " << results.size() << std::endl;
    dispatcher.printResults();
    dispatcher.printStatistics();
    
    // 演示代码合并器
    std::cout << "\n9. 代码合并器演示" << std::endl;
    std::cout << "===============" << std::endl;
    
    CHTL::CodeMerger merger;
    CHTL::MergeOptions mergeOptions;
    mergeOptions.format = CHTL::OutputFormat::HTML;
    mergeOptions.includeComments = true;
    mergeOptions.minifyOutput = false;
    merger.setOptions(mergeOptions);
    
    CHTL::MergeResult mergeResult = merger.merge(results);
    
    if (mergeResult.success) {
        std::cout << "代码合并成功" << std::endl;
        std::cout << "HTML长度: " << mergeResult.html.length() << " 字符" << std::endl;
        std::cout << "CSS长度: " << mergeResult.css.length() << " 字符" << std::endl;
        std::cout << "JS长度: " << mergeResult.js.length() << " 字符" << std::endl;
    } else {
        std::cout << "代码合并失败: " << mergeResult.errorMessage << std::endl;
    }
    
    std::cout << "\n=== 综合演示完成 ===" << std::endl;
    std::cout << "CHTL项目核心功能已全部实现并演示完毕！" << std::endl;
    
    return 0;
}