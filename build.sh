#!/bin/bash

set -e

echo "Building CHTL C++ Implementation..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc)

echo "Build completed successfully!"
echo ""
echo "Available executables:"
echo "  ./chtl - Main CHTL compiler"
echo "  ./chtl_test - Test suite"
echo ""
echo "Usage:"
echo "  ./chtl <input.chtl> <output.html>"
echo "  ./chtl_test"