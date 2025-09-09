# CHTL 核心库
add_library(CHTL_CORE STATIC
    # CHTL 编译器核心
    CHTL/CHTLContext.cpp
    CHTL/CHTLGenerator.cpp
    CHTL/CHTLLoader.cpp
    CHTL/CHTLManage.cpp
    CHTL/CHTLParser.cpp
    CHTL/CHTLState.cpp
    CHTL/CHTLIOStream.cpp
    CHTL/CMODSystem.cpp
    
    # 词法分析器
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLLexer/Token.cpp
    
    # AST 节点
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/CommentNode.cpp
    CHTL/CHTLNode/TemplateNode.cpp
    CHTL/CHTLNode/CustomNode.cpp
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/ScriptNode.cpp
    CHTL/CHTLNode/OriginNode.cpp
    CHTL/CHTLNode/ImportNode.cpp
    CHTL/CHTLNode/ConfigNode.cpp
    CHTL/CHTLNode/NamespaceNode.cpp
    CHTL/CHTLNode/OperatorNode.cpp
    
    # CHTL JS 编译器
    CHTLJS/CHTLJSContext.cpp
    CHTLJS/CHTLJSGenerator.cpp
    CHTLJS/CHTLJSLoader.cpp
    CHTLJS/CHTLJSManage.cpp
    CHTLJS/CHTLJSParser.cpp
    CHTLJS/CHTLJSState.cpp
    CHTLJS/CHTLJSIOStream.cpp
    CHTLJS/CJMODSystem.cpp
    
    # 统一扫描器
    Scanner/UnifiedScanner.cpp
    
    # 代码合并器
    CodeMerger/CodeMerger.cpp
    
    # 编译器调度器
    CompilerDispatcher/CompilerDispatcher.cpp
    
    # 工具类
    Util/FileSystem/FileSystem.cpp
    Util/ZipUtil/ZipUtil.cpp
    Util/StringUtil/StringUtil.cpp
)

# 设置包含目录
target_include_directories(CHTL_CORE PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接线程库
target_link_libraries(CHTL_CORE Threads::Threads)