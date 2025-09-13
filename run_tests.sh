#!/bin/bash
set -e # Exit immediately if a command exits with a non-zero status.

echo "--- Configuring and Building CHTL Compiler ---"

# Go to the project root directory, just in case
cd "$(dirname "$0")"

# Clean and configure the project using modern CMake
rm -rf build
cmake -S . -B build

# Build the project
echo "--- Building ---"
cmake --build build

# Run the tests
echo "--- Running Tests ---"
cd build
ctest --output-on-failure --verbose

echo "--- Test run complete ---"
