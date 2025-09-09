# CSS Core Library
add_library(CSSCore STATIC
    # 待实现文件
)

target_include_directories(CSSCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(CSSCore PUBLIC cxx_std_17)