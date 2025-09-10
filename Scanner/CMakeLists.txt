add_library(scanner_lib STATIC
    UnifiedScanner.cpp
)

target_include_directories(scanner_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
