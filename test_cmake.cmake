file(GLOB_RECURSE CHTL_SOURCES
    "CHTL/*.cpp"
    "CHTL/*/*.cpp"
    "CHTL/*/*/*.cpp"
    "CHTL JS/*.cpp"
    "CHTL JS/*/*.cpp"
    "CHTL JS/*/*/*.cpp"
    "Scanner/*.cpp"
    "Util/*.cpp"
    "Util/*/*.cpp"
    "miniz.c"
)
foreach(source ${CHTL_SOURCES})
    message("Found: ${source}")
endforeach()