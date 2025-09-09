#include <iostream>
#include <fstream>
#include <memory>
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"

using namespace CHTL;

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "Usage: " << programName << " [options] <input_file>\n";
    std::cout << "\nOptions:\n";
    std::cout << "  -o, --output <file>    Output file path\n";
    std::cout << "  -d, --debug           Enable debug mode\n";
    std::cout << "  -h, --help            Show this help message\n";
    std::cout << "  -v, --version         Show version information\n";
}

void printVersion() {
    std::cout << "CHTL Compiler v1.0.0\n";
    std::cout << "A C++ based hypertext language compiler\n";
    std::cout << "MIT License\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string inputFile;
    std::string outputFile;
    bool debugMode = false;
    
    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return 0;
        } else if (arg == "-d" || arg == "--debug") {
            debugMode = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an output file path\n";
                return 1;
            }
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
    
    try {
        // 创建编译上下文
        auto context = std::make_shared<CHTLContext>();
        context->setDebugMode(debugMode);
        
        if (!outputFile.empty()) {
            context->setOutputPath(outputFile);
        }
        
        // 读取输入文件
        std::string source = readFile(inputFile);
        
        if (debugMode) {
            std::cout << "Debug: Reading file " << inputFile << " (" << source.length() << " characters)\n";
        }
        
        // 词法分析
        CHTLLexer lexer(context);
        auto tokens = lexer.tokenize(source);
        
        if (debugMode) {
            std::cout << "Debug: Generated " << tokens.size() << " tokens\n";
        }
        
        // 语法分析
        CHTLParser parser(context);
        auto ast = parser.parse(tokens);
        
        if (context->hasErrors()) {
            std::cerr << "Compilation errors:\n";
            for (const auto& error : context->getErrors()) {
                std::cerr << "Error: " << error << "\n";
            }
            return 1;
        }
        
        if (debugMode) {
            std::cout << "Debug: AST generated successfully\n";
        }
        
        // 代码生成
        CHTLGenerator generator(context);
        auto generatedCode = generator.generate(ast);
        
        // 输出结果
        if (outputFile.empty()) {
            // 默认输出到控制台
            std::cout << "=== HTML ===\n";
            std::cout << generatedCode.html << "\n\n";
            std::cout << "=== CSS ===\n";
            std::cout << generatedCode.css << "\n\n";
            std::cout << "=== JavaScript ===\n";
            std::cout << generatedCode.javascript << "\n";
        } else {
            // 输出到文件
            writeFile(outputFile + ".html", generatedCode.html);
            writeFile(outputFile + ".css", generatedCode.css);
            writeFile(outputFile + ".js", generatedCode.javascript);
            
            std::cout << "Compilation successful!\n";
            std::cout << "Generated files:\n";
            std::cout << "  " << outputFile << ".html\n";
            std::cout << "  " << outputFile << ".css\n";
            std::cout << "  " << outputFile << ".js\n";
        }
        
        // 显示警告
        if (!context->getWarnings().empty()) {
            std::cout << "\nWarnings:\n";
            for (const auto& warning : context->getWarnings()) {
                std::cout << "Warning: " << warning << "\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}