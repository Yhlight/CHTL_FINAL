# CHTL JS Core Library
add_library(CHTLJSCore STATIC
    # 待实现文件
)

target_include_directories(CHTLJSCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(CHTLJSCore PUBLIC cxx_std_17)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTLJSCore PRIVATE /W4)
else()
    target_compile_options(CHTLJSCore PRIVATE -Wall -Wextra -Wpedantic)
endif()