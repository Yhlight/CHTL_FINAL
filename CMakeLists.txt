cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(chtl_compiler main.cpp)

# Define subdirectory order. Dependencies must be added before their dependents.
add_subdirectory(Scanner)
add_subdirectory(Loader)
add_subdirectory(CHTL)
add_subdirectory(CHTL_JS)
add_subdirectory(CompilerDispatcher)
add_subdirectory(CodeMerger)
add_subdirectory(Test)

# Link libraries to the main executable
target_link_libraries(chtl_compiler PRIVATE
    merger
    dispatcher
    # The rest are dependencies of dispatcher and merger,
    # and will be linked transitively because of PUBLIC linking.
)
