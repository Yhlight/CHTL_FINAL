add_library(ThirdParty STATIC
    # 第三方库源文件将在这里添加
)

target_include_directories(ThirdParty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)