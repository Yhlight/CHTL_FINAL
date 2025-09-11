add_library(loader STATIC
    ModuleLoader.cpp
)

target_include_directories(loader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
