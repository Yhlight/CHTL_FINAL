#!/bin/bash

# CHTL项目构建脚本

echo "Building CHTL Project..."

# 创建构建目录
mkdir -p build
cd build

# 运行CMake配置
cmake ..

# 编译项目
make -j$(nproc)

echo "Build completed!"

# 运行测试
echo "Running tests..."
./bin/chtl_test

echo "All done!"