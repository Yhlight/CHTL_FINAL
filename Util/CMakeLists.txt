add_library(chtl_util STATIC
    ASTCloner.cpp
)

target_include_directories(chtl_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# The cloner needs access to the AST nodes
target_link_libraries(chtl_util PRIVATE chtl_ast)
