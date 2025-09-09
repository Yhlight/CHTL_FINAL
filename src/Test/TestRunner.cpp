#include "Test/CHTLTest.h"
#include "Test/CHTLLexerTest.h"
#include "Test/CHTLParserTest.h"
#include "Test/CHTLGeneratorTest.h"
#include "Test/CHTLCliTest.h"
#include <iostream>

namespace CHTL {

class TestRunner {
public:
    static void runAllTests() {
        std::cout << "CHTL Compiler Test Suite" << std::endl;
        std::cout << "========================" << std::endl;
        
        // 运行所有测试套件
        CHTLLexerTest::runTests();
        CHTLParserTest::runTests();
        CHTLGeneratorTest::runTests();
        CHTLCliTest::runTests();
        
        std::cout << "\nAll tests completed!" << std::endl;
    }
    
    static void runSpecificTest(const std::string& testName) {
        std::cout << "Running specific test: " << testName << std::endl;
        std::cout << "=========================" << std::endl;
        
        if (testName == "lexer") {
            CHTLLexerTest::runTests();
        } else if (testName == "parser") {
            CHTLParserTest::runTests();
        } else if (testName == "generator") {
            CHTLGeneratorTest::runTests();
        } else if (testName == "cli") {
            CHTLCliTest::runTests();
        } else {
            std::cout << "Unknown test: " << testName << std::endl;
            std::cout << "Available tests: lexer, parser, generator, cli" << std::endl;
        }
    }
};

} // namespace CHTL

int main(int argc, char* argv[]) {
    if (argc > 1) {
        CHTL::TestRunner::runSpecificTest(argv[1]);
    } else {
        CHTL::TestRunner::runAllTests();
    }
    
    return 0;
}