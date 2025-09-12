#!/bin/bash

# CHTL项目构建脚本

set -e

echo "CHTL Compiler Build Script"
echo "=========================="

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Please install CMake first."
    exit 1
fi

# 检查C++编译器是否安装
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# 创建构建目录
echo "Creating build directory..."
mkdir -p build
cd build

# 配置项目
echo "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "Building project..."
make -j$(nproc)

echo ""
echo "Build completed successfully!"
echo "=========================="
echo "Executable: build/bin/chtl"
echo ""

# 运行测试
if [ -f "test.chtl" ]; then
    echo "Running test compilation..."
    ./bin/chtl -v test.chtl
    echo ""
    echo "Test completed! Check test.html for output."
fi

echo "Build script finished."