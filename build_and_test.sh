#!/bin/bash
set -e
mkdir -p build
cd build
cmake ..
make
./test_parser ../tests/parser_test.cjjs
