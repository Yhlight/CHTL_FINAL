#include <iostream>
#include <string>
#include "CLI/CHTLCli.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    std::cout << "Starting CHTL CLI debug..." << std::endl;
    
    CHTLCli cli;
    cli.setDebugMode(true);
    
    // 解析命令行参数
    std::cout << "Parsing arguments..." << std::endl;
    CommandType command = cli.parseArguments(argc, argv);
    std::cout << "Command type: " << static_cast<int>(command) << std::endl;
    
    // 执行命令
    std::cout << "Executing command..." << std::endl;
    int result = cli.executeCommand(command);
    std::cout << "Command result: " << result << std::endl;
    
    // 显示错误信息
    auto errors = cli.getErrors();
    if (!errors.empty()) {
        std::cerr << std::endl << "Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  " << error << std::endl;
        }
    }
    
    std::cout << "Debug complete." << std::endl;
    return result;
}