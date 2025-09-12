# CHTL核心库
add_library(CHTLCore STATIC
    # 核心组件
    CHTLContext.cpp
    Token.cpp
    CHTLLexer.cpp
    CHTLParser.cpp
    CHTLGenerator.cpp
    
    # 节点系统
    CHTLNode/BaseNode.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/TemplateNode.cpp
    CHTLNode/CustomNode.cpp
    CHTLNode/ImportNode.cpp
    CHTLNode/NamespaceNode.cpp
    CHTLNode/ConfigurationNode.cpp
    CHTLNode/ModuleNode.cpp
    
    # 导入系统
    ImportManager.cpp
    
    # 命名空间系统
    NamespaceManager.cpp
    
    # 配置系统
    ConfigurationManager.cpp
    
    # 模块系统
    ModuleManager.cpp
    
    # CJMOD系统
    CJMOD/CJMODAPI.cpp
    CJMOD/CJMODCompiler.cpp
    
    # 代码合并器
    CodeMerger/CodeMerger.cpp
    
    # 编译器调度器
    CompilerDispatcher/CompilerDispatcher.cpp
    
    # 扫描器
    Scanner/UnifiedScanner.cpp
)

# 设置包含目录
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 查找并链接依赖库
find_package(PkgConfig REQUIRED)
pkg_check_modules(ZLIB REQUIRED zlib)

target_link_libraries(CHTLCore
    ${ZLIB_LIBRARIES}
)

# 编译选项
target_compile_features(CHTLCore PUBLIC cxx_std_17)