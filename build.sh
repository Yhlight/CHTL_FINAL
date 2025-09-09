#!/bin/bash

# CHTL 构建脚本

echo "Building CHTL..."

# 创建构建目录
mkdir -p build
cd build

# 运行 CMake
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed"
    exit 1
fi

# 编译项目
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

echo "Build completed successfully!"

# 运行测试
echo "Running test..."
cd ..
./build/bin/chtl test.chtl test_output.html

if [ $? -eq 0 ]; then
    echo "Test completed successfully!"
    echo "Generated file: test_output.html"
else
    echo "Test failed"
    exit 1
fi