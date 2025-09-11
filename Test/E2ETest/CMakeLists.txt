# Define the executable for the end-to-end test
add_executable(E2ETest E2ETest.cpp)
add_executable(AdvancedStylesTest AdvancedStylesTest.cpp)
add_executable(TemplateSystemTest TemplateSystemTest.cpp)
add_executable(CustomSystemTest CustomSystemTest.cpp)
add_executable(CustomElementTest CustomElementTest.cpp)

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

target_link_libraries(CustomSystemTest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

target_link_libraries(CustomElementTest
    PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

# Add the tests to be run by ctest
add_test(NAME CHTL_EndToEndTest COMMAND E2ETest)
add_test(NAME CHTL_AdvancedStylesTest COMMAND AdvancedStylesTest)
add_test(NAME CHTL_TemplateSystemTest COMMAND TemplateSystemTest)
add_test(NAME CHTL_CustomSystemTest COMMAND CustomSystemTest)
add_test(NAME CHTL_CustomElementTest COMMAND CustomElementTest)
