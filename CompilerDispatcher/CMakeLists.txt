add_executable(chtl_compiler main.cpp)

# Link against the actual CHTL component libraries
target_link_libraries(chtl_compiler PRIVATE
    CHTLLexer
    CHTLParser
    CHTLGenerator
)

# Include the root directory to find the component headers
target_include_directories(chtl_compiler PRIVATE
    ..
)
