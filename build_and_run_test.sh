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

# --- Test with good example ---
echo "--- Running compiler on good example (example.chtl) ---"
./chtl-compiler ../example.chtl
echo ""
echo "--- Content of generated output.html ---"
cat output.html
echo ""

# --- Test with bad example ---
echo "--- Running compiler on bad example (bad_example.chtl) ---"
echo "--- Expecting a compilation error... ---"
# We expect this to fail, so we use ! to invert the exit code.
# The script will continue if the compiler fails (as expected),
# but will exit with an error if it succeeds (which would be a bug).
! ./chtl-compiler ../bad_example.chtl
echo "--- Compiler correctly failed as expected. ---"
