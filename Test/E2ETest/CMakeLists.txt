# Define the executable for the end-to-end test
add_executable(E2ETest E2ETest.cpp)
add_executable(AdvancedStylesTest AdvancedStylesTest.cpp)

# Link the tests against the CHTL components
target_link_libraries(E2ETest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

target_link_libraries(AdvancedStylesTest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

# Add the tests to be run by ctest
add_test(NAME CHTL_EndToEndTest COMMAND E2ETest)
add_test(NAME CHTL_AdvancedStylesTest COMMAND AdvancedStylesTest)
