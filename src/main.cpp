#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLLoader.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << programName << " [options] <input_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <dir>     Set output directory (default: current directory)\n";
    std::cout << "  -m, --minify           Minify output\n";
    std::cout << "  -d, --debug            Enable debug mode\n";
    std::cout << "  -v, --verbose          Verbose output\n";
    std::cout << "  -h, --help             Show this help message\n";
    std::cout << "  --version              Show version information\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " input.chtl\n";
    std::cout << "  " << programName << " -o output/ -m input.chtl\n";
    std::cout << "  " << programName << " --debug --verbose input.chtl\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Copyright (c) 2024 CHTL Team\n";
    std::cout << "MIT License\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputDir = ".";
    bool minify = false;
    bool debug = false;
    bool verbose = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputDir = argv[++i];
            } else {
                std::cerr << "Error: --output requires a directory path\n";
                return 1;
            }
        } else if (arg == "-m" || arg == "--minify") {
            minify = true;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        // 创建编译器调度器
        auto dispatcher = std::make_shared<CompilerDispatcher>();
        
        // 设置编译选项
        dispatcher->setCompileOptions(minify, debug, false);
        dispatcher->setOutputDirectory(outputDir);
        
        if (verbose) {
            std::cout << "Compiling " << inputFile << "...\n";
            std::cout << "Output directory: " << outputDir << "\n";
            std::cout << "Minify: " << (minify ? "yes" : "no") << "\n";
            std::cout << "Debug: " << (debug ? "yes" : "no") << "\n";
        }
        
        // 编译文件
        CompilationResult result = dispatcher->compileFile(inputFile);
        
        if (result.success) {
            std::cout << "Compilation successful!\n";
            
            if (verbose) {
                std::cout << "Generated files:\n";
                if (!result.html.empty()) {
                    std::cout << "  - HTML: " << result.html.length() << " characters\n";
                }
                if (!result.css.empty()) {
                    std::cout << "  - CSS: " << result.css.length() << " characters\n";
                }
                if (!result.javascript.empty()) {
                    std::cout << "  - JavaScript: " << result.javascript.length() << " characters\n";
                }
            }
            
            // 输出统计信息
            auto stats = dispatcher->getCompilationStats();
            if (verbose && !stats.empty()) {
                std::cout << "\nCompilation statistics:\n";
                for (const auto& stat : stats) {
                    std::cout << "  " << stat.first << ": " << stat.second << "\n";
                }
            }
            
        } else {
            std::cerr << "Compilation failed!\n";
            
            if (!result.errors.empty()) {
                std::cerr << "\nErrors:\n";
                for (const auto& error : result.errors) {
                    std::cerr << "  " << error << "\n";
                }
            }
            
            if (!result.warnings.empty()) {
                std::cerr << "\nWarnings:\n";
                for (const auto& warning : result.warnings) {
                    std::cerr << "  " << warning << "\n";
                }
            }
            
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}