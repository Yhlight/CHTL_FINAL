# CMake generated Testfile for 
# Source directory: /workspace/test
# Build directory: /workspace/build/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(UnifiedScannerTest "/workspace/build/bin/chtl_test" "--gtest_filter=UnifiedScannerTest.*")
set_tests_properties(UnifiedScannerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/test/CMakeLists.txt;15;add_test;/workspace/test/CMakeLists.txt;0;")
add_test(CHTLLexerTest "/workspace/build/bin/chtl_test" "--gtest_filter=CHTLLexerTest.*")
set_tests_properties(CHTLLexerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/test/CMakeLists.txt;16;add_test;/workspace/test/CMakeLists.txt;0;")
add_test(CHTLParserTest "/workspace/build/bin/chtl_test" "--gtest_filter=CHTLParserTest.*")
set_tests_properties(CHTLParserTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/test/CMakeLists.txt;17;add_test;/workspace/test/CMakeLists.txt;0;")
