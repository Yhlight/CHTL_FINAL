#!/bin/bash

# ANTLR4构建脚本
# 用于生成CHTL和CHTL JS的ANTLR4解析器

set -e

# 配置
ANTLR_VERSION="4.13.1"
ANTLR_JAR="antlr-${ANTLR_VERSION}-complete.jar"
GRAMMAR_DIR="grammar"
OUTPUT_DIR="generated"
JAVA_PACKAGE="chtl.antlr"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building ANTLR4 parsers for CHTL...${NC}"

# 检查Java是否安装
if ! command -v java &> /dev/null; then
    echo -e "${RED}Error: Java is not installed. Please install Java 8 or higher.${NC}"
    exit 1
fi

# 检查ANTLR JAR文件
if [ ! -f "$ANTLR_JAR" ]; then
    echo -e "${YELLOW}Downloading ANTLR4 JAR file...${NC}"
    wget "https://www.antlr.org/download/${ANTLR_JAR}"
fi

# 创建输出目录
mkdir -p "$OUTPUT_DIR"

# 生成CHTL解析器
echo -e "${GREEN}Generating CHTL parser...${NC}"
java -jar "$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -package "$JAVA_PACKAGE" \
    -o "$OUTPUT_DIR/chtl" \
    -listener \
    -visitor \
    "$GRAMMAR_DIR/CHTL.g4"

# 生成CHTL JS解析器
echo -e "${GREEN}Generating CHTL JS parser...${NC}"
java -jar "$ANTLR_JAR" \
    -Dlanguage=Cpp \
    -package "$JAVA_PACKAGE" \
    -o "$OUTPUT_DIR/chtljs" \
    -listener \
    -visitor \
    "$GRAMMAR_DIR/CHTLJS.g4"

# 创建CMakeLists.txt文件
echo -e "${GREEN}Creating CMakeLists.txt...${NC}"
cat > "$OUTPUT_DIR/CMakeLists.txt" << EOF
# ANTLR4 Generated Files
cmake_minimum_required(VERSION 3.10)

# CHTL Parser
add_library(chtl_parser
    chtl/CHTLLexer.cpp
    chtl/CHTLParser.cpp
    chtl/CHTLBaseListener.cpp
    chtl/CHTLBaseVisitor.cpp
    chtl/CHTLListener.cpp
    chtl/CHTLVisitor.cpp
)

# CHTL JS Parser
add_library(chtljs_parser
    chtljs/CHTLJSLexer.cpp
    chtljs/CHTLJSParser.cpp
    chtljs/CHTLJSBaseListener.cpp
    chtljs/CHTLJSBaseVisitor.cpp
    chtljs/CHTLJSListener.cpp
    chtljs/CHTLJSVisitor.cpp
)

# Include directories
target_include_directories(chtl_parser PUBLIC chtl)
target_include_directories(chtljs_parser PUBLIC chtljs)

# Link ANTLR4 runtime
find_package(antlr4-runtime REQUIRED)
target_link_libraries(chtl_parser antlr4-runtime)
target_link_libraries(chtljs_parser antlr4-runtime)
EOF

# 创建头文件包含目录
mkdir -p "$OUTPUT_DIR/include/chtl/antlr"

# 创建示例使用文件
echo -e "${GREEN}Creating example usage...${NC}"
cat > "$OUTPUT_DIR/example_usage.cpp" << 'EOF'
#include "chtl/CHTLLexer.h"
#include "chtl/CHTLParser.h"
#include "chtljs/CHTLJSLexer.h"
#include "chtljs/CHTLJSParser.h"
#include <iostream>
#include <fstream>

using namespace chtl::antlr;

void parseCHTL(const std::string& input) {
    antlr4::ANTLRInputStream inputStream(input);
    CHTLLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CHTLParser parser(&tokens);
    
    auto tree = parser.program();
    std::cout << "CHTL parsed successfully!" << std::endl;
}

void parseCHTLJS(const std::string& input) {
    antlr4::ANTLRInputStream inputStream(input);
    CHTLJSLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CHTLJSParser parser(&tokens);
    
    auto tree = parser.program();
    std::cout << "CHTL JS parsed successfully!" << std::endl;
}

int main() {
    std::string chtlInput = R"(
        html {
            head { title { text { Hello World } } }
            body { div { text { Hello CHTL! } } }
        }
    )";
    
    std::string chtljsInput = R"(
        const message = "Hello CHTL JS!";
        {{#myDiv}}->addEventListener('click', function() {
            console.log(message);
        });
    )";
    
    parseCHTL(chtlInput);
    parseCHTLJS(chtljsInput);
    
    return 0;
}
EOF

echo -e "${GREEN}ANTLR4 build completed successfully!${NC}"
echo -e "${YELLOW}Generated files are in: $OUTPUT_DIR${NC}"
echo -e "${YELLOW}To use the parsers, include the generated headers and link against the libraries.${NC}"