# Define the executable for the end-to-end test
add_executable(E2ETest E2ETest.cpp)

# Link the test against the CHTL components
target_link_libraries(E2ETest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

# Add the test to be run by ctest
add_test(NAME CHTL_EndToEndTest COMMAND E2ETest)
