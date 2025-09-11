add_library(context STATIC
    ConfigurationContext.cpp
    ConfigurationVisitor.cpp
)

target_include_directories(context PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Context needs the AST nodes
target_link_libraries(context PRIVATE chtl_ast)
