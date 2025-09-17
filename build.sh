#!/bin/bash

# CHTL项目构建脚本

echo "CHTL项目构建脚本"
echo "=================="

# 检查是否安装了必要的工具
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到cmake，请先安装cmake"
    exit 1
fi

if ! command -v make &> /dev/null; then
    echo "错误: 未找到make，请先安装make"
    exit 1
fi

# 创建构建目录
echo "创建构建目录..."
mkdir -p build
cd build

# 配置CMake
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译项目..."
make -j$(nproc)

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "可执行文件位置: build/bin/chtl"
    
    # 创建输出目录
    mkdir -p output
    
    # 运行测试
    echo "运行测试..."
    ./bin/chtl ../test.chtl output
    
    if [ $? -eq 0 ]; then
        echo "测试成功！"
        echo "输出文件:"
        echo "  - output/test.html"
        echo "  - output/test.css"
        echo "  - output/test.js"
    else
        echo "测试失败！"
        exit 1
    fi
else
    echo "编译失败！"
    exit 1
fi

echo "构建完成！"