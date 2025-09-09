#!/bin/bash

# CHTL 项目构建脚本

set -e

echo "Building CHTL Compiler..."

# 创建构建目录
mkdir -p build
cd build

# 配置 CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "Compiling..."
make -j$(nproc)

echo "Build completed successfully!"

# 创建输出目录
mkdir -p ../test/output

# 测试编译
echo "Testing compilation..."
./bin/chtl ../test/simple.chtl -o ../test/output/simple

echo "Test completed! Check test/output/ for generated files."