#include "CompilerDispatcher/CompilerDispatcher.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    std::cout << "====================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input_file> [output_directory]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputDir = argc > 2 ? argv[2] : "./output";
    
    // 创建编译器调度器
    CHTL::CompilerDispatcher compiler;
    compiler.setOutputDirectory(outputDir);
    compiler.setDebugMode(true);
    
    std::cout << "Compiling: " << inputFile << std::endl;
    std::cout << "Output directory: " << outputDir << std::endl;
    
    // 编译文件
    CHTL::CompileResult result = compiler.compileFile(inputFile);
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Generated files:" << std::endl;
        std::cout << "  - HTML: " << outputDir << "/" << inputFile.substr(0, inputFile.find_last_of('.')) << ".html" << std::endl;
        std::cout << "  - CSS: " << outputDir << "/" << inputFile.substr(0, inputFile.find_last_of('.')) << ".css" << std::endl;
        std::cout << "  - JS: " << outputDir << "/" << inputFile.substr(0, inputFile.find_last_of('.')) << ".js" << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        std::cout << "Errors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    if (!result.warnings.empty()) {
        std::cout << "Warnings:" << std::endl;
        for (const auto& warning : result.warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    return result.success ? 0 : 1;
}