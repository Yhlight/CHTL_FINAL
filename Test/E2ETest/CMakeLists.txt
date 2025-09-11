# Define the test executables
add_executable(NamespaceSystemTest ModuleSystemTest.cpp)

# Link the tests against the CHTL components and gtest
target_link_libraries(NamespaceSystemTest
    PRIVATE
    gtest_main
    chtl_lib
)

# Discover the tests in each executable
include(GoogleTest)
gtest_discover_tests(NamespaceSystemTest)
