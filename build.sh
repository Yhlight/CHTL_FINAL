#!/bin/bash

# This script builds the CHTL compiler and all associated tests.
# Ensure you have g++ installed.

# Exit immediately if a command exits with a non-zero status.
set -e

echo "--- Configuring Build ---"

# Common source files for the CHTL library
CHTL_SOURCES="
CHTL/CHTLCompiler.cpp
CHTL/CHTLGenerator/CHTLGenerator.cpp
CHTL/CHTLLoader/CHTLLoader.cpp
CHTL/CHTLLexer/CHTLLexer.cpp
CHTL/CHTLLexer/Token.cpp
CHTL/CHTLParser/CHTLParser.cpp
CHTL/Expression/ExpressionEvaluator.cpp
CHTL/CHTLNode/ElementNode.cpp
CHTL/CHTLNode/TextNode.cpp
CHTL/CHTLNode/StyleNode.cpp
CHTL/CHTLNode/OriginNode.cpp
CHTL/CHTLNode/TemplateDeclarationNode.cpp
CHTL/CHTLNode/TemplateDefinitionNode.cpp
CHTL/CHTLNode/CustomDeclarationNode.cpp
CHTL/CHTLNode/ImportNode.cpp
CHTL/CHTLNode/ScriptNode.cpp
"

# Compiler flags
CXX="g++"
CXX_FLAGS="-std=c++17 -I. -Wall -Wextra -g"

echo "--- Building Tests ---"

# Build Full Pipeline Test
echo "Building full_pipeline_test..."
$CXX $CXX_FLAGS -o full_pipeline_test Test/FullPipelineTest/main.cpp $CHTL_SOURCES

# Build Arithmetic Test
echo "Building arithmetic_test..."
$CXX $CXX_FLAGS -o arithmetic_test Test/ArithmeticTest/main.cpp $CHTL_SOURCES

# Build Conditional Test
echo "Building conditional_test..."
$CXX $CXX_FLAGS -o conditional_test Test/ConditionalTest/main.cpp $CHTL_SOURCES


echo "--- Build Complete ---"
echo "You can now run tests, for example: ./full_pipeline_test"
