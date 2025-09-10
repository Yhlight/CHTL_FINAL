cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/CHTL)
include_directories(${CMAKE_SOURCE_DIR}/CHTL_JS)
include_directories(${CMAKE_SOURCE_DIR}/Util)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

# 源文件
set(CHTL_SOURCES
    CHTL/CHTLContext/CHTLContext.cpp
    CHTL/CHTLContext/CHTLContext.h
    CHTL/CHTLGenerator/CHTLGenerator.cpp
    CHTL/CHTLGenerator/CHTLGenerator.h
    CHTL/CHTLStyle/CHTLStyleProcessor.cpp
    CHTL/CHTLStyle/CHTLStyleProcessor.h
    CHTL/CHTLProperty/CHTLPropertyReference.cpp
    CHTL/CHTLProperty/CHTLPropertyReference.h
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLLexer/CHTLLexer.h
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLLexer/Lexer.h
    CHTL/CHTLLexer/GlobalMap.cpp
    CHTL/CHTLLexer/GlobalMap.h
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLLexer/Token.h
    CHTL/CHTLLoader/CHTLLoader.cpp
    CHTL/CHTLLoader/CHTLLoader.h
    CHTL/CHTLManage/CHTLManage.cpp
    CHTL/CHTLManage/CHTLManage.h
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/BaseNode.h
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/ElementNode.h
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/TextNode.h
    CHTL/CHTLNode/CommentNode.cpp
    CHTL/CHTLNode/CommentNode.h
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/StyleNode.h
    CHTL/CHTLNode/ScriptNode.cpp
    CHTL/CHTLNode/ScriptNode.h
    CHTL/CHTLNode/TemplateNode.cpp
    CHTL/CHTLNode/TemplateNode.h
    CHTL/CHTLNode/CustomNode.cpp
    CHTL/CHTLNode/CustomNode.h
    CHTL/CHTLNode/OriginNode.cpp
    CHTL/CHTLNode/OriginNode.h
    CHTL/CHTLNode/ImportNode.cpp
    CHTL/CHTLNode/ImportNode.h
    CHTL/CHTLNode/NamespaceNode.cpp
    CHTL/CHTLNode/NamespaceNode.h
    CHTL/CHTLNode/ConstraintNode.cpp
    CHTL/CHTLNode/ConstraintNode.h
    CHTL/CHTLNode/ConfigurationNode.cpp
    CHTL/CHTLNode/ConfigurationNode.h
    CHTL/CHTLNode/UseNode.cpp
    CHTL/CHTLNode/UseNode.h
    CHTL/CHTLConstraint/CHTLConstraintProcessor.cpp
    CHTL/CHTLConstraint/CHTLConstraintProcessor.h
    CHTL/CHTLConfiguration/CHTLConfigurationProcessor.cpp
    CHTL/CHTLConfiguration/CHTLConfigurationProcessor.h
    CHTL/CHTLUse/CHTLUseProcessor.cpp
    CHTL/CHTLUse/CHTLUseProcessor.h
    CHTL/CHTLParser/CHTLParser.cpp
    CHTL/CHTLParser/CHTLParser.h
    CHTL/CHTLState/CHTLState.cpp
    CHTL/CHTLState/CHTLState.h
    CHTL/CHTLIOStream/CHTLIOStream.cpp
    CHTL/CHTLIOStream/CHTLIOStream.h
    CHTL/CMODSystem/CMODSystem.cpp
    CHTL/CMODSystem/CMODSystem.h
    CHTL/CMODSystem/CMODModule.cpp
    CHTL/CMODSystem/CMODModule.h
    CHTL/CMODSystem/CMODLoader.cpp
    CHTL/CMODSystem/CMODLoader.h
)

set(CHTL_JS_SOURCES
    CHTL_JS/CHTLJSContext/CHTLJSContext.cpp
    CHTL_JS/CHTLJSContext/CHTLJSContext.h
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h
    CHTL_JS/CHTLJSLexer/CHTLJSLexer.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSLexer.h
    CHTL_JS/CHTLJSLexer/CHTLJSToken.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSToken.h
    CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.h
    CHTL_JS/CHTLJSLexer/Lexer.cpp
    CHTL_JS/CHTLJSLexer/Lexer.h
    CHTL_JS/CHTLJSLexer/GlobalMap.cpp
    CHTL_JS/CHTLJSLexer/GlobalMap.h
    CHTL_JS/CHTLJSLexer/Token.cpp
    CHTL_JS/CHTLJSLexer/Token.h
    CHTL_JS/CHTLJSLoader/CHTLJSLoader.cpp
    CHTL_JS/CHTLJSLoader/CHTLJSLoader.h
    CHTL_JS/CHTLJSManage/CHTLJSManage.cpp
    CHTL_JS/CHTLJSManage/CHTLJSManage.h
    CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
    CHTL_JS/CHTLJSNode/CHTLJSBaseNode.h
    CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    CHTL_JS/CHTLJSParser/CHTLJSParser.h
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h
    CHTL_JS/CHTLJSState/CHTLJSState.cpp
    CHTL_JS/CHTLJSState/CHTLJSState.h
    CHTL_JS/CHTLJSIOStream/CHTLJSIOStream.cpp
    CHTL_JS/CHTLJSIOStream/CHTLJSIOStream.h
    CHTL_JS/CJMODSystem/CJMODSystem.cpp
    CHTL_JS/CJMODSystem/CJMODSystem.h
    CHTL_JS/CJMODSystem/CJMODModule.cpp
    CHTL_JS/CJMODSystem/CJMODModule.h
    CHTL_JS/CJMODSystem/CJMODSyntax.cpp
    CHTL_JS/CJMODSystem/CJMODSyntax.h
    CHTL_JS/CJMODSystem/CJMODArg.cpp
    CHTL_JS/CJMODSystem/CJMODArg.h
    CHTL_JS/CJMODSystem/CJMODScanner.cpp
    CHTL_JS/CJMODSystem/CJMODScanner.h
    CHTL_JS/CJMODSystem/CJMODGenerator.cpp
    CHTL_JS/CJMODSystem/CJMODGenerator.h
    CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedSelector.cpp
    CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedSelector.h
    CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedListener.cpp
    CHTL_JS/CHTLJSEnhanced/CHTLJSEnhancedListener.h
    CHTL_JS/CHTLJSEnhanced/CHTLJSEventDelegation.cpp
    CHTL_JS/CHTLJSEnhanced/CHTLJSEventDelegation.h
)

set(UTIL_SOURCES
    Util/FileSystem/FileSystem.cpp
    Util/FileSystem/FileSystem.h
    Util/ZipUtil/ZipUtil.cpp
    Util/ZipUtil/ZipUtil.h
    Util/StringUtil/StringUtil.cpp
    Util/StringUtil/StringUtil.h
)

set(SCANNER_SOURCES
    Scanner/CHTLUnifiedScanner.cpp
    Scanner/CHTLUnifiedScanner.h
    Scanner/UnifiedScanner.cpp
    Scanner/UnifiedScanner.h
)

set(COMPILER_SOURCES
    CompilerDispatcher/CompilerDispatcher.cpp
    CompilerDispatcher/CompilerDispatcher.h
    CompilerDispatcher/CSSCompiler.cpp
    CompilerDispatcher/CSSCompiler.h
    CompilerDispatcher/JSCompiler.cpp
    CompilerDispatcher/JSCompiler.h
    CodeMerger/CodeMerger.cpp
    CodeMerger/CodeMerger.h
)

set(CLI_SOURCES
    CLI/CLITool.cpp
    CLI/CLITool.h
    CLI/main.cpp
)

# 创建可执行文件
add_executable(chtl_compiler
    main.cpp
    ${CHTL_SOURCES}
    ${CHTL_JS_SOURCES}
    ${UTIL_SOURCES}
    ${SCANNER_SOURCES}
    ${COMPILER_SOURCES}
    ${COMPILER_MONITOR_SOURCES}
    ${OFFICIAL_MODULE_SOURCES}
)

# 创建CLI工具
add_executable(chtl_cli
    ${CLI_SOURCES}
    ${CHTL_SOURCES}
    ${CHTL_JS_SOURCES}
    ${UTIL_SOURCES}
    ${SCANNER_SOURCES}
    ${COMPILER_SOURCES}
    ${COMPILER_MONITOR_SOURCES}
    ${OFFICIAL_MODULE_SOURCES}
)

# 链接库
target_link_libraries(chtl_compiler)
target_link_libraries(chtl_cli)

# 测试
enable_testing()

# 编译监视器
set(COMPILER_MONITOR_SOURCES
    CompilerMonitor/CompilerMonitor.cpp
)

# 官方模块
set(OFFICIAL_MODULE_SOURCES
    OfficialModules/ChthollyModule/ChthollyModule.cpp
    OfficialModules/YuigahamaModule/YuigahamaModule.cpp
    OfficialModules/OfficialModuleManager.cpp
)

# 测试系统
set(TEST_SOURCES
    Test/TestFramework.cpp
    Test/UtilTest.cpp
    Test/TokenTest.cpp
    Test/AstTest.cpp
    Test/test_main.cpp
)

# 创建测试可执行文件
add_executable(chtl_test_system
    ${TEST_SOURCES}
    ${CHTL_SOURCES}
    ${CHTL_JS_SOURCES}
    ${UTIL_SOURCES}
    ${SCANNER_SOURCES}
    ${COMPILER_SOURCES}
    ${COMPILER_MONITOR_SOURCES}
    ${OFFICIAL_MODULE_SOURCES}
)

# 链接库
target_link_libraries(chtl_test_system)

add_subdirectory(Test)