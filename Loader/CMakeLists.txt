add_library(loader STATIC
    Loader.cpp
)

target_include_directories(loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
