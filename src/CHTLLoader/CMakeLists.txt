add_library(CHTLLoader STATIC
    CHTLLoader.cpp
)

target_include_directories(CHTLLoader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against Util library to get access to FileSystem
target_link_libraries(CHTLLoader PUBLIC Util)
