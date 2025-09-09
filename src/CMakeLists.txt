# CHTL Core Library
add_library(CHTLCore STATIC
    # CHTL Core
    CHTL/CHTLContext.cpp
    CHTL/CHTLGenerator.cpp
    CHTL/CHTLLexer.cpp
    CHTL/CHTLLoader.cpp
    CHTL/CHTLManage.cpp
    CHTL/CHTLParser.cpp
    CHTL/CHTLState.cpp
    CHTL/CHTLIOStream.cpp
    
    # CHTL Nodes
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
    
    # CHTL JS
    CHTLJS/CHTLJSContext.cpp
    CHTLJS/CHTLJSGenerator.cpp
    CHTLJS/CHTLJSLexer.cpp
    CHTLJS/CHTLJSLoader.cpp
    CHTLJS/CHTLJSManage.cpp
    CHTLJS/CHTLJSParser.cpp
    CHTLJS/CHTLJSState.cpp
    CHTLJS/CHTLJSIOStream.cpp
    
    # CHTL JS Nodes
    CHTLJS/CHTLJSNode/CHTLJSBaseNode.cpp
    CHTLJS/CHTLJSNode/CHTLJSFunctionNode.cpp
    CHTLJS/CHTLJSNode/CHTLJSVirtualNode.cpp
    CHTLJS/CHTLJSNode/CHTLJSSelectorNode.cpp
    CHTLJS/CHTLJSNode/CHTLJSAnimationNode.cpp
    CHTLJS/CHTLJSNode/CHTLJSRouterNode.cpp
    
    # Scanner
    Scanner/CHTLUnifiedScanner.cpp
    Scanner/CodeMerger.cpp
    Scanner/CompilerDispatcher.cpp
    
    # Modules
    CMODSystem/CMODLoader.cpp
    CMODSystem/CMODParser.cpp
    CMODSystem/CMODGenerator.cpp
    CJMODSystem/CJMODLoader.cpp
    CJMODSystem/CJMODParser.cpp
    CJMODSystem/CJMODGenerator.cpp
    CJMODSystem/CJMODAPI.cpp
    
    # CLI
    CLI/CHTLCli.cpp
    
    # Test Framework
    Test/CHTLTest.cpp
    
    # Utilities
    Util/FileSystem/FileSystem.cpp
    Util/ZipUtil/ZipUtil.cpp
    Util/StringUtil/StringUtil.cpp
    
    # Third Party
    # ThirdParty/antlr4-runtime.cpp
)

# 设置包含目录
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接库
target_link_libraries(CHTLCore
    # 添加必要的系统库
)

# 编译选项
target_compile_definitions(CHTLCore PRIVATE
    CHTL_VERSION_MAJOR=1
    CHTL_VERSION_MINOR=0
    CHTL_VERSION_PATCH=0
)

# 测试可执行文件
add_executable(chtl-test
    Test/TestRunner.cpp
    Test/CHTLLexerTest.cpp
    Test/CHTLParserTest.cpp
    Test/CHTLGeneratorTest.cpp
    Test/CHTLCliTest.cpp
)

target_link_libraries(chtl-test CHTLCore)