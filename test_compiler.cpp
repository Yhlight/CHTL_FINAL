#include <iostream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
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
        
        std::cout << "All tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}