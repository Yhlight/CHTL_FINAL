# CHTL 解析器库
add_library(CHTLParser STATIC
    Parser.cpp
)

target_include_directories(CHTLParser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLParser PUBLIC
    CHTLLexer
    CHTLNode
)

# 设置编译选项
target_compile_features(CHTLParser PUBLIC cxx_std_17)