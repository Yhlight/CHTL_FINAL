# CHTL核心库
add_library(CHTLCore STATIC
    CHTLContext.cpp
    CHTLLexer.cpp
    CHTLParser.cpp
    CHTLGenerator.cpp
    CHTLLoader.cpp
    CHTLNode.cpp
)

# 设置包含目录
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# 设置C++标准
target_compile_features(CHTLCore PUBLIC cxx_std_17)

# 如果启用了测试，添加测试目标
if(BUILD_TESTING)
    target_compile_definitions(CHTLCore PRIVATE CHTL_TESTING)
endif()