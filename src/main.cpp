#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

int main() {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    // 创建上下文
    auto context = std::make_shared<CHTLContext>();
    context->setDebugMode(true);
    
    // 测试词法分析器
    std::string testCode = R"(
html
{
    head
    {
        title
        {
            "CHTL Test Page"
        }
    }
    
    body
    {
        div
        {
            class: welcome;
            id: main-content;
            
            text
            {
                Hello, CHTL World!
            }
        }
    }
}
)";
    
    std::cout << "\nTesting CHTL Lexer..." << std::endl;
    auto lexer = std::make_shared<CHTLLexer>(testCode);
    auto tokens = lexer->tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  " << static_cast<int>(token.type) << ": '" << token.value 
                  << "' (line " << token.line << ", col " << token.column << ")" << std::endl;
    }
    
    // 测试节点创建
    std::cout << "\nTesting Node Creation..." << std::endl;
    
    // 创建HTML元素
    auto htmlElement = std::make_shared<ElementNode>("html");
    htmlElement->setPosition(1, 1);
    
    // 创建head元素
    auto headElement = std::make_shared<ElementNode>("head");
    headElement->setPosition(3, 5);
    
    // 创建title元素
    auto titleElement = std::make_shared<ElementNode>("title");
    titleElement->setPosition(5, 9);
    
    // 创建文本节点
    auto titleText = std::make_shared<TextNode>("CHTL Test Page");
    titleText->setPosition(7, 13);
    
    // 创建body元素
    auto bodyElement = std::make_shared<ElementNode>("body");
    bodyElement->setPosition(11, 5);
    
    // 创建div元素
    auto divElement = std::make_shared<ElementNode>("div");
    divElement->setPosition(13, 9);
    divElement->addClass("welcome");
    divElement->setID("main-content");
    
    // 创建div文本节点
    auto divText = std::make_shared<TextNode>("Hello, CHTL World!");
    divText->setPosition(19, 13);
    
    // 构建节点层次结构
    titleElement->addChild(titleText);
    headElement->addChild(titleElement);
    bodyElement->addChild(divElement);
    divElement->addChild(divText);
    htmlElement->addChild(headElement);
    htmlElement->addChild(bodyElement);
    
    // 测试节点访问
    std::cout << "\nNode Structure:" << std::endl;
    std::cout << htmlElement->toString() << std::endl;
    std::cout << "  " << headElement->toString() << std::endl;
    std::cout << "    " << titleElement->toString() << std::endl;
    std::cout << "      " << titleText->toString() << std::endl;
    std::cout << "  " << bodyElement->toString() << std::endl;
    std::cout << "    " << divElement->toString() << std::endl;
    std::cout << "      " << divText->toString() << std::endl;
    
    // 测试节点验证
    std::cout << "\nNode Validation:" << std::endl;
    std::cout << "HTML element valid: " << (htmlElement->validate() ? "true" : "false") << std::endl;
    std::cout << "Div element valid: " << (divElement->validate() ? "true" : "false") << std::endl;
    std::cout << "Div has class 'welcome': " << (divElement->hasClass("welcome") ? "true" : "false") << std::endl;
    std::cout << "Div has ID 'main-content': " << (divElement->hasID() ? "true" : "false") << std::endl;
    
    // 测试上下文功能
    std::cout << "\nContext Testing:" << std::endl;
    context->setConfiguration("DEBUG_MODE", "true");
    context->addVariable("siteName", "CHTL Test Site");
    context->addTemplate("Button", "Element", "button { class: btn; }");
    
    std::cout << "Debug mode: " << context->getConfiguration("DEBUG_MODE") << std::endl;
    std::cout << "Site name: " << context->getVariable("siteName") << std::endl;
    std::cout << "Button template: " << context->getTemplate("Button", "Element") << std::endl;
    
    std::cout << "\nCHTL Compiler test completed successfully!" << std::endl;
    
    return 0;
}