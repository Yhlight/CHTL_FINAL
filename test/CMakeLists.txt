cmake_minimum_required(VERSION 3.16)

# 查找测试框架
find_package(GTest QUIET)

if(GTest_FOUND)
    # 使用Google Test
    add_executable(chtl_test
        test_main.cpp
        test_lexer.cpp
        test_parser.cpp
        test_generator.cpp
        test_scanner.cpp
    )
    
    target_link_libraries(chtl_test
        GTest::GTest
        GTest::Main
    )
    
    enable_testing()
    add_test(NAME CHTLTests COMMAND chtl_test)
else()
    # 使用简单的测试框架
    add_executable(chtl_test
        test_main.cpp
        test_lexer.cpp
        test_parser.cpp
        test_generator.cpp
        test_scanner.cpp
    )
    
    # 不需要链接chtl，因为测试是独立的
    
    enable_testing()
    add_test(NAME CHTLTests COMMAND chtl_test)
endif()