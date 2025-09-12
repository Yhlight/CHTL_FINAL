#include <iostream>
#include <string>
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "=== CHTL JS Compiler Test ===" << std::endl;
    
    // 测试CHTL JS代码
    std::string chtl_js_code = R"(
        listen {
            click: function() { console.log("Clicked!"); }
        }
        
        animate {
            duration: 1000,
            easing: "ease-in-out"
        }
        
        vir myVirtualObject = {
            name: "test",
            value: 42
        }
        
        util condition -> changeExpr -> thenExpr
    )";
    
    std::cout << "Source CHTL JS code:" << std::endl;
    std::cout << chtl_js_code << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    // 词法分析
    std::cout << "1. Lexical Analysis:" << std::endl;
    CHTLJSLexer lexer;
    auto tokens = lexer.tokenize(chtl_js_code);
    
    std::cout << "Found " << tokens.size() << " tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  " << static_cast<int>(token.type) << ": '" << token.value << "'" << std::endl;
    }
    std::cout << std::endl;
    
    // 语法分析
    std::cout << "2. Syntax Analysis:" << std::endl;
    CHTLJSParser parser;
    auto ast = parser.parse(tokens);
    
    if (ast) {
        std::cout << "AST created successfully" << std::endl;
        std::cout << "Root node type: " << static_cast<int>(ast->getType()) << std::endl;
        std::cout << "Root node name: " << ast->getName() << std::endl;
        std::cout << "Children count: " << ast->getChildren().size() << std::endl;
    } else {
        std::cout << "Failed to create AST" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    std::cout << std::endl;
    
    // 代码生成
    std::cout << "3. Code Generation:" << std::endl;
    CHTLJSGenerator generator;
    generator.setStrictMode(true);
    generator.setMinify(false);
    
    std::string javascript = generator.generateJavaScript(ast);
    std::cout << "Generated JavaScript:" << std::endl;
    std::cout << javascript << std::endl;
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "CHTL JS Compiler Test Complete!" << std::endl;
    
    return 0;
}