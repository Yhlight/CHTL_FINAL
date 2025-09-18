# Gather all source files for the CHTL library
file(GLOB_RECURSE CHTL_SOURCES "*.cpp" "*.h")

# Create the main CHTL library
add_library(CHTL STATIC ${CHTL_SOURCES})

# It needs the utility library
target_link_libraries(CHTL PRIVATE CHTL_Util)

# Publicize its own include directory
target_include_directories(CHTL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
