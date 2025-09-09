# Utility Library
add_library(UtilCore STATIC
    # 待实现文件
)

target_include_directories(UtilCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(UtilCore PUBLIC cxx_std_17)