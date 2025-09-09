#include "Test/CHTLTest.h"
#include "CLI/CHTLCli.h"
#include "Util/FileSystem/FileSystem.h"
#include <iostream>

namespace CHTL {

class CHTLCliTest {
public:
    static void runTests() {
        CHTLTestSuite suite("CHTL CLI Tests");
        
        // CLI初始化测试
        suite.addTest("CLI Initialization", []() {
            CHTLCli cli;
            
            return TestAssert::assertTrue(true, "CLI should initialize without errors");
        });
        
        // 帮助命令测试
        suite.addTest("Help Command", []() {
            CHTLCli cli;
            std::string help = cli.getHelpText();
            
            return TestAssert::assertNotEmpty(help, "Help text should not be empty") &&
                   TestAssert::assertContains(help, "CHTL Compiler", "Should contain compiler name") &&
                   TestAssert::assertContains(help, "compile", "Should contain compile command") &&
                   TestAssert::assertContains(help, "help", "Should contain help command");
        });
        
        // 版本命令测试
        suite.addTest("Version Command", []() {
            CHTLCli cli;
            std::string version = cli.getVersionText();
            
            return TestAssert::assertNotEmpty(version, "Version text should not be empty") &&
                   TestAssert::assertContains(version, "CHTL Compiler", "Should contain compiler name") &&
                   TestAssert::assertContains(version, "v1.0.0", "Should contain version number");
        });
        
        // 选项设置测试
        suite.addTest("Option Setting", []() {
            CHTLCli cli;
            
            cli.setDebugMode(true);
            cli.setPrettyPrint(false);
            cli.setIncludeComments(false);
            cli.setOutputPath("test.html");
            
            return TestAssert::assertTrue(true, "Options should be set without errors");
        });
        
        // 文件验证测试
        suite.addTest("File Validation", []() {
            CHTLCli cli;
            
            // 创建测试文件
            TestUtils::writeTestFile("test.chtl", "html { div { \"Hello\" } }");
            
            bool valid = cli.validateInputFile("test.chtl");
            bool invalid = cli.validateInputFile("test.txt");
            bool notExist = cli.validateInputFile("nonexistent.chtl");
            
            // 清理测试文件
            TestUtils::deleteTestFile("test.chtl");
            
            return TestAssert::assertTrue(valid, "Valid CHTL file should pass validation") &&
                   TestAssert::assertFalse(invalid, "Non-CHTL file should fail validation") &&
                   TestAssert::assertFalse(notExist, "Non-existent file should fail validation");
        });
        
        // 输出文件名生成测试
        suite.addTest("Output File Name Generation", []() {
            CHTLCli cli;
            
            std::string output1 = cli.getOutputFileName("test.chtl");
            std::string output2 = cli.getOutputFileName("test");
            
            return TestAssert::assertEquals("test.html", output1, "Should replace .chtl with .html") &&
                   TestAssert::assertEquals("test.html", output2, "Should add .html to filename without extension");
        });
        
        // 项目初始化测试
        suite.addTest("Project Initialization", []() {
            CHTLCli cli;
            
            // 测试项目初始化
            int result = cli.initCommand({"test-init-project"});
            
            // 检查项目目录是否创建
            FileSystem fs;
            bool dirExists = fs.directoryExists("test-init-project");
            bool fileExists = fs.fileExists("test-init-project/main.chtl");
            
            // 清理测试项目
            fs.removeDirectory("test-init-project");
            
            return TestAssert::assertEquals(0, result, "Init command should succeed") &&
                   TestAssert::assertTrue(dirExists, "Project directory should be created") &&
                   TestAssert::assertTrue(fileExists, "Main CHTL file should be created");
        });
        
        // 编译测试
        suite.addTest("Compilation", []() {
            CHTLCli cli;
            
            // 创建测试文件
            std::string testCode = "html { div { \"Hello, World!\" } }";
            TestUtils::writeTestFile("test-compile.chtl", testCode);
            
            // 设置输出路径
            cli.setOutputPath("test-compile.html");
            
            // 测试编译
            int result = cli.compileCommand({"test-compile.chtl"});
            
            // 检查输出文件
            FileSystem fs;
            bool outputExists = fs.fileExists("test-compile.html");
            
            // 清理测试文件
            TestUtils::deleteTestFile("test-compile.chtl");
            TestUtils::deleteTestFile("test-compile.html");
            
            return TestAssert::assertEquals(0, result, "Compilation should succeed") &&
                   TestAssert::assertTrue(outputExists, "Output file should be created");
        });
        
        // 错误处理测试
        suite.addTest("Error Handling", []() {
            CHTLCli cli;
            
            // 测试编译不存在的文件
            int result = cli.compileCommand({"nonexistent.chtl"});
            
            return TestAssert::assertNotEquals(0, result, "Compilation should fail for non-existent file");
        });
        
        suite.run();
    }
};

} // namespace CHTL