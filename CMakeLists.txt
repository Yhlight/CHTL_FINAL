cmake_minimum_required(VERSION 3.10)
project(CHTL_Project)

# Set C++ standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static linking to work around sandbox environment issues
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

# Enable testing with CTest
enable_testing()

# Add subdirectories for the main components
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(Scanner)
add_subdirectory(CodeMerger)
add_subdirectory(CompilerDispatcher)
# add_subdirectory("CHTL JS")

# Add subdirectory for tests
add_subdirectory(Test)

# Add the main executable
add_executable(chtl_compiler main.cpp)

# Link libraries
target_link_libraries(chtl_compiler PRIVATE CHTLCompiler Util)
