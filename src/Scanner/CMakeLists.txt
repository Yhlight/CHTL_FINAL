add_library(Scanner STATIC
    CHTLUnifiedScanner.cpp
)

target_include_directories(Scanner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)