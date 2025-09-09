# CHTL Core Module
add_library(CHTLCore STATIC
    # Context
    CHTLContext/CHTLContext.h
    CHTLContext/CHTLContext.cpp
    
    # Generator
    CHTLGenerator/CHTLGenerator.h
    CHTLGenerator/CHTLGenerator.cpp
    
    # Lexer
    CHTLLexer/CHTLLexer.h
    CHTLLexer/CHTLLexer.cpp
    CHTLLexer/Lexer.h
    CHTLLexer/Lexer.cpp
    CHTLLexer/GlobalMap.h
    CHTLLexer/GlobalMap.cpp
    CHTLLexer/Token.h
    CHTLLexer/Token.cpp
    
    # Loader
    CHTLLoader/CHTLLoader.h
    CHTLLoader/CHTLLoader.cpp
    
    # Manager
    CHTLManage/CHTLManage.h
    CHTLManage/CHTLManage.cpp
    CHTLManage/ImportManager.h
    CHTLManage/ImportManager.cpp
    CHTLManage/NamespaceManager.h
    CHTLManage/NamespaceManager.cpp
    CHTLManage/ConstraintManager.h
    CHTLManage/ConstraintManager.cpp
    CHTLManage/ConfigurationManager.h
    CHTLManage/ConfigurationManager.cpp
    
    # Nodes
    CHTLNode/BaseNode.h
    CHTLNode/BaseNode.cpp
    CHTLNode/ElementNode.h
    CHTLNode/ElementNode.cpp
    CHTLNode/TextNode.h
    CHTLNode/TextNode.cpp
    CHTLNode/TemplateNode.h
    CHTLNode/TemplateNode.cpp
    CHTLNode/CustomNode.h
    CHTLNode/CustomNode.cpp
    CHTLNode/StyleNode.h
    CHTLNode/StyleNode.cpp
    CHTLNode/ScriptNode.h
    CHTLNode/ScriptNode.cpp
    CHTLNode/ImportNode.h
    CHTLNode/ImportNode.cpp
    CHTLNode/NamespaceNode.h
    CHTLNode/NamespaceNode.cpp
    CHTLNode/ConstraintNode.h
    CHTLNode/ConstraintNode.cpp
    CHTLNode/ConfigurationNode.h
    CHTLNode/ConfigurationNode.cpp
    
    # Parser
    CHTLParser/CHTLParser.h
    CHTLParser/CHTLParser.cpp
    
    # State
    CHTLState/CHTLState.h
    CHTLState/CHTLState.cpp
    
    # IO Stream
    CHTLIOStream/CHTLIOStream.h
    CHTLIOStream/CHTLIOStream.cpp
    
    # Module System
    ModuleSystem/CMODSystem.h
    ModuleSystem/CMODSystem.cpp
    ModuleSystem/CJMODSystem.h
    ModuleSystem/CJMODSystem.cpp
    ModuleSystem/HybridModuleSystem.h
    ModuleSystem/HybridModuleSystem.cpp
    
    # CJMOD API
    CJMODAPI/Syntax.h
    CJMODAPI/Syntax.cpp
    CJMODAPI/Arg.h
    CJMODAPI/Arg.cpp
    CJMODAPI/AtomArg.h
    CJMODAPI/CHTLJSFunction.h
)

target_include_directories(CHTLCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})