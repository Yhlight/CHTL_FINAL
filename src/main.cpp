#include <iostream>
#include <string>
#include <memory>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/Scanner/UnifiedScanner.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "Usage: " << programName << " [options] <input_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  -o, --output <file>     Output file path" << std::endl;
    std::cout << std::endl;
    std::cout << "  -d, --debug             Enable debug mode" << std::endl;
    std::cout << "  -v, --verbose           Enable verbose output" << std::endl;
    std::cout << "  --default-struct        Generate default HTML structure" << std::endl;
    std::cout << "  --no-css                Disable CSS generation" << std::endl;
    std::cout << "  --no-js                 Disable JavaScript generation" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " input.chtl" << std::endl;
    std::cout << "  " << programName << " -o output.html input.chtl" << std::endl;
    std::cout << "  " << programName << " -d --verbose input.chtl" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile;
    bool debugMode = false;
    bool verboseMode = false;
    bool defaultStruct = false;
    bool noCSS = false;
    bool noJS = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a file path" << std::endl;
                return 1;
            }
        } else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verboseMode = true;
        } else if (arg == "--default-struct") {
            defaultStruct = true;
        } else if (arg == "--no-css") {
            noCSS = true;
        } else if (arg == "--no-js") {
            noJS = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified" << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    // 设置输出文件
    if (outputFile.empty()) {
        size_t dotPos = inputFile.find_last_of('.');
        if (dotPos != std::string::npos) {
            outputFile = inputFile.substr(0, dotPos) + ".html";
        } else {
            outputFile = inputFile + ".html";
        }
    }
    
    try {
        // 创建CHTL上下文
        auto context = std::make_shared<CHTLContext>();
        context->initialize();
        context->setDebugMode(debugMode);
        
        if (verboseMode) {
            std::cout << "CHTL Compiler starting..." << std::endl;
            std::cout << "Input file: " << inputFile << std::endl;
            std::cout << "Output file: " << outputFile << std::endl;
            std::cout << "Debug mode: " << (debugMode ? "enabled" : "disabled") << std::endl;
        }
        
        // 添加源文件
        if (!context->addSourceFile(inputFile)) {
            std::cerr << "Error: Failed to add source file " << inputFile << std::endl;
            return 1;
        }
        
        // 读取源文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
            return 1;
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
        file.close();
        
        if (verboseMode) {
            std::cout << "Source code loaded (" << sourceCode.length() << " characters)" << std::endl;
        }
        
        // 使用统一扫描器分离代码片段
        UnifiedScanner scanner;
        scanner.setDebugMode(debugMode);
        auto fragments = scanner.scan(sourceCode);
        
        if (verboseMode) {
            std::cout << "Code fragments identified: " << fragments.size() << std::endl;
            for (const auto& fragment : fragments) {
                std::cout << "  Type: " << static_cast<int>(fragment.type) 
                          << ", Lines: " << fragment.startLine << "-" << fragment.endLine << std::endl;
            }
        }
        
        // 处理CHTL片段
        for (const auto& fragment : fragments) {
            if (fragment.type == CodeFragmentType::CHTL) {
                if (verboseMode) {
                    std::cout << "Processing CHTL fragment..." << std::endl;
                }
                
                // 创建词法分析器
                CHTLLexer lexer(context);
                lexer.setDebugMode(debugMode);
                
                // 词法分析
                auto tokens = lexer.tokenize(fragment.content);
                
                if (verboseMode) {
                    std::cout << "Tokens generated: " << tokens.size() << std::endl;
                }
                
                // 这里应该进行语法分析和代码生成
                // 目前只是演示基本流程
            }
        }
        
        // 检查错误
        if (context->hasErrors()) {
            std::cerr << "Compilation errors:" << std::endl;
            for (const auto& error : context->getErrors()) {
                std::cerr << "  " << error << std::endl;
            }
            return 1;
        }
        
        // 输出警告
        if (!context->getWarnings().empty()) {
            std::cout << "Compilation warnings:" << std::endl;
            for (const auto& warning : context->getWarnings()) {
                std::cout << "  " << warning << std::endl;
            }
        }
        
        // 生成输出文件
        std::ofstream output(outputFile);
        if (!output.is_open()) {
            std::cerr << "Error: Cannot create output file " << outputFile << std::endl;
            return 1;
        }
        
        // 简单的HTML输出（演示用）
        output << "<!DOCTYPE html>" << std::endl;
        output << "<html>" << std::endl;
        output << "<head>" << std::endl;
        output << "  <meta charset=\"UTF-8\">" << std::endl;
        output << "  <title>CHTL Output</title>" << std::endl;
        output << "</head>" << std::endl;
        output << "<body>" << std::endl;
        output << "  <!-- Generated by CHTL Compiler v1.0.0 -->" << std::endl;
        output << "  <p>CHTL compilation successful!</p>" << std::endl;
        output << "  <p>Input file: " << inputFile << "</p>" << std::endl;
        output << "  <p>Fragments processed: " << fragments.size() << "</p>" << std::endl;
        output << "</body>" << std::endl;
        output << "</html>" << std::endl;
        
        output.close();
        
        if (verboseMode) {
            std::cout << "Compilation completed successfully!" << std::endl;
            std::cout << "Output written to: " << outputFile << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}