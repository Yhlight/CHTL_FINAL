# Test/ParserTest/CMakeLists.txt

# Define the executable for this test
add_executable(ParserTest main.cpp)

# Link the test executable against our CHTL library
target_link_libraries(ParserTest PRIVATE CHTL_LIB)

# The executable needs to find the CHTL headers
target_include_directories(ParserTest PRIVATE
    ${CMAKE_SOURCE_DIR}/CHTL
)
