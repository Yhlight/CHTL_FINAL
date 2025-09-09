add_library(CJMOD STATIC
    Syntax.cpp
    Arg.cpp
    CJMODScanner.cpp
    CJMODGenerator.cpp
    CHTLJSFunction.cpp
)

target_include_directories(CJMOD PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)