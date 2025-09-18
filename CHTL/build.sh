#!/bin/bash

echo "开始构建CHTL项目..."

# 创建构建目录
mkdir -p build
cd build

# 运行CMake配置
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 编译项目
echo "编译项目..."
make -j$(nproc)

if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "可执行文件位置: build/bin/chtl"
    
    # 测试编译
    echo "测试编译..."
    ./bin/chtl ../test.chtl --output test.html
    
    if [ $? -eq 0 ]; then
        echo "测试编译成功！输出文件: test.html"
        echo "HTML内容预览:"
        head -20 test.html
    else
        echo "测试编译失败！"
        exit 1
    fi
else
    echo "编译失败！"
    exit 1
fi