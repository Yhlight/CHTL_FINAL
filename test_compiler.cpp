#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

int main() {
    std::cout << "CHTL Compiler Test" << std::endl;
    std::cout << "==================" << std::endl;
    
    // 读取示例文件
    std::ifstream file("examples/basic.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open examples/basic.chtl" << std::endl;
        return 1;
    }
    
    std::string source_code((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "Source code loaded: " << source_code.length() << " characters" << std::endl;
    
    // 创建编译器上下文
    CHTL::CHTLContext context;
    context.setDebugMode(true);
    
    // 使用统一扫描器分离代码
    CHTL::UnifiedScanner scanner;
    auto fragments = scanner.scan(source_code);
    
    std::cout << "Code fragments identified: " << fragments.size() << std::endl;
    
    // 处理CHTL代码片段
    for (const auto& fragment : fragments) {
        if (fragment.type == CHTL::UnifiedScanner::CodeType::CHTL) {
            std::cout << "\nProcessing CHTL fragment..." << std::endl;
            
            // 词法分析
            CHTL::CHTLLexer lexer;
            auto tokens = lexer.tokenize(fragment.content);
            
            std::cout << "Tokens generated: " << tokens.size() << std::endl;
            
            // 语法分析
            CHTL::CHTLParser parser;
            auto ast = parser.parse(tokens);
            
            if (ast) {
                std::cout << "AST generated successfully" << std::endl;
                
                // 代码生成
                CHTL::CHTLGenerator generator;
                generator.setDefaultStructure(true);
                
                std::string html = generator.generateHTML(ast);
                std::string css = generator.generateCSS(ast);
                std::string js = generator.generateJavaScript(ast);
                
                std::cout << "Generated HTML length: " << html.length() << std::endl;
                std::cout << "Generated CSS length: " << css.length() << std::endl;
                std::cout << "Generated JS length: " << js.length() << std::endl;
                
                // 保存输出
                std::ofstream out_file("test_output.html");
                if (out_file.is_open()) {
                    out_file << html;
                    out_file.close();
                    std::cout << "Output saved to test_output.html" << std::endl;
                }
            } else {
                std::cerr << "Error: Failed to parse CHTL code" << std::endl;
                const auto& errors = parser.getErrors();
                for (const auto& error : errors) {
                    std::cerr << "  " << error << std::endl;
                }
            }
        }
    }
    
    std::cout << "\nTest completed!" << std::endl;
    return 0;
}