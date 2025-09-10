# --- Lexer Library ---
add_library(chtl_lexer STATIC
    CHTLLexer/Lexer.cpp
)
target_include_directories(chtl_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# --- AST Library ---
# For now, the AST is header-only, so we create an INTERFACE library
# to manage include paths.
add_library(chtl_ast INTERFACE)
target_include_directories(chtl_ast INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


# --- Parser Library ---
add_library(chtl_parser STATIC
    CHTLParser/Parser.cpp
    CHTLParser/ASTPrinter.cpp
)
target_link_libraries(chtl_parser PRIVATE chtl_lexer chtl_ast)
target_include_directories(chtl_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# --- Generator Library ---
add_library(chtl_generator STATIC
    CHTLGenerator/Generator.cpp
    CHTLGenerator/ExprGenerator.cpp
)
target_link_libraries(chtl_generator PRIVATE chtl_ast chtl_util)
target_include_directories(chtl_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
