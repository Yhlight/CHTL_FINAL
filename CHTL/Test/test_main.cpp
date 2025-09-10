#include "TestFramework.h"
#include "UtilTest.h"
#include "TokenTest.h"
#include "AstTest.h"
#include <iostream>

int main() {
    std::cout << "CHTL测试系统" << std::endl;
    std::cout << "=============" << std::endl;
    std::cout << std::endl;
    
    try {
        CHTL::Test::TestFramework framework;
        
        // 添加测试套件
        framework.addTestSuite(CHTL::Test::UtilTest::createTestSuite());
        framework.addTestSuite(CHTL::Test::TokenTest::createTestSuite());
        framework.addTestSuite(CHTL::Test::AstTest::createTestSuite());
        
        // 运行所有测试
        framework.runAllTests();
        
        std::cout << "所有测试完成！" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "测试过程中发生未知错误" << std::endl;
        return 1;
    }
}