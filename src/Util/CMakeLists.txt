add_library(Util STATIC
    StringUtil/StringUtil.cpp
    FileSystem/FileSystem.cpp
    ZipUtil/ZipUtil.cpp
)

target_include_directories(Util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
