#!/usr/bin/env python3
import subprocess
import os

# Change to workspace directory
os.chdir('/workspace')

# Run cmake with verbose output
result = subprocess.run(['cmake', '-B', 'cmake-build', '-S', '.', '-DCMAKE_VERBOSE_MAKEFILE=ON'], 
                       capture_output=True, text=True)

print("STDOUT:")
print(result.stdout)
print("\nSTDERR:")
print(result.stderr)

# Check if miniz.c is in the source list
if 'miniz.c' in result.stdout:
    print("\n✓ miniz.c found in CMake output")
else:
    print("\n✗ miniz.c NOT found in CMake output")

# Check if *.c pattern is working
if '*.c' in result.stdout:
    print("\n✓ *.c pattern found in CMake output")
else:
    print("\n✗ *.c pattern NOT found in CMake output")