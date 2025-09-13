# CMake generated Testfile for
# Source directory: /app
# Build directory: /app
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(run_delete_test "/app/chtl_compiler" "/app/delete_test.chtl")
set_tests_properties(run_delete_test PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;34;add_test;/app/CMakeLists.txt;0;")
add_test(run_insert_test "/app/chtl_compiler" "/app/insert_test.chtl")
set_tests_properties(run_insert_test PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;38;add_test;/app/CMakeLists.txt;0;")
