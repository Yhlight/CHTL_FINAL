# CHTL 核心模块
add_library(CHTLCore STATIC
    CHTLNode.cpp
    CHTLLexer.cpp
    CHTLParser.cpp
    CHTLGenerator.cpp
    CHTLContext.cpp
    CHTLVisitor.cpp
)

target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(CHTLCore
    ${Boost_LIBRARIES}
)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTLCore PRIVATE /W4)
else()
    target_compile_options(CHTLCore PRIVATE -Wall -Wextra -Wpedantic)
endif()