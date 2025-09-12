#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CLITools.h"

using namespace CHTL;

int main(int argc, char* argv[]) {
    // 创建CLI工具
    CHTLCompilerCLI cli;
    
    // 运行CLI
    int result = cli.run(argc, argv);
    
    return result;
}