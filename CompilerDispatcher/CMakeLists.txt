add_executable(chtl_compiler main.cpp)

target_link_libraries(chtl_compiler PRIVATE
    chtl_lib
    scanner_lib
    chtl_js_lib
)

target_include_directories(chtl_compiler PRIVATE
    ..
)
