cmake_minimum_required(VERSION 3.16)

# 测试可执行文件
add_executable(test_lexer test_lexer.cpp)
target_link_libraries(test_lexer CHTLCore)

# 如果启用了测试
if(BUILD_TESTING)
    enable_testing()
    add_test(NAME LexerTest COMMAND test_lexer)
endif()