#include <iostream>
#include <fstream>
#include <memory>
#include "CHTL/CHTLCli.h"

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
    // 创建 CHTL 上下文
    auto context = std::make_shared<CHTLContext>();
    
    // 创建命令行应用程序
    CliApplication app(context);
    
    // 初始化应用程序
    app.initialize();
    
    // 运行应用程序
    app.run(argc, argv);
    
    // 关闭应用程序
    app.shutdown();
    
    return 0;
}