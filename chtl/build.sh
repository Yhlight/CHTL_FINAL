#!/bin/bash

# CHTL编译器构建脚本

echo "开始构建CHTL编译器..."

# 创建构建目录
mkdir -p build
cd build

# 运行CMake配置
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译项目..."
make -j$(nproc)

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "可执行文件位置: build/chtl"
    echo "测试文件位置: test/simple.chtl"
    echo ""
    echo "使用方法:"
    echo "  ./chtl ../test/simple.chtl output.html"
else
    echo "编译失败！"
    exit 1
fi