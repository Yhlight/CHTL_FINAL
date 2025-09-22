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
)
list(APPEND CHTL_SOURCES "miniz.c")
message("Total sources: ${CHTL_SOURCES}")