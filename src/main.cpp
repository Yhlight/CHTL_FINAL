#include <iostream>
#include <fstream>
#include <string>
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> [-o output.html]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    
    // 解析命令行参数
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[i + 1];
            i++;
        }
    }
    
    try {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
            return 1;
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
        file.close();
        
        // 使用统一扫描器分离代码
        UnifiedScanner scanner;
        auto fragments = scanner.scan(sourceCode);
        
        // 处理CHTL代码片段
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        
        for (const auto& fragment : fragments) {
            if (fragment.type == CodeType::CHTL) {
                // 词法分析
                auto lexer = std::make_unique<CHTLLexer>();
                lexer->setSource(fragment.content);
                auto tokens = lexer->tokenize();
                
                // 语法分析
                auto parser = std::make_unique<CHTLParser>();
                parser->setLexer(std::move(lexer));
                auto ast = parser->parse();
                
                // 代码生成
                CHTLGenerator generator;
                htmlOutput += generator.generateHTML(ast.get());
                cssOutput += generator.generateCSS(ast.get());
                jsOutput += generator.generateJavaScript(ast.get());
            }
            else if (fragment.type == CodeType::CSS) {
                cssOutput += fragment.content;
            }
            else if (fragment.type == CodeType::JS) {
                jsOutput += fragment.content;
            }
        }
        
        // 生成完整HTML文档
        std::stringstream completeHTML;
        completeHTML << "<!DOCTYPE html>\n<html>\n<head>\n";
        
        if (!cssOutput.empty()) {
            completeHTML << "<style>\n" << cssOutput << "\n</style>\n";
        }
        
        completeHTML << "</head>\n<body>\n";
        completeHTML << htmlOutput;
        completeHTML << "</body>\n";
        
        if (!jsOutput.empty()) {
            completeHTML << "<script>\n" << jsOutput << "\n</script>\n";
        }
        
        completeHTML << "</html>";
        
        // 写入输出文件
        std::ofstream outFile(outputFile);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot create output file " << outputFile << std::endl;
            return 1;
        }
        
        outFile << completeHTML.str();
        outFile.close();
        
        std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}