# Define an executable for the parser test
add_executable(parser_test parser_test.cpp)

# Link the test executable against the main chtl_lib
# We need to link against the library defined in the parent scope
target_link_libraries(parser_test PRIVATE chtl_lib)
