#!/bin/bash

# CHTL编译器构建脚本

echo "Building CHTL Compiler..."

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
echo "Compiling..."
make -j$(nproc)

# 检查编译是否成功
if [ $? -eq 0 ]; then
    echo "Build successful!"
    
    # 运行测试
    echo "Running tests..."
    make test
    
    # 测试编译器
    echo "Testing compiler with basic example..."
    ./chtl ../examples/basic.chtl output.html
    
    if [ -f "output.html" ]; then
        echo "Compilation test successful!"
        echo "Output file: output.html"
        echo "Content preview:"
        head -20 output.html
    else
        echo "Compilation test failed!"
        exit 1
    fi
else
    echo "Build failed!"
    exit 1
fi