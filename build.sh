#!/bin/bash

# CHTL Compiler Build Script

echo "Building CHTL Compiler..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running CHTL Compiler test..."
    ./bin/chtl
else
    echo "Build failed!"
    exit 1
fi