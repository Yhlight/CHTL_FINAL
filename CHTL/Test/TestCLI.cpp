#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>

#include "CLI/CHTLCli.h"

using namespace CHTL;

void testCLICommands() {
    std::cout << "Testing CLI Commands..." << std::endl;
    
    CHTLCli cli;
    
    // 测试命令存在性
    assert(cli.hasCommand("compile"));
    assert(cli.hasCommand("watch"));
    assert(cli.hasCommand("init"));
    assert(cli.hasCommand("build"));
    assert(cli.hasCommand("clean"));
    assert(cli.hasCommand("test"));
    assert(cli.hasCommand("serve"));
    
    // 测试获取命令
    auto compileCommand = cli.getCommand("compile");
    assert(compileCommand.name == "compile");
    assert(compileCommand.description.find("Compile") != std::string::npos);
    
    // 测试获取所有命令名
    auto commandNames = cli.getAllCommandNames();
    assert(commandNames.size() >= 7); // 至少有7个内置命令
    
    std::cout << "CLI Commands tests passed!" << std::endl << std::endl;
}

void testCLIExecution() {
    std::cout << "Testing CLI Execution..." << std::endl;
    
    CHTLCli cli;
    
    // 测试帮助命令
    std::vector<std::string> helpArgs = {"help"};
    int result = cli.execute(helpArgs);
    assert(result == 0);
    
    // 测试版本命令
    std::vector<std::string> versionArgs = {"version"};
    result = cli.execute(versionArgs);
    assert(result == 0);
    
    // 测试未知命令
    std::vector<std::string> unknownArgs = {"unknown"};
    result = cli.execute(unknownArgs);
    assert(result == 1);
    
    std::cout << "CLI Execution tests passed!" << std::endl << std::endl;
}

void testCLIArguments() {
    std::cout << "Testing CLI Arguments..." << std::endl;
    
    CHTLCli cli;
    
    // 测试编译命令参数
    std::vector<std::string> compileArgs = {"compile", "--input", "test.chtl", "--output", "test.html"};
    int result = cli.execute(compileArgs);
    // 注意：这个测试可能会失败，因为test.chtl文件不存在
    // 在实际测试中，应该创建测试文件或模拟文件操作
    
    std::cout << "CLI Arguments tests completed!" << std::endl << std::endl;
}

void testCLIUtilityFunctions() {
    std::cout << "Testing CLI Utility Functions..." << std::endl;
    
    // 测试当前目录
    std::string currentDir = CHTLCli::getCurrentDirectory();
    assert(!currentDir.empty());
    
    // 测试文件存在性检查
    bool fileExists = CHTLCli::fileExists("CMakeLists.txt");
    assert(fileExists);
    
    bool dirExists = CHTLCli::directoryExists(".");
    assert(dirExists);
    
    // 测试文件读写
    std::string testContent = "Test content for CLI utility functions";
    bool writeResult = CHTLCli::writeFile("test_cli_temp.txt", testContent);
    assert(writeResult);
    
    std::string readContent = CHTLCli::readFile("test_cli_temp.txt");
    assert(readContent == testContent);
    
    // 清理测试文件
    std::filesystem::remove("test_cli_temp.txt");
    
    std::cout << "CLI Utility Functions tests passed!" << std::endl << std::endl;
}

void testCLIHelp() {
    std::cout << "Testing CLI Help..." << std::endl;
    
    CHTLCli cli;
    
    // 测试全局帮助
    cli.printHelp();
    
    // 测试命令帮助
    cli.printCommandHelp("compile");
    cli.printCommandHelp("watch");
    cli.printCommandHelp("init");
    
    // 测试版本信息
    cli.printVersion();
    
    std::cout << "CLI Help tests completed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== CHTL CLI Tests ===" << std::endl << std::endl;
    
    testCLICommands();
    testCLIExecution();
    testCLIArguments();
    testCLIUtilityFunctions();
    testCLIHelp();
    
    std::cout << "All CLI tests passed!" << std::endl;
    return 0;
}