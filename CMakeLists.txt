cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Third-party libraries
find_package(ANTLR4 REQUIRED)

# Source files
set(CHTL_SOURCES
    src/CHTL/CHTLContext.cpp
    src/CHTL/CHTLGenerator.cpp
    src/CHTL/CHTLLexer.cpp
    src/CHTL/CHTLLoader.cpp
    src/CHTL/CHTLManage.cpp
    src/CHTL/CHTLParser.cpp
    src/CHTL/CHTLState.cpp
    src/CHTL/CHTLIOStream.cpp
    src/CHTL/CMODSystem.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/ElementNode.cpp
    src/CHTL/CHTLNode/TextNode.cpp
    src/CHTL/CHTLNode/CommentNode.cpp
    src/CHTL/CHTLNode/TemplateNode.cpp
    src/CHTL/CHTLNode/CustomNode.cpp
    src/CHTL/CHTLNode/StyleNode.cpp
    src/CHTL/CHTLNode/ScriptNode.cpp
    src/CHTL/CHTLNode/OriginNode.cpp
    src/CHTL/CHTLNode/ImportNode.cpp
    src/CHTL/CHTLNode/ConfigNode.cpp
    src/CHTL/CHTLNode/NamespaceNode.cpp
    src/CHTL/CHTLNode/OperatorNode.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL JS/CHTLJSContext.cpp
    src/CHTL JS/CHTLJSGenerator.cpp
    src/CHTL JS/CHTLJSLexer.cpp
    src/CHTL JS/CHTLJSLoader.cpp
    src/CHTL JS/CHTLJSManage.cpp
    src/CHTL JS/CHTLJSParser.cpp
    src/CHTL JS/CHTLJSState.cpp
    src/CHTL JS/CHTLJSIOStream.cpp
    src/CHTL JS/CJMODSystem.cpp
    src/Scanner/CHTLUnifiedScanner.cpp
    src/CodeMerger/CodeMerger.cpp
    src/CompilerDispatcher/CompilerDispatcher.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/Util/ZipUtil/ZipUtil.cpp
    src/Util/StringUtil/StringUtil.cpp
)

# Header files
set(CHTL_HEADERS
    include/CHTL/CHTLContext.h
    include/CHTL/CHTLGenerator.h
    include/CHTL/CHTLLexer.h
    include/CHTL/CHTLLoader.h
    include/CHTL/CHTLManage.h
    include/CHTL/CHTLParser.h
    include/CHTL/CHTLState.h
    include/CHTL/CHTLIOStream.h
    include/CHTL/CMODSystem.h
    include/CHTL/CHTLNode/BaseNode.h
    include/CHTL/CHTLNode/ElementNode.h
    include/CHTL/CHTLNode/TextNode.h
    include/CHTL/CHTLNode/CommentNode.h
    include/CHTL/CHTLNode/TemplateNode.h
    include/CHTL/CHTLNode/CustomNode.h
    include/CHTL/CHTLNode/StyleNode.h
    include/CHTL/CHTLNode/ScriptNode.h
    include/CHTL/CHTLNode/OriginNode.h
    include/CHTL/CHTLNode/ImportNode.h
    include/CHTL/CHTLNode/ConfigNode.h
    include/CHTL/CHTLNode/NamespaceNode.h
    include/CHTL/CHTLNode/OperatorNode.h
    include/CHTL/CHTLLexer/Lexer.h
    include/CHTL/CHTLLexer/GlobalMap.h
    include/CHTL/CHTLLexer/Token.h
    include/CHTL JS/CHTLJSContext.h
    include/CHTL JS/CHTLJSGenerator.h
    include/CHTL JS/CHTLJSLexer.h
    include/CHTL JS/CHTLJSLoader.h
    include/CHTL JS/CHTLJSManage.h
    include/CHTL JS/CHTLJSParser.h
    include/CHTL JS/CHTLJSState.h
    include/CHTL JS/CHTLJSIOStream.h
    include/CHTL JS/CJMODSystem.h
    include/Scanner/CHTLUnifiedScanner.h
    include/CodeMerger/CodeMerger.h
    include/CompilerDispatcher/CompilerDispatcher.h
    include/Util/FileSystem/FileSystem.h
    include/Util/ZipUtil/ZipUtil.h
    include/Util/StringUtil/StringUtil.h
)

# Create CHTL library
add_library(chtl STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

# Link libraries
target_link_libraries(chtl ${ANTLR4_LIBRARIES})

# Create executable
add_executable(chtl_cli src/main.cpp)
target_link_libraries(chtl_cli chtl)

# Test executable
add_executable(chtl_test src/test/main.cpp)
target_link_libraries(chtl_test chtl)

# Install targets
install(TARGETS chtl chtl_cli DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)