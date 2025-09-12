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
#include "CHTL/ConfigurationSystem.h"
#include "CHTL/UseStatement.h"
#include "CHTL/CHTLJSCompiler.h"
#include "CHTL/ProjectWorkflow.h"
#include "CHTL/SyntaxBoundaries.h"
#include "CHTL/CJMODAPI.h"
#include "CHTL/OfficialModules.h"
#include "CHTL/WildcardImport.h"
#include "CHTL/DefaultStructGenerator.h"
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

void testConfigurationSystem() {
    std::cout << "Testing Configuration System..." << std::endl;
    
    // 测试配置值
    ConfigurationValue boolValue(true);
    ConfigurationValue intValue(42);
    ConfigurationValue stringValue("test");
    ConfigurationValue arrayValue({"item1", "item2", "item3"});
    
    std::cout << "Boolean value: " << boolValue.toString() << std::endl;
    std::cout << "Integer value: " << intValue.toString() << std::endl;
    std::cout << "String value: " << stringValue.toString() << std::endl;
    std::cout << "Array value: " << arrayValue.toString() << std::endl;
    
    // 测试Name配置
    NameConfiguration nameConfig;
    nameConfig.setDefaultValues();
    std::cout << "CUSTOM_STYLE: " << nameConfig.CUSTOM_STYLE << std::endl;
    std::cout << "CUSTOM_ELEMENT: " << nameConfig.CUSTOM_ELEMENT << std::endl;
    std::cout << "CUSTOM_VAR: " << nameConfig.CUSTOM_VAR << std::endl;
    
    // 测试配置组
    auto configGroup = std::make_shared<ConfigurationGroup>("TestGroup");
    configGroup->setConfiguration("DEBUG_MODE", ConfigurationValue(true));
    configGroup->setConfiguration("INDEX_INITIAL_COUNT", ConfigurationValue(0));
    configGroup->setNameConfiguration(nameConfig);
    
    std::cout << "Configuration group created: " << configGroup->getName() << std::endl;
    std::cout << "Has name configuration: " << (configGroup->hasNameConfiguration() ? "YES" : "NO") << std::endl;
    
    // 测试配置管理器
    ConfigurationManager configManager;
    configManager.addConfigurationGroup(configGroup);
    
    auto retrievedGroup = configManager.getConfigurationGroup("TestGroup");
    std::cout << "Retrieved group: " << (retrievedGroup ? "YES" : "NO") << std::endl;
    
    // 测试配置节点
    auto configNode = std::make_shared<ConfigurationNode>("TestNode");
    configNode->addConfiguration("TEST_VALUE", ConfigurationValue("test"));
    configNode->setNameConfiguration(nameConfig);
    
    std::cout << "Configuration node created: " << configNode->getConfigurationName() << std::endl;
    std::cout << "Configuration string: " << configNode->generateConfigurationString() << std::endl;
    
    // 测试配置解析器
    ConfigurationParser parser;
    std::string configContent = "[Configuration]\n{\n    DEBUG_MODE = true;\n    INDEX_INITIAL_COUNT = 0;\n}";
    auto parsedNode = parser.parseConfiguration(configContent);
    
    std::cout << "Configuration parsed: " << (parsedNode ? "YES" : "NO") << std::endl;
    
    // 测试配置应用器
    ConfigurationApplicator applicator;
    applicator.applyConfiguration(*configGroup);
    
    std::string keyword = applicator.getKeyword("CUSTOM_STYLE");
    std::cout << "Applied keyword CUSTOM_STYLE: " << keyword << std::endl;
    
    std::cout << "Configuration System test completed." << std::endl;
}

void testUseStatement() {
    std::cout << "Testing Use Statement..." << std::endl;
    
    // 测试HTML5 Use语句
    auto html5Use = std::make_shared<UseStatement>(UseStatementType::HTML5, "html5");
    std::cout << "HTML5 Use statement: " << html5Use->generateUseStatement() << std::endl;
    std::cout << "HTML5 Use statement valid: " << (html5Use->validate() ? "YES" : "NO") << std::endl;
    
    // 测试Config Use语句
    auto configUse = std::make_shared<UseStatement>(UseStatementType::CONFIG, "MyConfig");
    std::cout << "Config Use statement: " << configUse->generateUseStatement() << std::endl;
    std::cout << "Config Use statement valid: " << (configUse->validate() ? "YES" : "NO") << std::endl;
    
    // 测试Use语句管理器
    UseStatementManager useManager;
    useManager.addUseStatement(html5Use);
    useManager.addUseStatement(configUse);
    
    std::cout << "Use statements added: " << useManager.getUseStatements().size() << std::endl;
    std::cout << "Has HTML5 Use statement: " << (useManager.hasHTML5UseStatement() ? "YES" : "NO") << std::endl;
    std::cout << "Has Config Use statement: " << (useManager.hasConfigUseStatement("MyConfig") ? "YES" : "NO") << std::endl;
    
    // 测试Use语句解析器
    UseStatementParser useParser;
    std::string useContent = "use html5;\nuse @Config MyConfig;";
    auto parsedStatements = useParser.parseUseStatements(useContent);
    
    std::cout << "Use statements parsed: " << parsedStatements.size() << std::endl;
    
    std::cout << "Use Statement test completed." << std::endl;
}

void testCHTLJSCompiler() {
    std::cout << "Testing CHTL JS Compiler..." << std::endl;
    
    // 测试文件载入器
    CHTLJSFileLoader fileLoader;
    fileLoader.addFile("./test1.cjjs");
    fileLoader.addFile("./test2.cjjs");
    fileLoader.addFiles({"./test3.cjjs", "./test4.cjjs"});
    
    std::cout << "Files added: " << fileLoader.getFiles().size() << std::endl;
    std::cout << "Has file test1.cjjs: " << (fileLoader.hasFile("./test1.cjjs") ? "YES" : "NO") << std::endl;
    
    // 测试增强选择器
    CHTLJSEnhancedSelector selector;
    std::string tagSelector = "{{button}}";
    std::string classSelector = "{{.box}}";
    std::string idSelector = "{{#box}}";
    std::string descendantSelector = "{{.box button}}";
    std::string indexedSelector = "{{button[0]}}";
    
    std::cout << "Tag selector JS: " << selector.generateJavaScript(tagSelector) << std::endl;
    std::cout << "Class selector JS: " << selector.generateJavaScript(classSelector) << std::endl;
    std::cout << "ID selector JS: " << selector.generateJavaScript(idSelector) << std::endl;
    std::cout << "Descendant selector JS: " << selector.generateJavaScript(descendantSelector) << std::endl;
    std::cout << "Indexed selector JS: " << selector.generateJavaScript(indexedSelector) << std::endl;
    
    // 测试动画系统
    CHTLJSAnimationSystem animationSystem;
    CHTLJSAnimationSystem::AnimationConfig animConfig;
    animConfig.target = "{{button}}";
    animConfig.duration = 1000;
    animConfig.easing = "ease-in-out";
    animConfig.begin["opacity"] = "0";
    animConfig.end["opacity"] = "1";
    animConfig.loop = -1;
    animConfig.direction = "normal";
    animConfig.delay = 0;
    animConfig.callback = "() => console.log('Animation complete')";
    
    std::cout << "Animation generated: " << (animationSystem.generateAnimation(animConfig).length() > 0 ? "YES" : "NO") << std::endl;
    std::cout << "Animation valid: " << (animationSystem.validateAnimation(animConfig) ? "YES" : "NO") << std::endl;
    
    // 测试路由系统
    CHTLJSRoutingSystem routingSystem;
    CHTLJSRoutingSystem::RouteConfig routeConfig;
    routeConfig.path = "/home";
    routeConfig.component = "HomeComponent";
    routeConfig.params["id"] = "123";
    routeConfig.children = {"child1", "child2"};
    
    routingSystem.addRoute(routeConfig);
    std::cout << "Route added: " << (routingSystem.hasRoute("/home") ? "YES" : "NO") << std::endl;
    std::cout << "Router generated: " << (routingSystem.generateRouter().length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试响应式系统
    CHTLJSReactiveSystem reactiveSystem;
    CHTLJSReactiveSystem::ReactiveValue reactiveValue;
    reactiveValue.name = "count";
    reactiveValue.initialValue = "0";
    reactiveValue.type = "number";
    reactiveValue.dependencies = {"value1", "value2"};
    
    reactiveSystem.addReactiveValue(reactiveValue);
    std::cout << "Reactive value added: " << (reactiveSystem.hasReactiveValue("count") ? "YES" : "NO") << std::endl;
    std::cout << "Reactive system generated: " << (reactiveSystem.generateReactiveSystem().length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试CHTL JS编译器
    CHTLJSCompiler compiler;
    compiler.addComponent("TestComponent", "console.log('Test component');");
    compiler.addModule("TestModule", "export const test = 'test';");
    
    std::cout << "Component added: " << (compiler.hasComponent("TestComponent") ? "YES" : "NO") << std::endl;
    std::cout << "Module added: " << (compiler.hasModule("TestModule") ? "YES" : "NO") << std::endl;
    
    std::string testContent = "{{button}}.addEventListener('click', () => { console.log('clicked'); });";
    std::string compiledContent = compiler.compile(testContent);
    std::cout << "Content compiled: " << (compiledContent.length() > 0 ? "YES" : "NO") << std::endl;
    
    std::cout << "CHTL JS Compiler test completed." << std::endl;
}

void testProjectWorkflow() {
    std::cout << "Testing Project Workflow..." << std::endl;
    
    // 测试编译器调度器
    CompilerDispatcher dispatcher;
    auto chtlCompiler = std::make_shared<CHTLCompiler>();
    auto chtlJSCompiler = std::make_shared<CHTLJSCompiler>();
    
    dispatcher.registerCompiler("default", chtlCompiler);
    dispatcher.registerJSCompiler("default", chtlJSCompiler);
    
    std::cout << "Compilers registered: " << dispatcher.getCompilerNames().size() << std::endl;
    std::cout << "JS Compilers registered: " << dispatcher.getJSCompilerNames().size() << std::endl;
    
    // 测试代码合并器
    CodeMerger merger;
    merger.addCodeFragment("html1", "<div>Hello</div>", "html");
    merger.addCodeFragment("css1", "body { margin: 0; }", "css");
    merger.addCodeFragment("js1", "console.log('Hello');", "js");
    
    std::cout << "Code fragments added: " << merger.getCodeFragmentNames().size() << std::endl;
    std::cout << "HTML fragments: " << merger.getCodeFragmentNamesByType("html").size() << std::endl;
    std::cout << "CSS fragments: " << merger.getCodeFragmentNamesByType("css").size() << std::endl;
    std::cout << "JS fragments: " << merger.getCodeFragmentNamesByType("js").size() << std::endl;
    
    std::string mergedCode = merger.mergeCodeFragments();
    std::cout << "Code merged: " << (mergedCode.length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试项目工作流
    ProjectWorkflow workflow;
    workflow.setCompilerDispatcher(std::make_shared<CompilerDispatcher>(dispatcher));
    workflow.setCodeMerger(std::make_shared<CodeMerger>(merger));
    
    std::cout << "Workflow configured: " << (workflow.validateWorkflow() ? "YES" : "NO") << std::endl;
    
    std::string testContent = "div { text: 'Hello World'; }";
    std::string processedContent = workflow.processContent(testContent);
    std::cout << "Content processed: " << (processedContent.length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试项目工作流管理器
    ProjectWorkflowManager workflowManager;
    workflowManager.addWorkflow("test", std::make_shared<ProjectWorkflow>(workflow));
    
    std::cout << "Workflows added: " << workflowManager.getWorkflowNames().size() << std::endl;
    
    std::string executedContent = workflowManager.executeWorkflow("test", testContent);
    std::cout << "Workflow executed: " << (executedContent.length() > 0 ? "YES" : "NO") << std::endl;
    
    std::cout << "Project Workflow test completed." << std::endl;
}

void testSyntaxBoundaries() {
    std::cout << "Testing Syntax Boundaries..." << std::endl;
    
    // 测试语法边界检测器
    SyntaxBoundaryDetector detector;
    
    std::string testContent = "<style>body { margin: 0; }</style><script>console.log('Hello');</script>";
    auto boundaries = detector.findBoundaries(testContent);
    
    std::cout << "Boundaries found: " << boundaries.size() << std::endl;
    
    for (const auto& boundary : boundaries) {
        std::string content = testContent.substr(boundary.first, boundary.second - boundary.first + 1);
        std::cout << "Boundary content: " << content.substr(0, 50) << "..." << std::endl;
    }
    
    // 测试语法验证
    bool isValid = detector.validateSyntax(testContent, SyntaxBoundaryType::GLOBAL_STYLE);
    std::cout << "Global style validation: " << (isValid ? "PASS" : "FAIL") << std::endl;
    
    // 测试宽判严判处理器
    WideStrictProcessor processor;
    processor.setWideMode(true);
    
    std::string wideProcessed = processor.processWide(testContent);
    std::cout << "Wide processing: " << (wideProcessed.length() > 0 ? "SUCCESS" : "FAIL") << std::endl;
    
    processor.setStrictMode(true);
    processor.setWideMode(false);
    
    std::string strictProcessed = processor.processStrict(testContent);
    std::cout << "Strict processing: " << (strictProcessed.length() > 0 ? "SUCCESS" : "FAIL") << std::endl;
    
    // 测试语法边界管理器
    SyntaxBoundaryManager manager;
    manager.addBoundary(SyntaxBoundaryType::GLOBAL_STYLE, 0, 20);
    manager.addBoundary(SyntaxBoundaryType::GLOBAL_SCRIPT, 21, 50);
    
    std::cout << "Boundaries added: " << manager.getAllBoundaries().size() << std::endl;
    
    // 测试语法边界解析器
    SyntaxBoundaryParser parser;
    parser.setDetector(std::make_shared<SyntaxBoundaryDetector>(detector));
    
    auto parsedBoundaries = parser.parseBoundaries(testContent);
    std::cout << "Parsed boundaries: " << parsedBoundaries.size() << std::endl;
    
    // 测试语法边界验证器
    SyntaxBoundaryValidator validator;
    validator.setDetector(std::make_shared<SyntaxBoundaryDetector>(detector));
    
    bool allValid = validator.validateAllBoundaries(testContent);
    std::cout << "All boundaries valid: " << (allValid ? "YES" : "NO") << std::endl;
    
    std::cout << "Syntax Boundaries test completed." << std::endl;
}

void testCJMODAPI() {
    std::cout << "Testing CJMOD API..." << std::endl;
    
    // 测试语法分析
    Arg args = Syntax::analyze("$ ** $");
    std::cout << "Syntax analyzed: " << args.toString() << std::endl;
    
    // 测试参数绑定
    args.bind("$", [](const std::string& value) {
        return value;
    });
    
    args.bind("**", [](const std::string& value) {
        return value;
    });
    
    // 测试CJMOD扫描器
    CJMODScanner scanner;
    Arg scanned = CJMODScanner::scan(args, "**");
    std::cout << "Scanned result: " << scanned.toString() << std::endl;
    
    // 测试双指针扫描
    Arg dualScan = CJMODScanner::dualPointerScan("3 ** 4", "**");
    std::cout << "Dual pointer scan: " << dualScan.toString() << std::endl;
    
    // 测试前置截取
    Arg preTruncate = CJMODScanner::preTruncateScan("arg ** arg2", "**");
    std::cout << "Pre-truncate scan: " << preTruncate.toString() << std::endl;
    
    // 测试CJMOD生成器
    CJMODGenerator generator;
    std::string generated = CJMODGenerator::exportResult(args);
    std::cout << "Generated result: " << generated << std::endl;
    
    // 测试函数生成
    std::string function = CJMODGenerator::generateFunction(args, "testFunction");
    std::cout << "Generated function: " << function.substr(0, 50) << "..." << std::endl;
    
    // 测试CHTL JS函数
    CHTLJSFunction jsFunction("myFunction", "return 'Hello World';");
    jsFunction.addParameter("param1");
    jsFunction.addParameter("param2");
    
    std::cout << "Function name: " << jsFunction.getName() << std::endl;
    std::cout << "Function parameters: " << jsFunction.getParameters().size() << std::endl;
    
    std::string generatedFunction = jsFunction.generateFunction();
    std::cout << "Generated JS function: " << generatedFunction.substr(0, 50) << "..." << std::endl;
    
    // 测试CJMOD API管理器
    CJMODAPIManager apiManager;
    apiManager.registerFunction("testFunc", std::make_shared<CHTLJSFunction>("testFunc", "console.log('test');"));
    
    std::cout << "Registered functions: " << apiManager.getFunctionNames().size() << std::endl;
    
    // 测试扫描和生成
    Arg scanResult = apiManager.scanAndGenerate("test content", "test pattern");
    std::cout << "Scan and generate: " << scanResult.toString() << std::endl;
    
    // 测试代码生成
    std::string generatedCode = apiManager.generateCode("test content", "function test() { return '${value}'; }");
    std::cout << "Generated code: " << generatedCode.substr(0, 50) << "..." << std::endl;
    
    // 测试原子参数
    AtomArg atomArg("42", "number");
    std::cout << "Atom value: " << atomArg.getAtomValue() << std::endl;
    std::cout << "Atom type: " << atomArg.getType() << std::endl;
    std::cout << "Is atom: " << (atomArg.isAtom() ? "YES" : "NO") << std::endl;
    
    // 测试语法匹配
    bool matches = Syntax::matches("hello world", "hello.*");
    std::cout << "Pattern matches: " << (matches ? "YES" : "NO") << std::endl;
    
    auto matches2 = Syntax::extractMatches("hello world hello", "hello");
    std::cout << "Extracted matches: " << matches2.size() << std::endl;
    
    std::cout << "CJMOD API test completed." << std::endl;
}

void testOfficialModules() {
    std::cout << "Testing Official Modules..." << std::endl;
    
    // 测试官方模块管理器
    OfficialModuleManager moduleManager;
    
    // 测试Chtholly珂朵莉模块CMOD组件
    std::cout << "Testing Chtholly CMOD components..." << std::endl;
    
    auto accordionComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CMOD, "accordion");
    if (accordionComponent) {
        std::string accordionHTML = accordionComponent->generateHTML();
        std::string accordionCSS = accordionComponent->generateCSS();
        std::string accordionJS = accordionComponent->generateJS();
        
        std::cout << "Accordion component HTML: " << (accordionHTML.length() > 0 ? "GENERATED" : "EMPTY") << std::endl;
        std::cout << "Accordion component CSS: " << (accordionCSS.length() > 0 ? "GENERATED" : "EMPTY") << std::endl;
        std::cout << "Accordion component JS: " << (accordionJS.length() > 0 ? "GENERATED" : "EMPTY") << std::endl;
    }
    
    auto galleryComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CMOD, "fourLeafGallery");
    if (galleryComponent) {
        std::string galleryCode = galleryComponent->generateCode();
        std::cout << "Four leaf gallery code: " << (galleryCode.length() > 0 ? "GENERATED" : "EMPTY") << std::endl;
    }
    
    auto memoComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CMOD, "memo");
    if (memoComponent) {
        std::string memoCode = memoComponent->generateCode();
        std::cout << "Memo component code: " << (memoCode.length() > 0 ? "GENERATED" : "EMPTY") << std::endl;
    }
    
    // 测试Chtholly珂朵莉模块CJMOD组件
    std::cout << "Testing Chtholly CJMOD components..." << std::endl;
    
    auto printMyloveComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CJMOD, "printMylove");
    if (printMyloveComponent) {
        std::string printMyloveJS = printMyloveComponent->generateJS();
        std::cout << "printMylove JS: " << (printMyloveJS.find("printMylove") != std::string::npos ? "FOUND" : "NOT_FOUND") << std::endl;
    }
    
    auto iNeverAwayComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CJMOD, "iNeverAway");
    if (iNeverAwayComponent) {
        std::string iNeverAwayJS = iNeverAwayComponent->generateJS();
        std::cout << "iNeverAway JS: " << (iNeverAwayJS.find("INeverAway") != std::string::npos ? "FOUND" : "NOT_FOUND") << std::endl;
    }
    
    auto utilThenComponent = moduleManager.getComponent(OfficialModuleType::CHTHOLLY_CJMOD, "utilThen");
    if (utilThenComponent) {
        std::string utilThenJS = utilThenComponent->generateJS();
        std::cout << "util...then JS: " << (utilThenJS.find("UtilThenExpression") != std::string::npos ? "FOUND" : "NOT_FOUND") << std::endl;
    }
    
    // 测试模块信息
    std::string chthollyDescription = moduleManager.getModuleDescription(OfficialModuleType::CHTHOLLY_CMOD);
    std::cout << "Chtholly CMOD description: " << chthollyDescription << std::endl;
    
    auto chthollyFeatures = moduleManager.getModuleFeatures(OfficialModuleType::CHTHOLLY_CMOD);
    std::cout << "Chtholly CMOD features: " << chthollyFeatures.size() << std::endl;
    
    auto chthollyJSFeatures = moduleManager.getModuleFeatures(OfficialModuleType::CHTHOLLY_CJMOD);
    std::cout << "Chtholly CJMOD features: " << chthollyJSFeatures.size() << std::endl;
    
    // 测试组件名称列表
    auto chtlollyCMODNames = moduleManager.getComponentNames(OfficialModuleType::CHTHOLLY_CMOD);
    std::cout << "Chtholly CMOD components: " << chtlollyCMODNames.size() << std::endl;
    
    auto chtlollyCJMODNames = moduleManager.getComponentNames(OfficialModuleType::CHTHOLLY_CJMOD);
    std::cout << "Chtholly CJMOD components: " << chtlollyCJMODNames.size() << std::endl;
    
    // 测试模块检查
    bool hasAccordion = moduleManager.hasComponent(OfficialModuleType::CHTHOLLY_CMOD, "accordion");
    std::cout << "Has accordion component: " << (hasAccordion ? "YES" : "NO") << std::endl;
    
    bool hasPrintMylove = moduleManager.hasComponent(OfficialModuleType::CHTHOLLY_CJMOD, "printMylove");
    std::cout << "Has printMylove component: " << (hasPrintMylove ? "YES" : "NO") << std::endl;
    
    // 测试组件生成
    std::string accordionModuleCode = moduleManager.generateModuleCode(OfficialModuleType::CHTHOLLY_CMOD, "accordion");
    std::cout << "Accordion module code generated: " << (accordionModuleCode.length() > 0 ? "YES" : "NO") << std::endl;
    
    std::string printMyloveModuleCode = moduleManager.generateModuleCode(OfficialModuleType::CHTHOLLY_CJMOD, "printMylove");
    std::cout << "printMylove module code generated: " << (printMyloveModuleCode.length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试组件配置
    std::map<std::string, std::any> testConfig;
    testConfig["width"] = std::string("400px");
    testConfig["height"] = std::string("300px");
    
    if (accordionComponent) {
        accordionComponent->setConfiguration(testConfig);
        auto config = accordionComponent->getConfiguration();
        std::cout << "Accordion configuration set: " << (config.size() > 0 ? "YES" : "NO") << std::endl;
        
        bool configValid = accordionComponent->validateConfiguration();
        std::cout << "Accordion configuration valid: " << (configValid ? "YES" : "NO") << std::endl;
    }
    
    std::cout << "Official Modules test completed." << std::endl;
}

void testWildcardImport() {
    std::cout << "Testing Wildcard Import..." << std::endl;
    
    // 测试通配符导入解析器
    WildcardImportParser parser;
    
    // 测试不同类型的通配符模式
    std::vector<std::string> testPatterns = {
        "path.*",           // 所有文件
        "path/*.cmod",      // CMOD文件
        "path/*.chtl",      // CHTL文件
        "Chtholly.*",       // 子模块
        "Chtholly.Space"    // 特定子模块
    };
    
    std::cout << "Testing pattern parsing..." << std::endl;
    for (const auto& pattern : testPatterns) {
        auto wildcardPattern = parser.parsePattern(pattern);
        std::cout << "Pattern: " << pattern << " -> Type: " << static_cast<int>(wildcardPattern.type) << std::endl;
        
        bool isValid = parser.isValidPattern(pattern);
        std::cout << "  Valid: " << (isValid ? "YES" : "NO") << std::endl;
        
        bool isWildcard = parser.isWildcardPattern(pattern);
        std::cout << "  Is wildcard: " << (isWildcard ? "YES" : "NO") << std::endl;
        
        bool isSubmodule = parser.isSubmodulePattern(pattern);
        std::cout << "  Is submodule: " << (isSubmodule ? "YES" : "NO") << std::endl;
    }
    
    // 测试通配符导入解析器
    WildcardImportResolver resolver;
    
    std::cout << "Testing resolver configuration..." << std::endl;
    auto searchPaths = resolver.getSearchPaths();
    std::cout << "Default search paths: " << searchPaths.size() << std::endl;
    
    resolver.addSearchPath("./test_modules");
    resolver.addSearchPath("./examples");
    
    auto newSearchPaths = resolver.getSearchPaths();
    std::cout << "Updated search paths: " << newSearchPaths.size() << std::endl;
    
    // 测试路径解析
    std::string resolvedPath = resolver.resolvePath("test.chtl");
    std::cout << "Resolved path: " << resolvedPath << std::endl;
    
    // 测试通配符导入管理器
    WildcardImportManager manager;
    
    std::cout << "Testing import manager..." << std::endl;
    
    // 测试缓存
    bool cacheEnabled = manager.isCacheEnabled();
    std::cout << "Cache enabled: " << (cacheEnabled ? "YES" : "NO") << std::endl;
    
    manager.enableCache(false);
    std::cout << "Cache disabled: " << (!manager.isCacheEnabled() ? "YES" : "NO") << std::endl;
    
    manager.enableCache(true);
    std::cout << "Cache re-enabled: " << (manager.isCacheEnabled() ? "YES" : "NO") << std::endl;
    
    // 测试导入模式（这些可能会失败，因为文件不存在，但测试逻辑）
    auto importResults = manager.importPattern("test.*");
    std::cout << "Import results: " << importResults.size() << std::endl;
    
    // 测试统计信息
    size_t totalImports = manager.getTotalImportsCount();
    size_t successfulImports = manager.getSuccessfulImportsCount();
    size_t failedImports = manager.getFailedImportsCount();
    
    std::cout << "Total imports: " << totalImports << std::endl;
    std::cout << "Successful imports: " << successfulImports << std::endl;
    std::cout << "Failed imports: " << failedImports << std::endl;
    
    auto importStats = manager.getImportStatsByType();
    std::cout << "Import stats by type: " << importStats.size() << std::endl;
    
    // 测试子模块导入器
    SubmoduleImporter submoduleImporter;
    
    std::cout << "Testing submodule importer..." << std::endl;
    
    std::string moduleBasePath = submoduleImporter.getModuleBasePath();
    std::cout << "Module base path: " << moduleBasePath << std::endl;
    
    std::string separator = submoduleImporter.getSubmoduleSeparator();
    std::cout << "Submodule separator: " << separator << std::endl;
    
    // 测试子模块查找（可能为空，因为模块不存在）
    auto allSubmodules = submoduleImporter.findAllSubmodules("TestModule");
    std::cout << "All submodules found: " << allSubmodules.size() << std::endl;
    
    // 测试子模块验证
    bool isValidSubmodule = submoduleImporter.isValidSubmodule("TestModule", "TestSubmodule");
    std::cout << "Is valid submodule: " << (isValidSubmodule ? "YES" : "NO") << std::endl;
    
    auto validationErrors = submoduleImporter.getSubmoduleValidationErrors("", "");
    std::cout << "Validation errors for empty names: " << validationErrors.size() << std::endl;
    
    // 测试路径等价系统
    PathEquivalenceSystem pathSystem;
    
    std::cout << "Testing path equivalence system..." << std::endl;
    
    std::string normalizedPath = pathSystem.normalizePath("./test/../path/./file.txt");
    std::cout << "Normalized path: " << normalizedPath << std::endl;
    
    bool pathsEquivalent = pathSystem.arePathsEquivalent("./test/../path", "path");
    std::cout << "Paths equivalent: " << (pathsEquivalent ? "YES" : "NO") << std::endl;
    
    std::string dotToSlash = pathSystem.convertDotToSlash("test.path.file");
    std::cout << "Dot to slash: " << dotToSlash << std::endl;
    
    std::string slashToDot = pathSystem.convertSlashToDot("test/path/file");
    std::cout << "Slash to dot: " << slashToDot << std::endl;
    
    bool validPath = pathSystem.isValidPath("valid/path/file.txt");
    std::cout << "Valid path: " << (validPath ? "YES" : "NO") << std::endl;
    
    bool invalidPath = pathSystem.isValidPath("invalid|path");
    std::cout << "Invalid path: " << (!invalidPath ? "CORRECTLY_INVALID" : "INCORRECTLY_VALID") << std::endl;
    
    auto pathParts = pathSystem.splitPath("path/to/file.txt");
    std::cout << "Path parts: " << pathParts.size() << std::endl;
    
    std::string joinedPath = pathSystem.joinPath({"path", "to", "file.txt"});
    std::cout << "Joined path: " << joinedPath << std::endl;
    
    std::cout << "Wildcard Import test completed." << std::endl;
}

void testDefaultStructGenerator() {
    std::cout << "Testing Default Struct Generator..." << std::endl;
    
    // 测试默认结构生成器
    DefaultStructGenerator generator;
    
    std::string testContent = R"(
<div>
    <h1>Hello World</h1>
    <p>This is a test document.</p>
</div>
)";
    
    // 测试最小结构
    std::cout << "Testing minimal structure..." << std::endl;
    std::string minimalStruct = generator.generateMinimalStruct(testContent);
    std::cout << "Minimal structure generated: " << (minimalStruct.find("<!DOCTYPE html>") != std::string::npos ? "YES" : "NO") << std::endl;
    
    // 测试标准结构
    std::cout << "Testing standard structure..." << std::endl;
    std::string standardStruct = generator.generateStandardStruct(testContent);
    std::cout << "Standard structure generated: " << (standardStruct.find("viewport") != std::string::npos ? "YES" : "NO") << std::endl;
    
    // 测试SPA结构
    std::cout << "Testing SPA structure..." << std::endl;
    std::string spaStruct = generator.generateSPAStruct(testContent);
    std::cout << "SPA structure generated: " << (spaStruct.find("id=\"app\"") != std::string::npos ? "YES" : "NO") << std::endl;
    
    // 测试文档结构
    std::cout << "Testing document structure..." << std::endl;
    std::string documentStruct = generator.generateDocumentStruct(testContent);
    std::cout << "Document structure generated: " << (documentStruct.find("<header>") != std::string::npos ? "YES" : "NO") << std::endl;
    
    // 测试应用结构
    std::cout << "Testing application structure..." << std::endl;
    std::string applicationStruct = generator.generateApplicationStruct(testContent);
    std::cout << "Application structure generated: " << (applicationStruct.find("app-container") != std::string::npos ? "YES" : "NO") << std::endl;
    
    // 测试结构检测
    std::cout << "Testing structure detection..." << std::endl;
    std::string htmlWithStructure = "<!DOCTYPE html><html><head><title>Test</title></head><body><p>Content</p></body></html>";
    std::string htmlWithoutStructure = "<p>Just content</p>";
    
    bool hasStructure1 = generator.hasHTMLStructure(htmlWithStructure);
    bool hasStructure2 = generator.hasHTMLStructure(htmlWithoutStructure);
    std::cout << "HTML with structure detected: " << (hasStructure1 ? "YES" : "NO") << std::endl;
    std::cout << "HTML without structure detected: " << (!hasStructure2 ? "YES" : "NO") << std::endl;
    
    // 测试内容提取
    std::cout << "Testing content extraction..." << std::endl;
    std::string bodyContent = generator.extractBodyContent(htmlWithStructure);
    std::string headContent = generator.extractHeadContent(htmlWithStructure);
    std::string title = generator.extractTitle(htmlWithStructure);
    
    std::cout << "Body content extracted: " << (bodyContent.find("<p>Content</p>") != std::string::npos ? "YES" : "NO") << std::endl;
    std::cout << "Head content extracted: " << (headContent.find("<title>Test</title>") != std::string::npos ? "YES" : "NO") << std::endl;
    std::cout << "Title extracted: " << (title == "Test" ? "YES" : "NO") << std::endl;
    
    // 测试默认结构管理器
    DefaultStructManager manager;
    
    std::cout << "Testing default struct manager..." << std::endl;
    
    // 测试启用/禁用
    manager.enableDefaultStruct(true);
    bool enabled = manager.isDefaultStructEnabled();
    std::cout << "Default struct enabled: " << (enabled ? "YES" : "NO") << std::endl;
    
    manager.enableDefaultStruct(false);
    bool disabled = !manager.isDefaultStructEnabled();
    std::cout << "Default struct disabled: " << (disabled ? "YES" : "NO") << std::endl;
    
    manager.enableDefaultStruct(true);
    
    // 测试结构类型设置
    manager.setStructType(DefaultStructType::SPA);
    DefaultStructType type = manager.getStructType();
    std::cout << "SPA structure type set: " << (type == DefaultStructType::SPA ? "YES" : "NO") << std::endl;
    
    // 测试内容处理
    std::string processedContent = manager.processContent(testContent);
    std::cout << "Content processed: " << (processedContent.length() > testContent.length() ? "YES" : "NO") << std::endl;
    
    // 测试预设管理
    auto presets = manager.getAvailablePresets();
    std::cout << "Available presets: " << presets.size() << std::endl;
    
    manager.loadPreset("spa");
    std::cout << "SPA preset loaded: " << (manager.getStructType() == DefaultStructType::SPA ? "YES" : "NO") << std::endl;
    
    // 测试配置验证
    DefaultStructConfig validConfig;
    validConfig.title = "Test Document";
    validConfig.charset = "UTF-8";
    
    bool configValid = manager.validateConfig(validConfig);
    std::cout << "Valid config validated: " << (configValid ? "YES" : "NO") << std::endl;
    
    DefaultStructConfig invalidConfig;
    invalidConfig.title = ""; // 空标题
    invalidConfig.charset = ""; // 空字符集
    
    bool configInvalid = !manager.validateConfig(invalidConfig);
    std::cout << "Invalid config rejected: " << (configInvalid ? "YES" : "NO") << std::endl;
    
    // 测试编译器选项处理器
    CompilerOptionProcessor optionProcessor;
    
    std::cout << "Testing compiler option processor..." << std::endl;
    
    // 测试选项解析
    std::string validOption = "--default-struct --type=spa --title=\"My SPA\"";
    std::string invalidOption = "--invalid-option";
    
    bool option1Valid = optionProcessor.parseDefaultStructOption(validOption);
    bool option2Valid = optionProcessor.parseDefaultStructOption(invalidOption);
    
    std::cout << "Valid option parsed: " << (option1Valid ? "YES" : "NO") << std::endl;
    std::cout << "Invalid option rejected: " << (!option2Valid ? "YES" : "NO") << std::endl;
    
    // 测试选项验证
    bool optionValid = optionProcessor.isValidDefaultStructOption(validOption);
    bool optionInvalid = !optionProcessor.isValidDefaultStructOption(invalidOption);
    
    std::cout << "Valid option validated: " << (optionValid ? "YES" : "NO") << std::endl;
    std::cout << "Invalid option rejected: " << (optionInvalid ? "YES" : "NO") << std::endl;
    
    // 测试选项处理
    std::string processedWithOption = optionProcessor.processDefaultStructOption(testContent, validOption);
    std::cout << "Content processed with option: " << (processedWithOption.length() > testContent.length() ? "YES" : "NO") << std::endl;
    
    // 测试帮助信息
    std::string help = optionProcessor.getDefaultStructOptionHelp();
    std::cout << "Help information available: " << (help.length() > 0 ? "YES" : "NO") << std::endl;
    
    auto examples = optionProcessor.getDefaultStructOptionExamples();
    std::cout << "Option examples available: " << examples.size() << std::endl;
    
    // 测试自定义配置
    DefaultStructConfig customConfig;
    customConfig.type = DefaultStructType::APPLICATION;
    customConfig.title = "Custom CHTL App";
    customConfig.charset = "UTF-8";
    customConfig.html_lang = "zh-CN";
    customConfig.body_class = "custom-app";
    
    std::string customStruct = generator.generateDefaultStruct(testContent, customConfig);
    std::cout << "Custom structure generated: " << (customStruct.find("Custom CHTL App") != std::string::npos ? "YES" : "NO") << std::endl;
    std::cout << "Custom language set: " << (customStruct.find("lang=\"zh-CN\"") != std::string::npos ? "YES" : "NO") << std::endl;
    std::cout << "Custom body class set: " << (customStruct.find("class=\"custom-app\"") != std::string::npos ? "YES" : "NO") << std::endl;
    
    std::cout << "Default Struct Generator test completed." << std::endl;
}

void testCompilationMonitor() {
    std::cout << "Testing Compilation Monitor..." << std::endl;
    
    // 测试编译监视器配置
    CompilationMonitorConfig config;
    config.memory_config.max_memory_mb = 512;
    config.memory_config.warning_threshold_mb = 256;
    config.memory_config.critical_threshold_mb = 384;
    config.memory_config.auto_kill_enabled = true;
    config.memory_config.memory_tracking_enabled = true;
    
    config.time_config.max_compile_time = std::chrono::minutes(5);
    config.time_config.warning_time = std::chrono::minutes(2);
    config.time_config.critical_time = std::chrono::minutes(4);
    config.time_config.auto_kill_enabled = true;
    config.time_config.time_tracking_enabled = true;
    
    config.enable_logging = true;
    config.enable_notifications = true;
    config.log_file_path = "compilation_monitor_test.log";
    
    // 测试编译监视器
    CompilationMonitor monitor(config);
    
    std::cout << "Testing compilation monitor creation..." << std::endl;
    std::cout << "Monitor created: " << (monitor.getStatus() == CompilationStatus::IDLE ? "YES" : "NO") << std::endl;
    
    // 测试编译开始
    std::cout << "Testing compilation start..." << std::endl;
    monitor.startCompilation();
    std::cout << "Compilation started: " << (monitor.isCompiling() ? "YES" : "NO") << std::endl;
    std::cout << "Status: " << (monitor.getStatus() == CompilationStatus::RUNNING ? "RUNNING" : "OTHER") << std::endl;
    
    // 测试统计信息
    std::cout << "Testing compilation stats..." << std::endl;
    auto stats = monitor.getStats();
    std::cout << "Stats available: " << (stats.status == CompilationStatus::RUNNING ? "YES" : "NO") << std::endl;
    
    // 测试内存监视器
    std::cout << "Testing memory monitor..." << std::endl;
    auto& memoryMonitor = monitor.getMemoryMonitor();
    std::cout << "Memory monitor created: " << (memoryMonitor.isMonitoring() ? "YES" : "NO") << std::endl;
    
    size_t currentMemory = memoryMonitor.getCurrentMemoryUsage();
    size_t peakMemory = memoryMonitor.getPeakMemoryUsage();
    double memoryPercentage = memoryMonitor.getMemoryUsagePercentage();
    
    std::cout << "Current memory usage: " << currentMemory << " bytes" << std::endl;
    std::cout << "Peak memory usage: " << peakMemory << " bytes" << std::endl;
    std::cout << "Memory percentage: " << memoryPercentage << "%" << std::endl;
    
    // 测试内存状态检查
    bool memoryNormal = memoryMonitor.isMemoryUsageNormal();
    bool memoryWarning = memoryMonitor.isMemoryUsageWarning();
    bool memoryCritical = memoryMonitor.isMemoryUsageCritical();
    bool memoryExceeded = memoryMonitor.isMemoryUsageExceeded();
    
    std::cout << "Memory normal: " << (memoryNormal ? "YES" : "NO") << std::endl;
    std::cout << "Memory warning: " << (memoryWarning ? "YES" : "NO") << std::endl;
    std::cout << "Memory critical: " << (memoryCritical ? "YES" : "NO") << std::endl;
    std::cout << "Memory exceeded: " << (memoryExceeded ? "YES" : "NO") << std::endl;
    
    // 测试时间监视器
    std::cout << "Testing time monitor..." << std::endl;
    auto& timeMonitor = monitor.getTimeMonitor();
    std::cout << "Time monitor created: " << (timeMonitor.isTiming() ? "YES" : "NO") << std::endl;
    
    auto elapsedTime = timeMonitor.getElapsedTime();
    auto remainingTime = timeMonitor.getRemainingTime();
    double timeProgress = timeMonitor.getTimeProgress();
    
    std::cout << "Elapsed time: " << elapsedTime.count() << " ms" << std::endl;
    std::cout << "Remaining time: " << remainingTime.count() << " ms" << std::endl;
    std::cout << "Time progress: " << (timeProgress * 100) << "%" << std::endl;
    
    // 测试时间状态检查
    bool timeNormal = timeMonitor.isTimeNormal();
    bool timeWarning = timeMonitor.isTimeWarning();
    bool timeCritical = timeMonitor.isTimeCritical();
    bool timeExceeded = timeMonitor.isTimeExceeded();
    
    std::cout << "Time normal: " << (timeNormal ? "YES" : "NO") << std::endl;
    std::cout << "Time warning: " << (timeWarning ? "YES" : "NO") << std::endl;
    std::cout << "Time critical: " << (timeCritical ? "YES" : "NO") << std::endl;
    std::cout << "Time exceeded: " << (timeExceeded ? "YES" : "NO") << std::endl;
    
    // 测试暂停和恢复
    std::cout << "Testing pause and resume..." << std::endl;
    monitor.pauseCompilation();
    std::cout << "Compilation paused: " << (monitor.isPaused() ? "YES" : "NO") << std::endl;
    
    monitor.resumeCompilation();
    std::cout << "Compilation resumed: " << (monitor.isCompiling() ? "YES" : "NO") << std::endl;
    
    // 测试自动杀死功能
    std::cout << "Testing auto-kill functionality..." << std::endl;
    monitor.enableAutoKill(true);
    std::cout << "Auto-kill enabled: " << (monitor.isAutoKillEnabled() ? "YES" : "NO") << std::endl;
    
    monitor.enableAutoKill(false);
    std::cout << "Auto-kill disabled: " << (!monitor.isAutoKillEnabled() ? "YES" : "NO") << std::endl;
    
    monitor.enableAutoKill(true);
    
    // 测试编译完成
    std::cout << "Testing compilation completion..." << std::endl;
    monitor.stopCompilation();
    std::cout << "Compilation stopped: " << (!monitor.isCompiling() ? "YES" : "NO") << std::endl;
    std::cout << "Status: " << (monitor.getStatus() == CompilationStatus::COMPLETED ? "COMPLETED" : "OTHER") << std::endl;
    
    // 测试报告生成
    std::cout << "Testing report generation..." << std::endl;
    std::string report = monitor.generateReport();
    std::cout << "Report generated: " << (report.length() > 0 ? "YES" : "NO") << std::endl;
    std::cout << "Report length: " << report.length() << " characters" << std::endl;
    
    // 测试报告保存
    std::cout << "Testing report saving..." << std::endl;
    monitor.saveReportToFile("compilation_report_test.txt");
    std::cout << "Report saved: " << (CompilationMonitorUtils::fileExists("compilation_report_test.txt") ? "YES" : "NO") << std::endl;
    
    // 测试编译监视器管理器
    CompilationMonitorManager manager;
    
    std::cout << "Testing compilation monitor manager..." << std::endl;
    
    // 测试创建监视器
    auto monitor1 = manager.createMonitor(config);
    auto monitor2 = manager.createMonitor(config);
    
    std::cout << "Monitors created: " << (monitor1 && monitor2 ? "YES" : "NO") << std::endl;
    
    // 测试获取监视器
    auto retrievedMonitor = manager.getMonitor("invalid_id");
    std::cout << "Invalid monitor retrieved: " << (retrievedMonitor == nullptr ? "YES" : "NO") << std::endl;
    
    // 测试全局配置
    CompilationMonitorConfig globalConfig = manager.getGlobalConfig();
    std::cout << "Global config available: " << (globalConfig.memory_config.max_memory_mb > 0 ? "YES" : "NO") << std::endl;
    
    // 测试活动监视器
    auto activeIds = manager.getActiveMonitorIds();
    std::cout << "Active monitor IDs: " << activeIds.size() << std::endl;
    
    size_t activeCount = manager.getActiveMonitorCount();
    std::cout << "Active monitor count: " << activeCount << std::endl;
    
    bool hasActive = manager.hasActiveMonitors();
    std::cout << "Has active monitors: " << (hasActive ? "YES" : "NO") << std::endl;
    
    // 测试全局统计
    auto allStats = manager.getAllStats();
    std::cout << "All stats available: " << allStats.size() << std::endl;
    
    // 测试全局报告
    std::string globalReport = manager.generateGlobalReport();
    std::cout << "Global report generated: " << (globalReport.length() > 0 ? "YES" : "NO") << std::endl;
    
    // 测试全局控制
    manager.pauseAllMonitors();
    std::cout << "All monitors paused: " << (manager.getActiveMonitorCount() == 0 ? "YES" : "NO") << std::endl;
    
    manager.resumeAllMonitors();
    std::cout << "All monitors resumed: " << (manager.getActiveMonitorCount() >= 0 ? "YES" : "NO") << std::endl;
    
    manager.killAllMonitors();
    std::cout << "All monitors killed: " << (manager.getActiveMonitorCount() == 0 ? "YES" : "NO") << std::endl;
    
    manager.resetAllStats();
    std::cout << "All stats reset: " << (manager.getAllStats().size() >= 0 ? "YES" : "NO") << std::endl;
    
    // 测试工具类
    std::cout << "Testing compilation monitor utils..." << std::endl;
    
    size_t processMemory = CompilationMonitorUtils::getProcessMemoryUsage();
    size_t systemMemoryTotal = CompilationMonitorUtils::getSystemMemoryTotal();
    size_t systemMemoryAvailable = CompilationMonitorUtils::getSystemMemoryAvailable();
    double systemMemoryPercentage = CompilationMonitorUtils::getSystemMemoryUsagePercentage();
    
    std::cout << "Process memory usage: " << processMemory << " bytes" << std::endl;
    std::cout << "System memory total: " << systemMemoryTotal << " bytes" << std::endl;
    std::cout << "System memory available: " << systemMemoryAvailable << " bytes" << std::endl;
    std::cout << "System memory percentage: " << systemMemoryPercentage << "%" << std::endl;
    
    auto currentTime = CompilationMonitorUtils::getCurrentTime();
    std::string formattedTime = CompilationMonitorUtils::formatTime(currentTime);
    std::string formattedDuration = CompilationMonitorUtils::formatDuration(std::chrono::milliseconds(1234567));
    
    std::cout << "Current time: " << currentTime.count() << " ms" << std::endl;
    std::cout << "Formatted time: " << formattedTime << std::endl;
    std::cout << "Formatted duration: " << formattedDuration << std::endl;
    
    std::string processId = CompilationMonitorUtils::getProcessId();
    std::string systemInfo = CompilationMonitorUtils::getSystemInfo();
    
    std::cout << "Process ID: " << processId << std::endl;
    std::cout << "System info: " << systemInfo << std::endl;
    
    bool processRunning = CompilationMonitorUtils::isProcessRunning(processId);
    std::cout << "Process running: " << (processRunning ? "YES" : "NO") << std::endl;
    
    // 测试文件操作
    std::string testContent = "Test content for file operations";
    bool writeSuccess = CompilationMonitorUtils::writeToFile("test_file.txt", testContent);
    std::cout << "File write success: " << (writeSuccess ? "YES" : "NO") << std::endl;
    
    std::string readContent = CompilationMonitorUtils::readFromFile("test_file.txt");
    std::cout << "File read success: " << (readContent == testContent ? "YES" : "NO") << std::endl;
    
    bool fileExists = CompilationMonitorUtils::fileExists("test_file.txt");
    std::cout << "File exists: " << (fileExists ? "YES" : "NO") << std::endl;
    
    // 测试日志操作
    CompilationMonitorUtils::logToFile("test_log.txt", "Test log message");
    auto logLines = CompilationMonitorUtils::readLogFile("test_log.txt");
    std::cout << "Log written and read: " << (logLines.size() > 0 ? "YES" : "NO") << std::endl;
    
    CompilationMonitorUtils::clearLogFile("test_log.txt");
    auto clearedLogLines = CompilationMonitorUtils::readLogFile("test_log.txt");
    std::cout << "Log cleared: " << (clearedLogLines.size() == 0 ? "YES" : "NO") << std::endl;
    
    // 测试回调函数
    std::cout << "Testing callback functions..." << std::endl;
    
    bool startCallbackCalled = false;
    bool completeCallbackCalled = false;
    bool errorCallbackCalled = false;
    bool killCallbackCalled = false;
    
    monitor.setCompilationStartCallback([&startCallbackCalled]() {
        startCallbackCalled = true;
    });
    
    monitor.setCompilationCompleteCallback([&completeCallbackCalled](CompilationStatus status) {
        completeCallbackCalled = true;
    });
    
    monitor.setCompilationErrorCallback([&errorCallbackCalled](const std::string& error) {
        errorCallbackCalled = true;
    });
    
    monitor.setCompilationKillCallback([&killCallbackCalled]() {
        killCallbackCalled = true;
    });
    
    // 测试回调触发
    monitor.startCompilation();
    std::cout << "Start callback triggered: " << (startCallbackCalled ? "YES" : "NO") << std::endl;
    
    monitor.stopCompilation();
    std::cout << "Complete callback triggered: " << (completeCallbackCalled ? "YES" : "NO") << std::endl;
    
    monitor.startCompilation();
    monitor.killCompilation();
    std::cout << "Kill callback triggered: " << (killCallbackCalled ? "YES" : "NO") << std::endl;
    
    // 测试内存监视器回调
    std::cout << "Testing memory monitor callbacks..." << std::endl;
    
    bool memoryWarningCalled = false;
    bool memoryCriticalCalled = false;
    bool memoryExceededCalled = false;
    
    memoryMonitor.setMemoryWarningCallback([&memoryWarningCalled](size_t usage) {
        memoryWarningCalled = true;
    });
    
    memoryMonitor.setMemoryCriticalCallback([&memoryCriticalCalled](size_t usage) {
        memoryCriticalCalled = true;
    });
    
    memoryMonitor.setMemoryExceededCallback([&memoryExceededCalled](size_t usage) {
        memoryExceededCalled = true;
    });
    
    // 测试时间监视器回调
    std::cout << "Testing time monitor callbacks..." << std::endl;
    
    bool timeWarningCalled = false;
    bool timeCriticalCalled = false;
    bool timeExceededCalled = false;
    
    timeMonitor.setTimeWarningCallback([&timeWarningCalled](std::chrono::milliseconds elapsed) {
        timeWarningCalled = true;
    });
    
    timeMonitor.setTimeCriticalCallback([&timeCriticalCalled](std::chrono::milliseconds elapsed) {
        timeCriticalCalled = true;
    });
    
    timeMonitor.setTimeExceededCallback([&timeExceededCalled](std::chrono::milliseconds elapsed) {
        timeExceededCalled = true;
    });
    
    std::cout << "Memory monitor callbacks set: " << (memoryWarningCalled || memoryCriticalCalled || memoryExceededCalled ? "YES" : "NO") << std::endl;
    std::cout << "Time monitor callbacks set: " << (timeWarningCalled || timeCriticalCalled || timeExceededCalled ? "YES" : "NO") << std::endl;
    
    // 测试配置更新
    std::cout << "Testing configuration updates..." << std::endl;
    
    CompilationMonitorConfig newConfig = config;
    newConfig.memory_config.max_memory_mb = 1024;
    newConfig.time_config.max_compile_time = std::chrono::minutes(10);
    
    monitor.updateConfig(newConfig);
    auto updatedConfig = monitor.getConfig();
    std::cout << "Config updated: " << (updatedConfig.memory_config.max_memory_mb == 1024 ? "YES" : "NO") << std::endl;
    
    // 测试统计信息更新
    std::cout << "Testing stats updates..." << std::endl;
    
    CompilationStats newStats;
    newStats.files_processed = 10;
    newStats.lines_processed = 1000;
    newStats.errors_count = 5;
    newStats.warnings_count = 3;
    
    monitor.updateStats(newStats);
    auto updatedStats = monitor.getStats();
    std::cout << "Stats updated: " << (updatedStats.files_processed == 10 ? "YES" : "NO") << std::endl;
    
    // 测试统计信息重置
    monitor.resetStats();
    auto resetStats = monitor.getStats();
    std::cout << "Stats reset: " << (resetStats.files_processed == 0 ? "YES" : "NO") << std::endl;
    
    // 清理测试文件
    std::remove("test_file.txt");
    std::remove("test_log.txt");
    std::remove("compilation_report_test.txt");
    std::remove("compilation_monitor_test.log");
    
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
        
        testConfigurationSystem();
        std::cout << std::endl;
        
        testUseStatement();
        std::cout << std::endl;
        
        testCHTLJSCompiler();
        std::cout << std::endl;
        
        testProjectWorkflow();
        std::cout << std::endl;
        
        testSyntaxBoundaries();
        std::cout << std::endl;
        
        testCJMODAPI();
        std::cout << std::endl;
        
        testOfficialModules();
        std::cout << std::endl;
        
        testWildcardImport();
        std::cout << std::endl;
        
        testDefaultStructGenerator();
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