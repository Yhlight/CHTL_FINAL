# 测试 CMakeLists.txt

# 创建测试可执行文件
add_executable(CHTL_Compiler_test
    test_main.cpp
)

# 链接库
target_link_libraries(CHTL_Compiler_test
    CHTLCore
)

# 添加测试
add_test(NAME CHTL_Compiler_test COMMAND CHTL_Compiler_test)