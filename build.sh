#!/bin/bash

# CHTL项目构建脚本

echo "CHTL Project Build Script"
echo "========================"

# 检查依赖
echo "Checking dependencies..."

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Please install CMake 3.16 or later."
    exit 1
fi

# 检查C++编译器
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# 检查ANTLR4
if ! pkg-config --exists antlr4-runtime; then
    echo "Warning: ANTLR4 runtime not found. Some features may not work."
    echo "Please install ANTLR4 runtime library."
fi

echo "Dependencies check completed."

# 创建构建目录
echo "Creating build directory..."
mkdir -p build
cd build

# 配置CMake
echo "Configuring CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "Building project..."
make -j$(nproc)

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Available executables:"
    echo "  - chtl: Main CHTL compiler"
    echo "  - test_compiler: Test suite"
    echo ""
    echo "Usage:"
    echo "  ./chtl <input_file> [options]"
    echo "  ./test_compiler"
else
    echo "Build failed!"
    exit 1
fi

# 运行测试
echo "Running tests..."
./test_compiler

if [ $? -eq 0 ]; then
    echo "All tests passed!"
else
    echo "Some tests failed!"
    exit 1
fi

echo "Build process completed successfully!"