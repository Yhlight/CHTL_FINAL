# CHTL CLI Module
add_executable(chtl_cli
    CHTLCli.h
    CHTLCli.cpp
    main.cpp
)

target_link_libraries(chtl_cli
    CHTLCore
    CHTLJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

target_include_directories(chtl_cli PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 安装CLI工具
install(TARGETS chtl_cli DESTINATION bin)