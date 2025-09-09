# Code Merger Library
add_library(CodeMergerCore STATIC
    # 待实现文件
)

target_include_directories(CodeMergerCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(CodeMergerCore PUBLIC cxx_std_17)