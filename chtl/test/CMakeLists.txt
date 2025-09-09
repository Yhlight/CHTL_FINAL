# Test executable
add_executable(chtl_test
    test_main.cpp
)

# Link with the main library
target_link_libraries(chtl_test chtl_lib)

# Add test
add_test(NAME chtl_tests COMMAND chtl_test)