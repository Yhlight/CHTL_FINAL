cmake_minimum_required(VERSION 3.16)

# 创建测试可执行文件
add_executable(chtl_test
    test_main.cpp
    UnifiedScannerTest.cpp
    CHTLLexerTest.cpp
    CHTLParserTest.cpp
)

# 链接库
target_link_libraries(chtl_test chtl_lib)

# 添加测试
add_test(NAME UnifiedScannerTest COMMAND chtl_test --gtest_filter=UnifiedScannerTest.*)
add_test(NAME CHTLLexerTest COMMAND chtl_test --gtest_filter=CHTLLexerTest.*)
add_test(NAME CHTLParserTest COMMAND chtl_test --gtest_filter=CHTLParserTest.*)