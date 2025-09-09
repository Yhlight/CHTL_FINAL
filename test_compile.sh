#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
cmake ..

# 编译项目
make -j4

# 运行测试
if [ -f chtl_cli ]; then
    echo "编译成功！"
    echo "测试编译 example.chtl..."
    ./chtl_cli ../example.chtl test_output.html
    
    if [ -f test_output.html ]; then
        echo "输出文件生成成功！"
        echo "输出内容："
        cat test_output.html
    else
        echo "输出文件生成失败！"
    fi
else
    echo "编译失败！"
    exit 1
fi