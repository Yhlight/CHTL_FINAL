# Third Party Libraries
add_library(ThirdPartyCore STATIC
    # 待实现文件
)

target_include_directories(ThirdPartyCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(ThirdPartyCore PUBLIC cxx_std_17)