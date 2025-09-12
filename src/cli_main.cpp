#include <iostream>
#include <string>
#include "CLI/CHTLCli.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    CHTLCli cli;
    
    // 解析命令行参数
    CommandType command = cli.parseArguments(argc, argv);
    
    // 执行命令
    int result = cli.executeCommand(command);
    
    // 显示错误信息
    auto errors = cli.getErrors();
    if (!errors.empty()) {
        std::cerr << std::endl << "Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  " << error << std::endl;
        }
    }
    
    return result;
}