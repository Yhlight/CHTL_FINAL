#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "CHTL/CHTLLexer.h"
#include "CHTL/UnifiedScanner.h"
#include "CHTL/CompilerDispatcher.h"
#include "CHTL/BaseNode.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> [options]" << std::endl;
        std::cerr << "Options:" << std::endl;
        std::cerr << "  --inline          Inline CSS and JS" << std::endl;
        std::cerr << "  --inline-css      Inline CSS only" << std::endl;
        std::cerr << "  --inline-js       Inline JS only" << std::endl;
        std::cerr << "  --default-struct  Generate default HTML structure" << std::endl;
        std::cerr << "  --debug           Enable debug mode" << std::endl;
        std::cerr << "  --output <file>   Specify output file" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = "output.html";
    bool inlineMode = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool defaultStruct = false;
    bool debugMode = false;
    
    // 解析命令行参数
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--inline") {
            inlineMode = true;
            inlineCSS = true;
            inlineJS = true;
        } else if (arg == "--inline-css") {
            inlineCSS = true;
        } else if (arg == "--inline-js") {
            inlineJS = true;
        } else if (arg == "--default-struct") {
            defaultStruct = true;
        } else if (arg == "--debug") {
            debugMode = true;
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << inputFile << std::endl;
        return 1;
    }
    
    std::string sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "CHTL Compiler v2.0.0" << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 创建编译器调度器
        CompilerDispatcher dispatcher;
        
        // 配置编译选项
        CompilerDispatcher::CompileConfig config;
        config.enableInlineCSS = inlineCSS;
        config.enableInlineJS = inlineJS;
        config.enableDefaultStruct = defaultStruct;
        config.enableDebugMode = debugMode;
        config.outputDirectory = ".";
        
        // 执行编译
        auto result = dispatcher.compile(sourceCode, config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        if (result.success) {
            // 写入输出文件
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << result.htmlOutput;
                outFile.close();
                std::cout << "Output written to: " << outputFile << std::endl;
            } else {
                std::cerr << "Error: Cannot create output file " << outputFile << std::endl;
                return 1;
            }
            
            // 写入CSS文件（如果未内联）
            if (!inlineCSS && !result.cssOutput.empty()) {
                std::ofstream cssFile("styles.css");
                if (cssFile.is_open()) {
                    cssFile << result.cssOutput;
                    cssFile.close();
                    std::cout << "CSS written to: styles.css" << std::endl;
                }
            }
            
            // 写入JS文件（如果未内联）
            if (!inlineJS && !result.jsOutput.empty()) {
                std::ofstream jsFile("scripts.js");
                if (jsFile.is_open()) {
                    jsFile << result.jsOutput;
                    jsFile.close();
                    std::cout << "JS written to: scripts.js" << std::endl;
                }
            }
            
            std::cout << "Compilation completed successfully!" << std::endl;
            std::cout << "Compile time: " << duration.count() << "ms" << std::endl;
            std::cout << "Memory usage: " << result.memoryUsage << " bytes" << std::endl;
            
            // 显示警告
            if (!result.warnings.empty()) {
                std::cout << "Warnings:" << std::endl;
                for (const auto& warning : result.warnings) {
                    std::cout << "  " << warning << std::endl;
                }
            }
            
        } else {
            std::cerr << "Compilation failed: " << result.errorMessage << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}