# JavaScript Core Library
add_library(JSCore STATIC
    # 待实现文件
)

target_include_directories(JSCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(JSCore PUBLIC cxx_std_17)