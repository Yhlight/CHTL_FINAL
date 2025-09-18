# Create a static library for the utility functions
add_library(CHTL_Util STATIC
    FileSystem/FileSystem.cpp
    FileSystem/FileSystem.h
    StringUtil/StringUtil.cpp
    StringUtil/StringUtil.h
)

# Publicize the include directory so other targets can find the headers
target_include_directories(CHTL_Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
