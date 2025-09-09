#!/bin/bash

# CHTL Build Script

echo "Building CHTL Project..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    
    # Run tests
    echo "Running tests..."
    if [ -f "./Test/TestBasic" ]; then
        ./Test/TestBasic
    else
        echo "Test executable not found"
    fi
    
    # Compile example
    echo "Compiling example..."
    if [ -f "./chtl" ]; then
        ./chtl ../examples/hello_world.chtl ../examples/hello_world.html
        echo "Example compiled to ../examples/hello_world.html"
    else
        echo "CHTL compiler not found"
    fi
    
else
    echo "Build failed!"
    exit 1
fi

echo "Build process completed."