# This is the main pipeline test, despite its name
add_executable(pipeline_test AstTest/main.cpp AstTest/ASTPrinter.cpp)
target_link_libraries(pipeline_test PRIVATE CHTL Util)

# This will be repurposed for the expression test
add_executable(expression_test FullPipelineTest/main.cpp)
target_link_libraries(expression_test PRIVATE CHTL Util)

# This is the original token test
add_executable(token_test TokenTest/main.cpp)
target_link_libraries(token_test PRIVATE CHTL Util)
