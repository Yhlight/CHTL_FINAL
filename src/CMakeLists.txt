# CHTL Core Library
add_library(CHTLCore STATIC
    # Base classes
    CHTL/BaseNode/BaseNode.cpp
    CHTL/ElementNode/ElementNode.cpp
    CHTL/TextNode/TextNode.cpp
    CHTL/StyleNode/StyleNode.cpp
    CHTL/ScriptNode/ScriptNode.cpp
    CHTL/TemplateNode/TemplateNode.cpp
    CHTL/CustomNode/CustomNode.cpp
    CHTL/ImportNode/ImportNode.cpp
    CHTL/NamespaceNode/NamespaceNode.cpp
    CHTL/ConfigurationNode/ConfigurationNode.cpp
    CHTL/OriginNode/OriginNode.cpp
    CHTL/OperatorNode/OperatorNode.cpp
    
    # Lexer
    CHTL/CHTLLexer/CHTLLexer.cpp
    
    # Parser (to be implemented)
    # CHTL/CHTLParser/CHTLParser.cpp
    
    # Generator (to be implemented)
    # CHTL/CHTLGenerator/CHTLGenerator.cpp
    
    # Context (to be implemented)
    # CHTL/CHTLContext/CHTLContext.cpp
    
    # State (to be implemented)
    # CHTL/CHTLState/CHTLState.cpp
    
    # Utilities (to be implemented)
    # Util/FileSystem/FileSystem.cpp
    # Util/StringUtil/StringUtil.cpp
    # Util/ZipUtil/ZipUtil.cpp
    
    # Scanner (to be implemented)
    # Scanner/UnifiedScanner.cpp
    
    # Code Merger (to be implemented)
    # CodeMerger/CodeMerger.cpp
    
    # Compiler Dispatcher (to be implemented)
    # CompilerDispatcher/CompilerDispatcher.cpp
)

# Set include directories
target_include_directories(CHTLCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++ standard
target_compile_features(CHTLCore PUBLIC cxx_std_17)

# Compiler-specific options
if(MSVC)
    target_compile_options(CHTLCore PRIVATE /W4)
else()
    target_compile_options(CHTLCore PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link libraries
target_link_libraries(CHTLCore PUBLIC)

# Install rules
install(TARGETS CHTLCore
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/CHTL/
    DESTINATION include/CHTL
    FILES_MATCHING PATTERN "*.h"
)