add_library(merger STATIC
    CodeMerger.cpp
)

target_include_directories(merger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Merger needs the dispatcher to get the compilation result
target_link_libraries(merger PUBLIC dispatcher)
