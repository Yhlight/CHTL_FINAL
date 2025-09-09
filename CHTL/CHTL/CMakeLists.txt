# CHTL 核心库
add_library(CHTLCore STATIC
    CHTLCore.cpp
)

# 添加子目录
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)
add_subdirectory(CHTLManage)
add_subdirectory(CHTLJS)
add_subdirectory(CHTLGenerator)
add_subdirectory(CHTLModule)
add_subdirectory(CHTLScanner)

# 链接依赖
target_link_libraries(CHTLCore PUBLIC
    CHTLLexer
    CHTLNode
    CHTLParser
    CHTLManage
    CHTLJSCore
    CHTLGenerator
    CHTLModule
    CHTLScanner
)

# 设置编译选项
target_compile_features(CHTLCore PUBLIC cxx_std_17)