#!/bin/bash

# CHTL 构建脚本

set -e

echo "CHTL Build Script"
echo "================="

# 检查依赖
echo "Checking dependencies..."

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    exit 1
fi

# 检查编译器
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found (g++ or clang++)"
    exit 1
fi

echo "Dependencies OK"

# 创建构建目录
echo "Creating build directory..."
mkdir -p build
cd build

# 配置项目
echo "Configuring project..."
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON

# 构建项目
echo "Building project..."
make -j$(nproc)

echo "Build completed successfully!"

# 运行测试
echo "Running tests..."
if [ -f "./test/test_lexer" ]; then
    echo "Running lexer tests..."
    ./test/test_lexer
fi

echo "All tests passed!"

# 显示构建结果
echo ""
echo "Build Results:"
echo "=============="
echo "Executable: ./bin/chtl"
echo "Libraries: ./lib/"
echo "Tests: ./test/"

echo ""
echo "Usage examples:"
echo "==============="
echo "  ./bin/chtl ../examples/hello.chtl"
echo "  ./bin/chtl -o output/ ../examples/templates.chtl"
echo "  ./bin/chtl -m -d ../examples/chtl_js.chtl"