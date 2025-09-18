# CMake generated Testfile for
# Source directory: /app/Test
# Build directory: /app/build/Test
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(ast_test "/app/build/Test/ast_test")
set_tests_properties(ast_test PROPERTIES  _BACKTRACE_TRIPLES "/app/Test/CMakeLists.txt;11;add_test;/app/Test/CMakeLists.txt;0;")
add_test(token_test "/app/build/Test/token_test")
set_tests_properties(token_test PROPERTIES  _BACKTRACE_TRIPLES "/app/Test/CMakeLists.txt;18;add_test;/app/Test/CMakeLists.txt;0;")
add_test(full_pipeline_test "/app/build/Test/full_pipeline_test")
set_tests_properties(full_pipeline_test PROPERTIES  _BACKTRACE_TRIPLES "/app/Test/CMakeLists.txt;25;add_test;/app/Test/CMakeLists.txt;0;")
add_test(parser_test "/app/build/Test/parser_test")
set_tests_properties(parser_test PROPERTIES  _BACKTRACE_TRIPLES "/app/Test/CMakeLists.txt;32;add_test;/app/Test/CMakeLists.txt;0;")
