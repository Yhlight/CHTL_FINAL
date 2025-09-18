add_library(Util STATIC
    FileSystem/FileSystem.cpp
    StringUtil/StringUtil.cpp
)

target_include_directories(Util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
