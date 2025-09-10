#include "CLITool.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        CHTL::CLITool tool;
        return tool.run(argc, argv);
    } catch (const std::exception& e) {
        CHTL::ColorOutput::printError("程序异常: " + std::string(e.what()));
        return 1;
    } catch (...) {
        CHTL::ColorOutput::printError("未知错误");
        return 1;
    }
}