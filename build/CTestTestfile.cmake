# CMake generated Testfile for
# Source directory: /app
# Build directory: /app/build
#
# This file includes the relevant testing commands required for
# testing this directory and lists subdirectories to be tested as well.
add_test(LexerTests "/app/build/run_lexer_tests")
set_tests_properties(LexerTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;36;add_test;/app/CMakeLists.txt;0;")
add_test(ParserTests "/app/build/run_parser_tests")
set_tests_properties(ParserTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;41;add_test;/app/CMakeLists.txt;0;")
add_test(GeneratorTests "/app/build/run_generator_tests")
set_tests_properties(GeneratorTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;46;add_test;/app/CMakeLists.txt;0;")
add_test(AttributeTests "/app/build/run_attribute_tests")
set_tests_properties(AttributeTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;51;add_test;/app/CMakeLists.txt;0;")
add_test(StyleTests "/app/build/run_style_tests")
set_tests_properties(StyleTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;56;add_test;/app/CMakeLists.txt;0;")
add_test(ErrorTests "/app/build/run_error_tests")
set_tests_properties(ErrorTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;61;add_test;/app/CMakeLists.txt;0;")
add_test(TemplateTests "/app/build/run_template_tests")
set_tests_properties(TemplateTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;66;add_test;/app/CMakeLists.txt;0;")
add_test(ElementTemplateTests "/app/build/run_element_template_tests")
set_tests_properties(ElementTemplateTests PROPERTIES  _BACKTRACE_TRIPLES "/app/CMakeLists.txt;71;add_test;/app/CMakeLists.txt;0;")
