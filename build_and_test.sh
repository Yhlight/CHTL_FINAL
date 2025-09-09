#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 运行 CMake 配置
cmake ..

# 编译项目
make -j$(nproc)

# 运行测试
if [ -f bin/chtl ]; then
    echo "Running CHTL compiler test..."
    ./bin/chtl ../test.chtl
else
    echo "Error: CHTL compiler not found!"
    exit 1
fi