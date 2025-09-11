add_library(chtl_compiler_lib STATIC
    CompilationContext.cpp
    CHTLCompiler.cpp
)

target_include_directories(chtl_compiler_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(chtl_compiler_lib
    PRIVATE
    chtl_lib
)
