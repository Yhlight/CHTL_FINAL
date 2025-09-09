# Scanner Core Library
add_library(ScannerCore STATIC
    # 待实现文件
)

target_include_directories(ScannerCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(ScannerCore PUBLIC cxx_std_17)