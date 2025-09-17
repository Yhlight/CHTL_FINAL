#!/bin/bash
set -e

# --- Build ---
echo "--- Building project ---"
rm -rf build
mkdir build
cd build
cmake ..
make
echo "--- Build complete ---"
echo ""

# --- Run CompilerTest for unit/integration tests ---
echo "--- Running CompilerTest ---"
./CompilerTest 2>&1
echo ""

# --- Test CLI with good example ---
echo "--- Running compiler on good example (example.chtl) ---"
./chtl-compiler ../example.chtl
echo ""
echo "--- Content of generated output.html ---"
cat output.html
echo ""

# --- Test CLI with bad example ---
echo "--- Running compiler on bad example (bad_example.chtl) ---"
echo "--- Expecting a compilation error... ---"
! ./chtl-compiler ../bad_example.chtl 2>&1
echo "--- Compiler correctly failed as expected. ---"
