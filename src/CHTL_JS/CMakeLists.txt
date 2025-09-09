# CHTL JS 核心模块
add_library(CHTLJSCore STATIC
    CHTLJSNode.cpp
    CHTLJSLexer.cpp
    CHTLJSParser.cpp
    CHTLJSGenerator.cpp
    CHTLJSContext.cpp
    CHTLJSVisitor.cpp
)

target_include_directories(CHTLJSCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CHTLJSCore
    ${Boost_LIBRARIES}
)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTLJSCore PRIVATE /W4)
else()
    target_compile_options(CHTLJSCore PRIVATE -Wall -Wextra -Wpedantic)
endif()