#!/bin/bash

# Exit on error
set -e

# 1. Configure
echo "--- Configuring with CMake ---"
mkdir -p cmake-build
cd cmake-build
cmake ..

# 2. Build
echo "--- Building with Make ---"
make

# 3. Go back to root
cd ..

# 4. Run the test
echo "--- Running EventHandlerTest ---"
./build/EventHandlerTest
