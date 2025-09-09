# Compiler Dispatcher Library
add_library(CompilerDispatcherCore STATIC
    # 待实现文件
)

target_include_directories(CompilerDispatcherCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(CompilerDispatcherCore PUBLIC cxx_std_17)