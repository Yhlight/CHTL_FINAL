# Module Library
add_library(ModuleCore STATIC
    # 待实现文件
)

target_include_directories(ModuleCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(ModuleCore PUBLIC cxx_std_17)