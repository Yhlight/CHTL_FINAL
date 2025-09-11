# Create a static library for the CHTL components
add_library(chtl_lib STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLParser/CHTLParser.cpp
    CHTLGenerator/CHTLGenerator.cpp
    CHTLAnalyzer/CHTLSemanticAnalyzer.cpp
    CHTLResolver/CHTLTemplateResolver.cpp
    CHTLLoader/CHTLLoader.cpp
    Scanner/CHTLUnifiedScanner.cpp
    CompilerDispatcher/CompilerDispatcher.cpp
    "../CHTL JS/CHTLJSLexer/CHTLJSLexer.cpp"
    "../CHTL JS/CHTLJSParser/CHTLJSParser.cpp"
    "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.cpp"
)

# Add include directories for the library
target_include_directories(chtl_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLObject
    ${CMAKE_SOURCE_DIR}
)

# Define the executable for the CHTL compiler
add_executable(chtl_compiler main.cpp)

# Link the compiler against the library
target_link_libraries(chtl_compiler PRIVATE chtl_lib)
