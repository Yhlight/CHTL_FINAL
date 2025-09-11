# --- AST Library ---
# Interface library, as it's header-only
add_library(chtl_ast INTERFACE)
target_include_directories(chtl_ast INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Context Library ---
add_library(context STATIC
    Context/ConfigurationContext.cpp
    Context/ConfigurationVisitor.cpp
)
target_include_directories(context PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(context PUBLIC chtl_ast) # Context needs AST nodes

# --- Util Library (for AST Cloner, etc.) ---
add_library(chtl_util STATIC
    CHTLUtil/ASTCloner.cpp
)
target_include_directories(chtl_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chtl_util PUBLIC chtl_ast) # Util needs AST nodes

# --- Lexer Library ---
add_library(chtl_lexer STATIC
    CHTLLexer/Lexer.cpp
)
target_include_directories(chtl_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chtl_lexer PUBLIC context) # Lexer needs config context

# --- Parser Library ---
add_library(chtl_parser STATIC
    CHTLParser/Parser.cpp
    CHTLParser/ASTPrinter.cpp
)
target_include_directories(chtl_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chtl_parser PUBLIC chtl_lexer chtl_ast)

# --- Generator Library ---
add_library(chtl_generator STATIC
    CHTLGenerator/Generator.cpp
    CHTLGenerator/ExprGenerator.cpp
)
target_include_directories(chtl_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(chtl_generator PUBLIC chtl_ast chtl_util)
