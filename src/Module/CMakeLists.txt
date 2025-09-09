add_library(Module STATIC
    # 模块系统源文件将在这里添加
)

target_include_directories(Module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)