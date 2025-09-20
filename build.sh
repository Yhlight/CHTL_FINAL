#!/bin/bash

# Exit on error
set -e

echo "Building CHTL compiler..."
g++ -std=c++17 -o chtl_compiler CHTL/CHTLGenerator/CHTLGenerator.cpp CHTL/CompilerDispatcher/CompilerDispatcher.cpp CHTL/CHTLLexer/CHTLLexer.cpp CHTL/CHTLLexer/Token.cpp CHTL/CHTLParser/CHTLParser.cpp CHTL/Expression/ExpressionEvaluator.cpp CHTL/Scanner/CHTLUnifiedScanner.cpp CHTL/CodeMerger/CodeMerger.cpp CHTL/main.cpp CHTL/CHTLNode/ElementNode.cpp CHTL/CHTLNode/OriginNode.cpp CHTL/CHTLNode/StyleNode.cpp CHTL/CHTLNode/TextNode.cpp CHTL/CHTLNode/TemplateDefinitionNode.cpp CHTL/CHTLNode/CustomDeclarationNode.cpp CHTL/CHTLNode/ReactiveValueNode.cpp CHTL/CHTLNode/TemplateDeclarationNode.cpp CHTL/CHTLNode/ImportNode.cpp Util/StringUtil/StringUtil.cpp Util/FileSystem/FileSystem.cpp

echo "Building scanner test..."
g++ -std=c++17 -o scanner_test Test/ScannerTest/main.cpp CHTL/Scanner/CHTLUnifiedScanner.cpp Util/StringUtil/StringUtil.cpp -I.

echo "Build complete."
