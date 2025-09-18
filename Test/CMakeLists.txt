# Enable testing for the project
enable_testing()

# AST Printer Test
add_executable(ast_test
    AstTest/main.cpp
    AstTest/ASTPrinter.cpp
    AstTest/ASTPrinter.h
)
target_link_libraries(ast_test PRIVATE CHTL)
add_test(NAME ast_test COMMAND ast_test)

# Token Test
add_executable(token_test
    TokenTest/main.cpp
)
target_link_libraries(token_test PRIVATE CHTL)
add_test(NAME token_test COMMAND token_test)

# Full Pipeline Test
add_executable(full_pipeline_test
    FullPipelineTest/main.cpp
)
target_link_libraries(full_pipeline_test PRIVATE CHTL)
add_test(NAME full_pipeline_test COMMAND full_pipeline_test)

# Parser Test
add_executable(parser_test
    ParserTest/main.cpp
)
target_link_libraries(parser_test PRIVATE CHTL)
add_test(NAME parser_test COMMAND parser_test)
