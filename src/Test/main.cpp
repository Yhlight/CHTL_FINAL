#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator.h"
#include "CHTL/TemplateSystem.h"
#include "CHTL/StyleSystem.h"
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

void testLexer() {
    std::cout << "Testing CHTL Lexer..." << std::endl;
    
    CHTLLexer lexer;
    lexer.setSource("text { Hello World }");
    
    auto tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
    
    std::cout << "Lexer test completed." << std::endl << std::endl;
}

void testScanner() {
    std::cout << "Testing Unified Scanner..." << std::endl;
    
    std::string sourceCode = R"(
        html {
            head {
                title { text { Test } }
            }
            body {
                div {
                    style {
                        color: red;
                    }
                }
            }
        }
    )";
    
    UnifiedScanner scanner;
    auto fragments = scanner.scan(sourceCode);
    
    std::cout << "Found " << fragments.size() << " code fragments:" << std::endl;
    for (const auto& fragment : fragments) {
        std::cout << "Type: " << static_cast<int>(fragment.type) 
                  << ", Content: " << fragment.content.substr(0, 50) << "..." << std::endl;
    }
    
    std::cout << "Scanner test completed." << std::endl << std::endl;
}

void testGenerator() {
    std::cout << "Testing CHTL Generator..." << std::endl;
    
    // 创建简单的AST
    auto root = std::make_unique<ASTNode>(ASTNode::NodeType::ROOT);
    auto html = std::make_unique<ElementNode>("html");
    auto body = std::make_unique<ElementNode>("body");
    auto div = std::make_unique<ElementNode>("div");
    auto text = std::make_unique<TextNode>("Hello CHTL!");
    
    div->addChild(std::move(text));
    body->addChild(std::move(div));
    html->addChild(std::move(body));
    root->addChild(std::move(html));
    
    CHTLGenerator generator;
    std::string htmlOutput = generator.generateHTML(root.get());
    
    std::cout << "Generated HTML:" << std::endl;
    std::cout << htmlOutput << std::endl;
    
    std::cout << "Generator test completed." << std::endl << std::endl;
}

void testCHTLJSLexer() {
    std::cout << "Testing CHTL JS Lexer..." << std::endl;
    
    CHTLJSLexer lexer;
    lexer.setSource("vir test = listen { click: () => {} }");
    
    auto tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
    
    std::cout << "CHTL JS Lexer test completed." << std::endl << std::endl;
}

void testCHTLJSGenerator() {
    std::cout << "Testing CHTL JS Generator..." << std::endl;
    
    // 创建简单的CHTL JS AST
    auto root = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::ROOT);
    auto virtualObj = std::make_unique<VirtualObjectNode>("testObject");
    auto listen = std::make_unique<ListenNode>("button");
    
    listen->events["click"] = "() => { console.log('clicked!'); }";
    virtualObj->properties["click"] = "() => { console.log('clicked!'); }";
    
    root->addChild(std::move(virtualObj));
    root->addChild(std::move(listen));
    
    CHTLJSGenerator generator;
    std::string jsOutput = generator.generateJavaScript(root.get());
    
    std::cout << "Generated JavaScript:" << std::endl;
    std::cout << jsOutput << std::endl;
    
    std::cout << "CHTL JS Generator test completed." << std::endl << std::endl;
}

void testTemplateSystem() {
    std::cout << "Testing Template System..." << std::endl;
    
    TemplateSystem templateSystem;
    
    // 创建样式组模板
    auto styleTemplate = std::make_unique<StyleTemplate>("DefaultText");
    styleTemplate->properties["color"] = "black";
    styleTemplate->properties["line-height"] = "1.6";
    styleTemplate->properties["font-size"] = "16px";
    
    templateSystem.registerTemplate(std::move(styleTemplate));
    
    // 创建变量组模板
    auto varTemplate = std::make_unique<VarTemplate>("ThemeColors");
    varTemplate->properties["primary"] = "rgb(255, 192, 203)";
    varTemplate->properties["secondary"] = "rgb(253, 144, 162)";
    
    templateSystem.registerTemplate(std::move(varTemplate));
    
    // 创建自定义样式组
    auto customStyle = std::make_unique<CustomStyle>("CustomButton");
    customStyle->properties["background-color"] = "ThemeColors(primary)";
    customStyle->properties["padding"] = "10px 20px";
    customStyle->properties["border"] = "none";
    customStyle->properties["border-radius"] = "5px";
    
    templateSystem.registerCustom(std::move(customStyle));
    
    // 测试模板展开
    std::map<std::string, std::string> params;
    params["color"] = "red";
    
    std::string expandedStyle = templateSystem.expandTemplate("DefaultText", params);
    std::cout << "Expanded Style Template:" << std::endl;
    std::cout << expandedStyle << std::endl;
    
    std::string expandedCustom = templateSystem.expandCustom("CustomButton");
    std::cout << "Expanded Custom Style:" << std::endl;
    std::cout << expandedCustom << std::endl;
    
    // 创建命名空间
    auto namespace = std::make_unique<Namespace>("MyTheme");
    templateSystem.registerNamespace(std::move(namespace));
    
    std::cout << "Template System test completed." << std::endl << std::endl;
}

void testStyleSystem() {
    std::cout << "Testing Style System..." << std::endl;

    StyleSystem styleSystem;

    // 测试属性运算
    std::map<std::string, std::string> context;
    context["width"] = "100px";
    context["height"] = "200px";

    std::string result1 = styleSystem.processPropertyCalculation("100px + 50px", context);
    std::cout << "Property Calculation (100px + 50px): " << result1 << std::endl;

    std::string result2 = styleSystem.processPropertyCalculation("200px * 2", context);
    std::cout << "Property Calculation (200px * 2): " << result2 << std::endl;

    // 测试条件表达式
    std::string result3 = styleSystem.processConditionalExpression("width > 50px ? \"red\" : \"blue\"", context);
    std::cout << "Conditional Expression: " << result3 << std::endl;

    // 测试属性引用
    context["box.width"] = "100px";
    std::string result4 = styleSystem.processPropertyReference("box.width", context);
    std::cout << "Property Reference (box.width): " << result4 << std::endl;

    // 测试响应式值
    context["boxClass"] = "responsive-box";
    context["boxWidth"] = "100";
    std::string result5 = styleSystem.processReactiveValue("$boxClass$", context);
    std::cout << "Reactive Value ($boxClass$): " << result5 << std::endl;

    // 测试全局样式
    styleSystem.addGlobalStyle(".test-class { color: red; }");
    styleSystem.addGlobalStyle("#test-id { background: blue; }");
    
    std::string globalStyles = styleSystem.getGlobalStyles();
    std::cout << "Global Styles:" << std::endl;
    std::cout << globalStyles << std::endl;

    std::cout << "Style System test completed." << std::endl << std::endl;
}

int main() {
    std::cout << "CHTL Test Suite" << std::endl;
    std::cout << "===============" << std::endl << std::endl;
    
    testLexer();
    testScanner();
    testGenerator();
    testCHTLJSLexer();
    testCHTLJSGenerator();
    testTemplateSystem();
    testStyleSystem();
    
    std::cout << "All tests completed!" << std::endl;
    
    return 0;
}