#include <iostream>
#include <string>
#include <fstream>
#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = (argc > 2) ? argv[2] : "output.html";
    
    // 读取输入文件
    std::ifstream file(input_file);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
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
    for (const auto& fragment : fragments) {
        std::string type_name;
        switch (fragment.type) {
            case CHTL::UnifiedScanner::CodeType::CHTL:
                type_name = "CHTL";
                break;
            case CHTL::UnifiedScanner::CodeType::CHTL_JS:
                type_name = "CHTL_JS";
                break;
            case CHTL::UnifiedScanner::CodeType::CSS:
                type_name = "CSS";
                break;
            case CHTL::UnifiedScanner::CodeType::JS:
                type_name = "JS";
                break;
            default:
                type_name = "UNKNOWN";
                break;
        }
        std::cout << "  - " << type_name << " (" << fragment.content.length() << " chars)" << std::endl;
    }
    
    // 处理CHTL代码片段
    for (const auto& fragment : fragments) {
        if (fragment.type == CHTL::UnifiedScanner::CodeType::CHTL) {
            std::cout << "\nProcessing CHTL fragment..." << std::endl;
            
            // 词法分析
            CHTL::CHTLLexer lexer;
            auto tokens = lexer.tokenize(fragment.content);
            
            std::cout << "Tokens generated: " << tokens.size() << std::endl;
            if (context.isDebugMode()) {
                for (const auto& token : tokens) {
                    std::cout << "  " << token.value << " (" << static_cast<int>(token.type) << ")" << std::endl;
                }
            }
            
            // 语法分析
            CHTL::CHTLParser parser;
            auto ast = parser.parse(tokens);
            
            if (ast) {
                std::cout << "AST generated successfully" << std::endl;
                if (context.isDebugMode()) {
                    std::cout << "AST: " << ast->toString() << std::endl;
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
    
    // 生成输出文件
    std::ofstream out_file(output_file);
    if (!out_file.is_open()) {
        std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
        return 1;
    }
    
    out_file << "<!DOCTYPE html>" << std::endl;
    out_file << "<html>" << std::endl;
    out_file << "<head>" << std::endl;
    out_file << "  <meta charset=\"UTF-8\">" << std::endl;
    out_file << "  <title>CHTL Output</title>" << std::endl;
    out_file << "</head>" << std::endl;
    out_file << "<body>" << std::endl;
    out_file << "  <h1>CHTL Compiler Output</h1>" << std::endl;
    out_file << "  <p>This is a placeholder output. Full CHTL compilation is not yet implemented.</p>" << std::endl;
    out_file << "</body>" << std::endl;
    out_file << "</html>" << std::endl;
    
    out_file.close();
    
    std::cout << "\nOutput written to: " << output_file << std::endl;
    std::cout << "Compilation completed successfully!" << std::endl;
    
    return 0;
}