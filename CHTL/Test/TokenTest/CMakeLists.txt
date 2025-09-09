# Token 测试
add_executable(TokenTest
    TokenTest.cpp
    LexerTest.cpp
)

target_link_libraries(TokenTest
    CHTLLexer
    gtest
    gtest_main
)

# 添加测试
add_test(NAME TokenTest COMMAND TokenTest)