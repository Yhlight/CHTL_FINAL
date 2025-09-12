#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
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
        
        std::cout << "All tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}