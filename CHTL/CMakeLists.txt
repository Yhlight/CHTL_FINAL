# CHTL/CMakeLists.txt

# Create a static library for the CHTL compiler components
add_library(CHTL_LIB STATIC
    CHTLLexer/Lexer.cpp
    CHTLParser/Parser.cpp
)

# Publicly expose the include directories for the library
target_include_directories(CHTL_LIB PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# We need to include the Util directory for StringUtil.h
target_include_directories(CHTL_LIB PUBLIC
    ${CMAKE_SOURCE_DIR}/Util
)
