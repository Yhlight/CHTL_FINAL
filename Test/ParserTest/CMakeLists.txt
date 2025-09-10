add_executable(ParserTest ParserTest.cpp)

target_link_libraries(ParserTest
    PRIVATE
    gtest_main
    chtl_lib
)

include(GoogleTest)
gtest_discover_tests(ParserTest)
