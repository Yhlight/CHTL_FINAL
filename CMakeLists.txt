# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CHTL_Compiler)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target
# All source files will be relative to this CMakeLists.txt file
add_executable(chtl_compiler
    src/main.cpp
    src/Util/StringUtil/StringUtil.cpp
    src/Util/FileSystem/FileSystem.cpp
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
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
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
)

# We will add more source files and include directories as we build them.
# For now, let's just include the root of our source tree.
include_directories(src)
