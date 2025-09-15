#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string input_file = argv[1];
    std::string output_file = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // 读取输入文件
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // 使用统一扫描器扫描代码片段
        UnifiedScanner scanner;
        scanner.setInput(content);
        std::vector<CodeFragment> fragments = scanner.scan();
        
        if (scanner.hasErrors()) {
            std::cerr << "Scanner errors:" << std::endl;
            for (const auto& error : scanner.getErrors()) {
                std::cerr << error << std::endl;
            }
            return 1;
        }
        
        // 处理每个代码片段
        std::string output;
        for (const auto& fragment : fragments) {
            switch (fragment.type) {
                case FragmentType::CHTL: {
                    // 处理CHTL代码
                    CHTLLexer lexer;
                    lexer.setInput(fragment.content);
                    std::vector<Token> tokens = lexer.tokenize();
                    
                    CHTLParser parser;
                    parser.setInput(fragment.content);
                    std::shared_ptr<CHTLNode> ast = parser.parse();
                    
                    if (parser.hasErrors()) {
                        std::cerr << "Parser errors:" << std::endl;
                        for (const auto& error : parser.getErrors()) {
                            std::cerr << error << std::endl;
                        }
                        continue;
                    }
                    
                    if (ast) {
                        // 生成HTML输出
                        CHTLContext context;
                        std::string html = ast->generateHTML(context);
                        output += html;
                    }
                    break;
                }
                case FragmentType::CHTL_JS: {
                    // 处理CHTL JS代码
                    output += "<script type=\"text/javascript\">\n";
                    output += fragment.content;
                    output += "\n</script>\n";
                    break;
                }
                case FragmentType::CSS: {
                    // 处理CSS代码
                    output += "<style>\n";
                    output += fragment.content;
                    output += "\n</style>\n";
                    break;
                }
                case FragmentType::HTML: {
                    // 直接输出HTML
                    output += fragment.content;
                    break;
                }
                case FragmentType::JAVASCRIPT: {
                    // 处理JavaScript代码
                    output += "<script type=\"text/javascript\">\n";
                    output += fragment.content;
                    output += "\n</script>\n";
                    break;
                }
                case FragmentType::TEXT: {
                    // 直接输出文本
                    output += fragment.content;
                    break;
                }
                default:
                    break;
            }
        }
        
        // 写入输出文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            std::cerr << "Error: Cannot create output file: " << output_file << std::endl;
            return 1;
        }
        
        out_file << output;
        out_file.close();
        
        std::cout << "Compilation successful. Output written to: " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}