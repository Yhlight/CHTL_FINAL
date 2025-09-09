add_library(JS STATIC
    # JavaScript 编译器源文件将在这里添加
)

target_include_directories(JS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)