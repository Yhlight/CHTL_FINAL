# Create a static library for the utility functions
add_library(Util STATIC
    FileSystem/FileSystem.cpp
    StringUtil/StringUtil.cpp
)

# Publicly expose the include directory for this library
target_include_directories(Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
