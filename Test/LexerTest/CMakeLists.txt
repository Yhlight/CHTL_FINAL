add_executable(LexerTest LexerTest.cpp)

target_link_libraries(LexerTest
    PRIVATE
    gtest_main
    chtl_lib
)

include(GoogleTest)
gtest_discover_tests(LexerTest)
