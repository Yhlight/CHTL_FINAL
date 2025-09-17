# CMake generated Testfile for
# Source directory: /app
# Build directory: /app/build
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(LexerTests "/app/build/run_lexer_tests")
set_tests_properties(LexerTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;32;add_test;/app/CMakeLists.txt;0;")
add_test(ParserTests "/app/build/run_parser_tests")
set_tests_properties(ParserTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;37;add_test;/app/CMakeLists.txt;0;")
add_test(GeneratorTests "/app/build/run_generator_tests")
set_tests_properties(GeneratorTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;42;add_test;/app/CMakeLists.txt;0;")
add_test(AttributeTests "/app/build/run_attribute_tests")
set_tests_properties(AttributeTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;47;add_test;/app/CMakeLists.txt;0;")
add_test(StyleTests "/app/build/run_style_tests")
set_tests_properties(StyleTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;52;add_test;/app/CMakeLists.txt;0;")
add_test(ErrorTests "/app/build/run_error_tests")
set_tests_properties(ErrorTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;57;add_test;/app/CMakeLists.txt;0;")
