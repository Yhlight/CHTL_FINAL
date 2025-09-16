#!/bin/bash
set -e

echo "--- BUILDING PROJECT ---"
# Clean and build from scratch
rm -rf CMakeFiles/ CMakeCache.txt cmake_install.cmake Makefile CTestTestfile.cmake *.a chtl_compiler test_runner test_parser test_scanner build/
cmake .
make

echo -e "\n--- RUNNING ALL TESTS ---"

# Run the original parser test
echo -e "\n>>> Running test_parser on tests/parser_test.cjjs"
./test_parser tests/parser_test.cjjs
echo "--------------------------------"

# Run all scanner tests
echo -e "\n>>> Running test_scanner on tests/refactor_test.chtl"
./test_scanner tests/refactor_test.chtl
echo "--------------------------------"

echo -e "\n>>> Running test_scanner on tests/block_recognition_test.chtl"
./test_scanner tests/block_recognition_test.chtl
echo "--------------------------------"

echo -e "\n>>> Running test_scanner on tests/chtl_constructs_test.chtl"
./test_scanner tests/chtl_constructs_test.chtl
echo "--------------------------------"

echo -e "\n>>> Running test_scanner on tests/chtljs_separation_test.chtl"
./test_scanner tests/chtljs_separation_test.chtl
echo "--------------------------------"

echo -e "\n>>> Running test_scanner on tests/responsive_values_test.chtl"
./test_scanner tests/responsive_values_test.chtl
echo "--------------------------------"

# Run the new scanner JS test
echo -e "\n>>> Running test_scanner on tests/scanner_js_test.chtl"
./test_scanner tests/scanner_js_test.chtl
echo "--------------------------------"

echo -e "\n--- ALL TESTS PASSED ---"
