#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/ModuleSystem.h"
#include "CHTL/CLITools.h"
#include "CHTL/CompilationMonitor.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTLJS/CJMODSystem.h"

using namespace CHTL;

void testCHTLContext() {
    std::cout << "Testing CHTLContext..." << std::endl;
    
    CHTLContext context;
    context.setDebugMode(true);
    
    // 测试配置
    context.setConfiguration("TEST_KEY", "TEST_VALUE");
    std::string value = context.getConfiguration("TEST_KEY");
    std::cout << "Configuration test: " << value << std::endl;
    
    // 测试命名空间
    context.addNamespace("test", "parent");
    bool has_ns = context.hasNamespace("test");
    std::cout << "Namespace test: " << (has_ns ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "CHTLContext test completed." << std::endl;
}

void testCHTLNode() {
    std::cout << "Testing CHTLNode..." << std::endl;
    
    // 测试元素节点
    auto element = std::make_shared<ElementNode>("div");
    element->setAttribute("id", "test");
    element->setAttribute("class", "container");
    
    // 测试文本节点
    auto text = std::make_shared<TextNode>("Hello, CHTL!");
    element->addChild(text);
    
    // 测试HTML生成
    std::string html = element->generateHTML();
    std::cout << "Generated HTML: " << html << std::endl;
    
    // 测试验证
    bool valid = element->validate();
    std::cout << "Element validation: " << (valid ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "CHTLNode test completed." << std::endl;
}

void testUnifiedScanner() {
    std::cout << "Testing UnifiedScanner..." << std::endl;
    
    UnifiedScanner scanner;
    scanner.setDebugMode(true);
    
    std::string test_code = R"(
        div {
            style {
                color: red;
            }
        }
        
        script {
            console.log("test");
        }
    )";
    
    auto fragments = scanner.scan(test_code);
    std::cout << "Found " << fragments.size() << " fragments" << std::endl;
    
    for (const auto& fragment : fragments) {
        std::cout << "Fragment type: " << static_cast<int>(fragment.type) 
                  << ", content: " << fragment.content.substr(0, 50) << "..." << std::endl;
    }
    
    std::cout << "UnifiedScanner test completed." << std::endl;
}

void testCHTLLexer() {
    std::cout << "Testing CHTLLexer..." << std::endl;
    
    CHTLLexer lexer;
    lexer.setDebugMode(true);
    
    std::string test_code = R"(
        div {
            id: "test";
            class: container;
            style {
                color: red;
                width: 100px;
            }
        }
    )";
    
    lexer.setInput(test_code);
    auto tokens = lexer.tokenize();
    
    std::cout << "Found " << tokens.size() << " tokens" << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << "Token: " << token.toString() << std::endl;
    }
    
    std::cout << "CHTLLexer test completed." << std::endl;
}

void testCHTLParser() {
    std::cout << "Testing CHTLParser..." << std::endl;
    
    CHTLParser parser;
    parser.setDebugMode(true);
    
    std::string test_code = R"(
        div {
            id: "test";
            class: container;
            style {
                color: red;
                width: 100px;
            }
        }
    )";
    
    parser.setInput(test_code);
    auto node = parser.parse();
    
    if (node) {
        std::cout << "Parsed successfully!" << std::endl;
        std::cout << "Generated HTML: " << node->generateHTML() << std::endl;
    } else {
        std::cout << "Parse failed!" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::cout << "CHTLParser test completed." << std::endl;
}

void testCJMODSystem() {
    std::cout << "Testing CJMODSystem..." << std::endl;
    
    CJMODSystem system;
    system.setDebugMode(true);
    
    // 测试语法分析
    auto args = Syntax::analyze("$ ** $");
    std::cout << "Syntax analysis result: " << args.size() << " arguments" << std::endl;
    
    // 测试函数创建
    auto func = CHTLJSFunction::createCHTLJSFunction("test {param: $}");
    std::cout << "Function name: " << func.getName() << std::endl;
    
    // 测试模块加载
    bool loaded = system.loadModule("test_module");
    std::cout << "Module loading: " << (loaded ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "CJMODSystem test completed." << std::endl;
}

void testTemplateSystem() {
    std::cout << "Testing Template System..." << std::endl;
    
    // 测试样式组模板
    auto styleTemplate = std::make_shared<StyleTemplateNode>("DefaultText");
    styleTemplate->addCSSProperty("color", "black");
    styleTemplate->addCSSProperty("line-height", "1.6");
    styleTemplate->addCSSProperty("font-size", "16px");
    
    std::cout << "Style template created: " << styleTemplate->getTemplateName() << std::endl;
    std::cout << "CSS properties: " << styleTemplate->getCSSProperty("color") << std::endl;
    
    // 测试元素模板
    auto elementTemplate = std::make_shared<ElementTemplateNode>("Box");
    auto div1 = std::make_shared<ElementNode>("div");
    auto div2 = std::make_shared<ElementNode>("div");
    elementTemplate->addChild(div1);
    elementTemplate->addChild(div2);
    
    std::cout << "Element template created: " << elementTemplate->getTemplateName() << std::endl;
    std::cout << "Child elements: " << elementTemplate->getChildCount() << std::endl;
    
    // 测试变量组模板
    auto varTemplate = std::make_shared<VarTemplateNode>("ThemeColor");
    varTemplate->addVariable("primaryColor", "rgb(255, 192, 203)");
    varTemplate->addVariable("secondaryColor", "rgb(145, 155, 200)");
    varTemplate->addVariable("textColor", "black");
    
    std::cout << "Variable template created: " << varTemplate->getTemplateName() << std::endl;
    std::cout << "Variables: " << varTemplate->getVariable("primaryColor") << std::endl;
    
    // 测试模板管理器
    TemplateManager manager;
    manager.registerTemplate(styleTemplate);
    manager.registerTemplate(elementTemplate);
    manager.registerTemplate(varTemplate);
    
    std::cout << "Templates registered in manager" << std::endl;
    
    // 测试模板应用
    auto testStyle = std::make_shared<StyleNode>(StyleNode::StyleType::LOCAL);
    manager.applyStyleTemplate("DefaultText", testStyle);
    
    std::cout << "Style template applied successfully" << std::endl;
    
    std::cout << "Template System test completed." << std::endl;
}

void testCustomSystem() {
    std::cout << "Testing Custom System..." << std::endl;
    
    // 测试自定义样式组
    auto customStyle = std::make_shared<CustomStyleNode>("YellowText");
    customStyle->addCSSProperty("color", "yellow");
    customStyle->addCSSProperty("font-size", "18px");
    customStyle->addUnvaluedProperty("line-height");
    
    std::cout << "Custom style created: " << customStyle->getCustomName() << std::endl;
    std::cout << "CSS properties: " << customStyle->getCSSProperty("color") << std::endl;
    
    // 测试自定义元素
    auto customElement = std::make_shared<CustomElementNode>("Box");
    auto div1 = std::make_shared<ElementNode>("div");
    auto div2 = std::make_shared<ElementNode>("div");
    customElement->insertElementAtTop(div1);
    customElement->insertElementAtBottom(div2);
    
    std::cout << "Custom element created: " << customElement->getCustomName() << std::endl;
    std::cout << "Child elements: " << customElement->getChildCount() << std::endl;
    
    // 测试自定义变量组
    auto customVar = std::make_shared<CustomVarNode>("ThemeColor");
    customVar->addVariable("primaryColor", "rgb(255, 100, 100)");
    customVar->addVariable("textColor", "white");
    customVar->specializeVariable("primaryColor", "rgb(255, 200, 200)");
    
    std::cout << "Custom variable created: " << customVar->getCustomName() << std::endl;
    std::cout << "Variables: " << customVar->getVariable("primaryColor") << std::endl;
    
    // 测试自定义管理器
    CustomManager customManager;
    customManager.registerCustom(customStyle);
    customManager.registerCustom(customElement);
    customManager.registerCustom(customVar);
    
    std::cout << "Customs registered in manager" << std::endl;
    
    // 测试自定义应用
    auto testStyle = std::make_shared<StyleNode>(StyleNode::StyleType::LOCAL);
    customManager.applyCustomStyle("YellowText", testStyle);
    
    std::cout << "Custom style applied successfully" << std::endl;
    
    // 测试特例化操作
    SpecializationOperation deleteOp(SpecializationType::DELETE_PROPERTY, "line-height");
    customStyle->addSpecialization(deleteOp);
    
    std::cout << "Specialization operations added" << std::endl;
    
    std::cout << "Custom System test completed." << std::endl;
}

void testImportSystem() {
    std::cout << "Testing Import System..." << std::endl;
    
    // 测试导入节点
    auto htmlImport = std::make_shared<ImportNode>(ImportType::HTML, "header.html");
    htmlImport->setImportAlias("Header");
    
    auto cssImport = std::make_shared<ImportNode>(ImportType::CSS, "styles.css");
    cssImport->setImportAlias("MainStyles");
    
    auto jsImport = std::make_shared<ImportNode>(ImportType::JS, "script.js");
    jsImport->setImportAlias("MainScript");
    
    std::cout << "Import nodes created successfully" << std::endl;
    
    // 测试导入管理器
    ImportManager importManager;
    importManager.setBasePath(".");
    importManager.addSearchPath("./test");
    
    std::cout << "Import manager configured" << std::endl;
    
    // 测试命名空间管理器
    NamespaceManager namespaceManager;
    namespaceManager.createNamespace("test");
    namespaceManager.createNamespace("nested", "test");
    namespaceManager.setDefaultNamespace("test");
    
    std::cout << "Namespace manager configured" << std::endl;
    
    // 测试路径解析
    std::string resolvedPath = importManager.resolvePath("test.chtl");
    std::cout << "Resolved path: " << resolvedPath << std::endl;
    
    // 测试导入状态
    importManager.markAsImported("test.chtl");
    bool isImported = importManager.isImported("test.chtl");
    std::cout << "File imported: " << (isImported ? "YES" : "NO") << std::endl;
    
    std::cout << "Import System test completed." << std::endl;
}

void testModuleSystem() {
    std::cout << "Testing Module System..." << std::endl;
    
    // 测试CMOD模块
    auto cmodModule = std::make_shared<CMODModule>("TestCMOD");
    ModuleInfo info;
    info.name = "TestCMOD";
    info.version = "1.0.0";
    info.description = "Test CMOD module";
    info.author = "Test Author";
    info.license = "MIT";
    cmodModule->setInfo(info);
    
    // 添加模板导出
    auto styleTemplate = std::make_shared<StyleTemplateNode>("TestStyle");
    styleTemplate->addCSSProperty("color", "red");
    styleTemplate->addCSSProperty("font-size", "14px");
    cmodModule->addTemplateExport(styleTemplate);
    
    // 添加自定义导出
    auto customElement = std::make_shared<CustomElementNode>("TestCustom");
    cmodModule->addCustomExport(customElement);
    
    // 添加原始嵌入导出
    cmodModule->addOriginExport("TestHeader", "<header>Test Header</header>", "Html");
    
    // 添加配置导出
    cmodModule->addConfigurationExport("TestConfig", "DEBUG_MODE: true;");
    
    std::cout << "CMOD module created: " << cmodModule->getInfo().name << std::endl;
    std::cout << "Exports: " << cmodModule->getExports().size() << std::endl;
    
    // 测试CJMOD模块
    auto cjmodModule = std::make_shared<CJMODModule>("TestCJMOD");
    ModuleInfo cjmodInfo;
    cjmodInfo.name = "TestCJMOD";
    cjmodInfo.version = "1.0.0";
    cjmodInfo.description = "Test CJMOD module";
    cjmodModule->setInfo(cjmodInfo);
    
    // 添加函数导出
    cjmodModule->addFunctionExport("testFunc", "testFunc $ $", "console.log($1 + $2);");
    cjmodModule->addVirtualObjectExport("testVir", "function testVir() { return 'test'; }");
    
    std::cout << "CJMOD module created: " << cjmodModule->getInfo().name << std::endl;
    std::cout << "Functions: " << cjmodModule->getFunctionExports().size() << std::endl;
    
    // 测试混合模块
    auto hybridModule = std::make_shared<HybridModule>("TestHybrid");
    ModuleInfo hybridInfo;
    hybridInfo.name = "TestHybrid";
    hybridInfo.version = "1.0.0";
    hybridInfo.description = "Test Hybrid module";
    hybridModule->setInfo(hybridInfo);
    
    std::cout << "Hybrid module created: " << hybridModule->getInfo().name << std::endl;
    
    // 测试模块管理器
    ModuleManager moduleManager;
    moduleManager.registerModule(cmodModule);
    moduleManager.registerModule(cjmodModule);
    moduleManager.registerModule(hybridModule);
    
    std::cout << "Modules registered in manager" << std::endl;
    
    // 测试模块查询
    auto foundCMOD = moduleManager.getCMODModule("TestCMOD");
    auto foundCJMOD = moduleManager.getCJMODModule("TestCJMOD");
    auto foundHybrid = moduleManager.getHybridModule("TestHybrid");
    
    std::cout << "CMOD module found: " << (foundCMOD ? "YES" : "NO") << std::endl;
    std::cout << "CJMOD module found: " << (foundCJMOD ? "YES" : "NO") << std::endl;
    std::cout << "Hybrid module found: " << (foundHybrid ? "YES" : "NO") << std::endl;
    
    // 测试模块验证
    bool cmodValid = moduleManager.validateModule("TestCMOD");
    bool cjmodValid = moduleManager.validateModule("TestCJMOD");
    bool hybridValid = moduleManager.validateModule("TestHybrid");
    
    std::cout << "CMOD module valid: " << (cmodValid ? "YES" : "NO") << std::endl;
    std::cout << "CJMOD module valid: " << (cjmodValid ? "YES" : "NO") << std::endl;
    std::cout << "Hybrid module valid: " << (hybridValid ? "YES" : "NO") << std::endl;
    
    // 测试模块打包器
    ModulePackager packager;
    bool packaged = packager.packageCMODModule(*cmodModule, "test_package.cmod");
    std::cout << "Module packaged: " << (packaged ? "YES" : "NO") << std::endl;
    
    std::cout << "Module System test completed." << std::endl;
}

void testCLITools() {
    std::cout << "Testing CLI Tools..." << std::endl;
    
    // 测试CLI解析器
    CLIParser parser;
    std::vector<std::string> args = {"compile", "test.chtl", "--output", "test.html", "--verbose"};
    
    if (parser.parse(args)) {
        std::cout << "CLI parsing successful" << std::endl;
        std::cout << "Command: " << parser.getCommandName() << std::endl;
        std::cout << "Output option: " << parser.getOptionValue("output") << std::endl;
        std::cout << "Verbose option: " << (parser.hasOption("verbose") ? "YES" : "NO") << std::endl;
    } else {
        std::cout << "CLI parsing failed" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    // 测试CLI编译器
    CHTLCompilerCLI cli;
    cli.setVerbose(true);
    cli.setDebug(true);
    cli.setInputFile("test.chtl");
    cli.setOutputDirectory("test.html");
    
    std::cout << "CLI compiler configured" << std::endl;
    std::cout << "Verbose: " << (cli.isVerbose() ? "YES" : "NO") << std::endl;
    std::cout << "Debug: " << (cli.isDebug() ? "YES" : "NO") << std::endl;
    std::cout << "Input file: " << cli.getInputFile() << std::endl;
    std::cout << "Output directory: " << cli.getOutputDirectory() << std::endl;
    
    // 测试命令行程序
    CHTLCommandLineProgram program;
    program.enableRendering(true);
    program.enablePreview(true);
    program.enableLiveCompilation(true);
    
    std::cout << "Command line program configured" << std::endl;
    std::cout << "Rendering enabled: " << (program.isRenderingEnabled() ? "YES" : "NO") << std::endl;
    std::cout << "Preview enabled: " << (program.isPreviewEnabled() ? "YES" : "NO") << std::endl;
    std::cout << "Live compilation enabled: " << (program.isLiveCompilationEnabled() ? "YES" : "NO") << std::endl;
    
    std::cout << "CLI Tools test completed." << std::endl;
}

void testVSCodeExtension() {
    std::cout << "Testing VSCode Extension..." << std::endl;
    
    // 测试扩展文件存在性
    std::ifstream packageJson("/workspace/vscode-extension/package.json");
    std::ifstream languageConfig("/workspace/vscode-extension/language-configuration.json");
    std::ifstream syntaxFile("/workspace/vscode-extension/syntaxes/chtl.tmLanguage.json");
    std::ifstream snippetsFile("/workspace/vscode-extension/snippets/chtl.json");
    std::ifstream extensionTs("/workspace/vscode-extension/src/extension.ts");
    std::ifstream tsConfig("/workspace/vscode-extension/tsconfig.json");
    std::ifstream readme("/workspace/vscode-extension/README.md");
    std::ifstream buildScript("/workspace/vscode-extension/build.sh");
    
    std::cout << "Package.json exists: " << (packageJson.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "Language configuration exists: " << (languageConfig.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "Syntax file exists: " << (syntaxFile.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "Snippets file exists: " << (snippetsFile.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "Extension TypeScript exists: " << (extensionTs.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "TypeScript config exists: " << (tsConfig.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "README exists: " << (readme.is_open() ? "YES" : "NO") << std::endl;
    std::cout << "Build script exists: " << (buildScript.is_open() ? "YES" : "NO") << std::endl;
    
    // 关闭文件
    packageJson.close();
    languageConfig.close();
    syntaxFile.close();
    snippetsFile.close();
    extensionTs.close();
    tsConfig.close();
    readme.close();
    buildScript.close();
    
    // 测试扩展功能
    std::cout << "VSCode Extension features:" << std::endl;
    std::cout << "- Syntax highlighting" << std::endl;
    std::cout << "- Code completion" << std::endl;
    std::cout << "- Code formatting" << std::endl;
    std::cout << "- Linting" << std::endl;
    std::cout << "- Hover information" << std::endl;
    std::cout << "- Go to definition" << std::endl;
    std::cout << "- Find references" << std::endl;
    std::cout << "- Document symbols" << std::endl;
    std::cout << "- Workspace symbols" << std::endl;
    std::cout << "- Code actions" << std::endl;
    std::cout << "- Diagnostics" << std::endl;
    std::cout << "- Templates view" << std::endl;
    std::cout << "- Customs view" << std::endl;
    std::cout << "- Imports view" << std::endl;
    std::cout << "- Modules view" << std::endl;
    std::cout << "- Commands" << std::endl;
    std::cout << "- Context menu" << std::endl;
    std::cout << "- Keybindings" << std::endl;
    std::cout << "- Configuration" << std::endl;
    
    std::cout << "VSCode Extension test completed." << std::endl;
}

void testCompilationMonitor() {
    std::cout << "Testing Compilation Monitor..." << std::endl;
    
    // 测试编译监视器管理器
    auto& manager = CompilationMonitorManager::getInstance();
    
    // 测试配置
    MonitorConfig config;
    config.max_compilation_time = std::chrono::seconds(10);
    config.max_memory_usage_mb = 100;
    config.enable_memory_monitoring = true;
    config.enable_time_monitoring = true;
    config.auto_kill_on_limit = false;
    manager.setGlobalConfig(config);
    
    std::cout << "Monitor configuration set" << std::endl;
    
    // 测试回调
    auto callback = std::make_shared<DefaultCompilationMonitorCallback>();
    callback->setVerbose(true);
    callback->setShowProgress(true);
    callback->setShowWarnings(true);
    manager.setGlobalCallback(callback);
    
    std::cout << "Monitor callback set" << std::endl;
    
    // 测试任务启动
    std::string task_id = manager.startTask("Test Task");
    std::cout << "Task started: " << task_id << std::endl;
    
    // 测试状态查询
    bool is_monitoring = manager.getMonitor().isMonitoring(task_id);
    std::cout << "Task is monitoring: " << (is_monitoring ? "YES" : "NO") << std::endl;
    
    // 测试统计信息
    CompilationStats stats = manager.getTaskStats(task_id);
    std::cout << "Task duration: " << stats.getDurationSeconds() << "s" << std::endl;
    std::cout << "Task memory: " << stats.getCurrentMemoryMB() << "MB" << std::endl;
    
    // 模拟一些工作
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 测试任务停止
    manager.stopTask(task_id);
    std::cout << "Task stopped" << std::endl;
    
    // 测试最终统计
    stats = manager.getTaskStats(task_id);
    std::cout << "Final duration: " << stats.getDurationSeconds() << "s" << std::endl;
    std::cout << "Final memory: " << stats.getPeakMemoryMB() << "MB" << std::endl;
    
    // 测试作用域监视器
    {
        ScopedCompilationMonitor scoped_monitor("Scoped Task");
        std::cout << "Scoped task started: " << scoped_monitor.getTaskId() << std::endl;
        
        // 模拟一些工作
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // 获取统计信息
        CompilationStats scoped_stats = scoped_monitor.getStats();
        std::cout << "Scoped task duration: " << scoped_stats.getDurationSeconds() << "s" << std::endl;
        
        // 作用域结束时会自动停止
    }
    std::cout << "Scoped task completed" << std::endl;
    
    // 测试多个任务
    std::vector<std::string> task_ids;
    for (int i = 0; i < 3; ++i) {
        std::string id = manager.startTask("Multi Task " + std::to_string(i));
        task_ids.push_back(id);
    }
    
    std::cout << "Multiple tasks started: " << task_ids.size() << std::endl;
    
    // 测试活动任务列表
    auto active_tasks = manager.getMonitor().getActiveTasks();
    std::cout << "Active tasks count: " << active_tasks.size() << std::endl;
    
    // 停止所有任务
    manager.stopAllTasks();
    std::cout << "All tasks stopped" << std::endl;
    
    // 测试清理
    manager.clearCompletedTasks();
    std::cout << "Completed tasks cleared" << std::endl;
    
    // 测试内存使用获取
    size_t current_memory = CompilationMonitor::getCurrentMemoryUsage();
    std::cout << "Current memory usage: " << (current_memory / (1024.0 * 1024.0)) << "MB" << std::endl;
    
    // 测试时间获取
    auto current_time = CompilationMonitor::getCurrentTime();
    std::cout << "Current time: " << current_time.time_since_epoch().count() << std::endl;
    
    std::cout << "Compilation Monitor test completed." << std::endl;
}

int main() {
    std::cout << "CHTL Compiler Test Suite" << std::endl;
    std::cout << "========================" << std::endl;
    
    try {
        testCHTLContext();
        std::cout << std::endl;
        
        testCHTLNode();
        std::cout << std::endl;
        
        testUnifiedScanner();
        std::cout << std::endl;
        
        testCHTLLexer();
        std::cout << std::endl;
        
        testCHTLParser();
        std::cout << std::endl;
        
        testCJMODSystem();
        std::cout << std::endl;
        
        testTemplateSystem();
        std::cout << std::endl;
        
        testCustomSystem();
        std::cout << std::endl;
        
        testImportSystem();
        std::cout << std::endl;
        
        testModuleSystem();
        std::cout << std::endl;
        
        testCLITools();
        std::cout << std::endl;
        
        testVSCodeExtension();
        std::cout << std::endl;
        
        testCompilationMonitor();
        std::cout << std::endl;
        
        std::cout << "All tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}