# CHTL Core Library
add_library(CHTLCore STATIC
    CHTLContext.cpp
    CHTLLexer.cpp
    CHTLNode.cpp
    CHTLParser.cpp
    CHTLGenerator.cpp
    CHTLTemplate.cpp
    CHTLCustom.cpp
    CHTLOrigin.cpp
    CHTLImport.cpp
    CHTLConstraints.cpp
    CHTLConfig.cpp
)

target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(CHTLCore PUBLIC cxx_std_17)

# 设置编译选项
if(MSVC)
    target_compile_options(CHTLCore PRIVATE /W4)
else()
    target_compile_options(CHTLCore PRIVATE -Wall -Wextra -Wpedantic)
endif()