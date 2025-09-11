add_library(dispatcher STATIC
    CompilerDispatcher.cpp
    ImportVisitor.cpp
)

target_include_directories(dispatcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Dispatcher needs all the compiler components
target_link_libraries(dispatcher PUBLIC
    scanner
    chtl_parser
    chtl_generator
    chtl_js_parser
    chtl_js_generator
    loader
)
