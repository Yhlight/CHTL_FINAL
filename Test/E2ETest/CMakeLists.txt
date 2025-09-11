# Define the test executables
add_executable(ImportSystemTest ImportSystemTest.cpp)
add_executable(ModuleSystemTest ModuleSystemTest.cpp)

# Link the tests against the CHTL components and gtest
target_link_libraries(ImportSystemTest
    PRIVATE
    gtest_main
    chtl_lib
)

target_link_libraries(ModuleSystemTest
    PRIVATE
    gtest_main
    chtl_lib
)

# Discover the tests in each executable
include(GoogleTest)
gtest_discover_tests(ImportSystemTest)
gtest_discover_tests(ModuleSystemTest)
