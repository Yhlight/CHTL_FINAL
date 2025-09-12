#include <iostream>
#include <fstream>
#include <memory>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/Scanner/UnifiedScanner.h"

using namespace CHTL;

int main() {
    // 读取测试文件
    std::ifstream file("test.chtl");
    std::string sourceCode((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Source code:" << std::endl;
    std::cout << sourceCode << std::endl;
    std::cout << "---" << std::endl;
    
    // 创建上下文
    auto context = std::make_shared<CHTLContext>();
    context->initialize();
    context->setDebugMode(true);
    
    // 使用统一扫描器
    UnifiedScanner scanner;
    scanner.setDebugMode(true);
    auto fragments = scanner.scan(sourceCode);
    
    std::cout << "Fragments found: " << fragments.size() << std::endl;
    for (const auto& fragment : fragments) {
        std::cout << "Type: " << static_cast<int>(fragment.type) 
                  << ", Content: " << fragment.content.substr(0, 100) << "..." << std::endl;
    }
    
    // 处理CHTL片段
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeFragmentType::CHTL) {
            std::cout << "Processing CHTL fragment..." << std::endl;
            
            // 词法分析
            CHTLLexer lexer(context);
            lexer.setDebugMode(true);
            auto tokens = lexer.tokenize(fragment.content);
            
            std::cout << "Tokens: " << tokens.size() << std::endl;
            for (size_t i = 0; i < tokens.size() && i < 10; ++i) {
                std::cout << "  " << i << ": " << tokens[i].toString() << std::endl;
            }
            
            // 语法分析
            CHTLParser parser(context);
            parser.setDebugMode(true);
            auto ast = parser.parse(tokens);
            
            if (ast) {
                std::cout << "AST generated successfully" << std::endl;
                std::cout << "Children: " << ast->getChildCount() << std::endl;
            } else {
                std::cout << "Failed to generate AST" << std::endl;
            }
        }
    }
    
    return 0;
}