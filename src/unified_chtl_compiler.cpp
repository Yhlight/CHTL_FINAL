#include "CHTL/CompilerDispatcher.h"
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        std::cerr << "       " << argv[0] << " --help" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    // 检查输入文件是否存在
    if (!std::filesystem::exists(input_file)) {
        std::cerr << "Error: Input file does not exist: " << input_file << std::endl;
        return 1;
    }

    // 创建编译器调度器
    CHTL::CompilerDispatcher dispatcher;

    // 编译文件
    auto result = dispatcher.compileFile(input_file, output_file);

    if (result.success) {
        std::cout << "Compilation successful. Output written to: " << output_file << std::endl;
        
        // 显示警告
        if (!result.warnings.empty()) {
            std::cout << "Warnings:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "  - " << warning << std::endl;
            }
        }
        
        return 0;
    } else {
        std::cerr << "Compilation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return 1;
    }
}