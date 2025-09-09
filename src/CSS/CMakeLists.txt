add_library(CSS STATIC
    # CSS 编译器源文件将在这里添加
)

target_include_directories(CSS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)