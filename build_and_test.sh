#!/bin/bash
set -e

echo "--- BUILDING PROJECT ---"
# Clean and build from scratch
rm -rf CMakeFiles/ CMakeCache.txt cmake_install.cmake Makefile CTestTestfile.cmake *.a chtl_compiler test_runner test_parser test_scanner build/
cmake .
make

echo -e "\n--- RUNNING SNAPSHOT TESTS ---"
./test_runner
