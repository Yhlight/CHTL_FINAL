# Test Library
add_library(TestCore STATIC
    # 待实现文件
)

target_include_directories(TestCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(TestCore PUBLIC cxx_std_17)