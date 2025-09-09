# CHTL JS 解析器库
add_library(CHTLJSParser STATIC
    CHTLJSParser.cpp
)

target_include_directories(CHTLJSParser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CHTLJSParser PUBLIC
    CHTLJSLexer
    CHTLJSNode
)

# 设置编译选项
target_compile_features(CHTLJSParser PUBLIC cxx_std_17)