#!/bin/bash
# This script automates the build and test process for the CHTL compiler.
set -e

echo "--- Starting CHTL build and test script ---"

# Navigate to the script's directory to ensure paths are correct
cd "$(dirname "$0")"

echo "--- Cleaning and preparing build directory ---"
rm -rf build
mkdir -p build
cd build

echo "--- Running CMake to configure project ---"
cmake ..

echo "--- Running Make to compile all targets ---"
make

echo ""
echo "--- Running Test Suite ---"
./run_tests

echo ""
echo "--- Running Main Executable for Manual Verification ---"
./chtl

echo ""
echo "--- CHTL build and test script finished ---"
