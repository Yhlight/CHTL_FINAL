#include <iostream>
#include <string>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test CHTL Compilation ===" << std::endl;
    
    // 测试CHTL代码
    std::string chtl_code = R"(
        html {
            head {
                title { text { "CHTL Test Page" } }
            }
            body {
                div {
                    text { "Hello from CHTL!" }
                    style {
                        color: red;
                        font-size: 20px;
                    }
                }
            }
        }
    )";
    
    std::cout << "Source CHTL code:" << std::endl;
    std::cout << chtl_code << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    try {
        // 1. 统一扫描器
        std::cout << "1. Unified Scanner..." << std::endl;
        UnifiedScanner scanner;
        auto fragments = scanner.scan(chtl_code);
        std::cout << "Found " << fragments.size() << " fragments" << std::endl;
        
        // 2. CHTL编译器
        std::cout << "2. CHTL Compiler..." << std::endl;
        CHTLLexer lexer;
        CHTLParser parser;
        CHTLGenerator generator;
        
        for (const auto& fragment : fragments) {
            if (fragment.type == UnifiedScanner::CodeType::CHTL) {
                std::cout << "Processing CHTL fragment: " << fragment.content << std::endl;
                
                // 词法分析
                auto tokens = lexer.tokenize(fragment.content);
                std::cout << "Lexer completed, found " << tokens.size() << " tokens" << std::endl;
                
                // 语法分析
                auto ast = parser.parse(tokens);
                if (ast) {
                    std::cout << "Parser completed successfully" << std::endl;
                    
                    // 代码生成
                    std::string html = generator.generateHTML(ast);
                    std::cout << "Generated HTML: " << html << std::endl;
                } else {
                    std::cout << "Parser failed" << std::endl;
                    auto errors = parser.getErrors();
                    for (const auto& error : errors) {
                        std::cout << "Error: " << error << std::endl;
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Test CHTL Compilation Complete!" << std::endl;
    
    return 0;
}