cmake_minimum_required(VERSION 3.10)
project(CHTLResponsiveValueTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 源文件
set(SOURCES
    test_responsive_values.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSLexer.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSToken.cpp
    CHTL_JS/CHTLJSLexer/CHTLJSGlobalMap.cpp
    CHTL_JS/CHTLJSParser/CHTLJSParser.cpp
    CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    CHTL_JS/CHTLJSNode/CHTLJSBaseNode.cpp
    CHTL/CHTLCustom/CHTLCustomProcessor.cpp
    CHTL/CHTLTemplate/CHTLTemplateProcessor.cpp
    CHTL/CHTLOrigin/CHTLOriginProcessor.cpp
    CHTL/CHTLImport/CHTLImportProcessor.cpp
    CHTL/CHTLNamespace/CHTLNamespaceProcessor.cpp
    CHTL/CHTLConstraint/CHTLConstraintProcessor.cpp
    CHTL/CHTLConfiguration/CHTLConfigurationProcessor.cpp
    CHTL/CHTLUse/CHTLUseProcessor.cpp
    CHTL/CHTLJS/CHTLJSAdvancedProcessor.cpp
    Module/CMODModule.cpp
    Module/CMODModuleManager.cpp
    Module/CJMODModule.cpp
    Module/OfficialModules/ChthollyModule.cpp
    Module/OfficialModules/YuigahamaModule.cpp
    Scanner/CHTLUnifiedScanner.cpp
    Test/TestFramework.cpp
    CHTL/CHTLNode/CustomNode.cpp
    CHTL/CHTLNode/TemplateNode.cpp
    CHTL/CHTLNode/BaseNode.cpp
    CHTL/CHTLNode/ElementNode.cpp
    CHTL/CHTLNode/TextNode.cpp
    CHTL/CHTLNode/CommentNode.cpp
    CHTL/CHTLNode/StyleNode.cpp
    CHTL/CHTLNode/ScriptNode.cpp
    CHTL/CHTLNode/OriginNode.cpp
    CHTL/CHTLNode/ImportNode.cpp
    CHTL/CHTLNode/NamespaceNode.cpp
    CHTL/CHTLNode/ConstraintNode.cpp
    CHTL/CHTLNode/ConfigurationNode.cpp
    CHTL/CHTLNode/UseNode.cpp
)

# 创建可执行文件
add_executable(test_responsive_values ${SOURCES})

# 链接库
target_link_libraries(test_responsive_values)