#include "CHTLCli.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    try {
        CHTL::CHTLCli cli;
        
        // 设置程序信息
        cli.setProgramName("chtl");
        cli.setVersion("1.0.0");
        cli.setDescription("CHTL (C++ HyperText Language) Compiler and Development Tools");
        
        // 执行命令行
        int result = cli.execute(argc, argv);
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}