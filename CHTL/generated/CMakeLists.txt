# ANTLR4 Generated Files
cmake_minimum_required(VERSION 3.10)

# CHTL Parser
add_library(chtl_parser
    chtl/CHTLLexer.cpp
    chtl/CHTLParser.cpp
    chtl/CHTLBaseListener.cpp
    chtl/CHTLBaseVisitor.cpp
    chtl/CHTLListener.cpp
    chtl/CHTLVisitor.cpp
)

# CHTL JS Parser
add_library(chtljs_parser
    chtljs/CHTLJSLexer.cpp
    chtljs/CHTLJSParser.cpp
    chtljs/CHTLJSBaseListener.cpp
    chtljs/CHTLJSBaseVisitor.cpp
    chtljs/CHTLJSListener.cpp
    chtljs/CHTLJSVisitor.cpp
)

# Include directories
target_include_directories(chtl_parser PUBLIC chtl)
target_include_directories(chtljs_parser PUBLIC chtljs)

# Link ANTLR4 runtime
find_package(antlr4-runtime REQUIRED)
target_link_libraries(chtl_parser antlr4-runtime)
target_link_libraries(chtljs_parser antlr4-runtime)
