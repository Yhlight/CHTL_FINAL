#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "Scanner/UnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] <input_file>\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>     Output file (default: index.html)\n";
    std::cout << "  -c, --css <file>        CSS output file (default: style.css)\n";
    std::cout << "  -j, --js <file>         JS output file (default: script.js)\n";
    std::cout << "  --inline                Inline CSS and JS into HTML\n";
    std::cout << "  --inline-css            Inline CSS into HTML\n";
    std::cout << "  --inline-js             Inline JS into HTML\n";
    std::cout << "  --minify                Minify output\n";
    std::cout << "  --default-struct        Include default HTML structure\n";
    std::cout << "  -d, --debug             Enable debug mode\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "A C++ implementation of the CHTL (C++ HyperText Language)\n";
    std::cout << "MIT License\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    std::string inputFile;
    std::string outputFile = "index.html";
    std::string cssFile = "style.css";
    std::string jsFile = "script.js";
    bool inlineAll = false;
    bool inlineCSS = false;
    bool inlineJS = false;
    bool minify = false;
    bool defaultStruct = false;
    bool debug = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a filename\n";
                return 1;
            }
        } else if (arg == "-c" || arg == "--css") {
            if (i + 1 < argc) {
                cssFile = argv[++i];
            } else {
                std::cerr << "Error: --css requires a filename\n";
                return 1;
            }
        } else if (arg == "-j" || arg == "--js") {
            if (i + 1 < argc) {
                jsFile = argv[++i];
            } else {
                std::cerr << "Error: --js requires a filename\n";
                return 1;
            }
        } else if (arg == "--inline") {
            inlineAll = true;
        } else if (arg == "--inline-css") {
            inlineCSS = true;
        } else if (arg == "--inline-js") {
            inlineJS = true;
        } else if (arg == "--minify") {
            minify = true;
        } else if (arg == "--default-struct") {
            defaultStruct = true;
        } else if (arg == "-d" || arg == "--debug") {
            debug = true;
        } else if (arg[0] != '-') {
            inputFile = arg;
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // 读取输入文件
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open input file " << inputFile << "\n";
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // 词法分析
        CHTLLexer lexer(source);
        TokenList tokens = lexer.tokenize();
        
        if (debug) {
            std::cout << "Tokens:\n";
            for (const auto& token : tokens) {
                std::cout << token->toString() << "\n";
            }
        }
        
        // 语法分析
        CHTLParser parser(tokens);
        NodePtr root = parser.parse();
        
        if (!root) {
            std::cerr << "Error: Failed to parse input file\n";
            return 1;
        }
        
        if (debug) {
            std::cout << "AST:\n";
            std::cout << root->toString() << "\n";
        }
        
        // 代码生成
        CHTLGenerator generator;
        GenerationOptions options;
        options.includeDefaultStructure = defaultStruct;
        options.inlineCSS = inlineAll || inlineCSS;
        options.inlineJS = inlineAll || inlineJS;
        options.minify = minify;
        options.debugMode = debug;
        options.htmlFileName = outputFile;
        options.cssFileName = cssFile;
        options.jsFileName = jsFile;
        
        GenerationResult result = generator.generate(root, options);
        
        // 检查错误
        if (result.hasErrors()) {
            std::cerr << "Errors:\n";
            for (const auto& error : result.errors) {
                std::cerr << "  " << error << "\n";
            }
            return 1;
        }
        
        // 输出警告
        if (result.hasWarnings()) {
            std::cout << "Warnings:\n";
            for (const auto& warning : result.warnings) {
                std::cout << "  " << warning << "\n";
            }
        }
        
        // 写入输出文件
        std::ofstream htmlFile(outputFile);
        if (htmlFile.is_open()) {
            htmlFile << result.html;
            htmlFile.close();
            std::cout << "HTML written to " << outputFile << "\n";
        } else {
            std::cerr << "Error: Cannot write to " << outputFile << "\n";
            return 1;
        }
        
        // 写入CSS文件
        if (!result.css.empty() && !options.inlineCSS) {
            std::ofstream cssFileStream(cssFile);
            if (cssFileStream.is_open()) {
                cssFileStream << result.css;
                cssFileStream.close();
                std::cout << "CSS written to " << cssFile << "\n";
            } else {
                std::cerr << "Error: Cannot write to " << cssFile << "\n";
                return 1;
            }
        }
        
        // 写入JS文件
        if (!result.js.empty() && !options.inlineJS) {
            std::ofstream jsFileStream(jsFile);
            if (jsFileStream.is_open()) {
                jsFileStream << result.js;
                jsFileStream.close();
                std::cout << "JS written to " << jsFile << "\n";
            } else {
                std::cerr << "Error: Cannot write to " << jsFile << "\n";
                return 1;
            }
        }
        
        std::cout << "Compilation successful!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}