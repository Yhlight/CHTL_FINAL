# Test/ParserTest/CMakeLists.txt

# Define the executable for this test
add_executable(ParserTest main.cpp)

# Link the test executable against our CHTL library
target_link_libraries(ParserTest PRIVATE CHTL_LIB)

# The executable needs to find the CHTL headers
target_include_directories(ParserTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL
)

# Copy data files needed for the test to the build directory
file(COPY
    ${CMAKE_SOURCE_DIR}/main.chtl
    ${CMAKE_SOURCE_DIR}/templates.chtl
    DESTINATION ${CMAKE_BINARY_DIR}
)
