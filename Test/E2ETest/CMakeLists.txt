# Define the test executables
add_executable(E2ETest E2ETest.cpp)
add_executable(AdvancedStylesTest AdvancedStylesTest.cpp)
add_executable(TemplateSystemTest TemplateSystemTest.cpp)

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

target_link_libraries(TemplateSystemTest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

# Add the tests to be run by ctest
add_test(NAME CHTL_EndToEndTest COMMAND E2ETest)
add_test(NAME CHTL_AdvancedStylesTest COMMAND AdvancedStylesTest)
add_test(NAME CHTL_TemplateSystemTest COMMAND TemplateSystemTest)
