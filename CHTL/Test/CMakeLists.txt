# 查找 Google Test
find_package(GTest REQUIRED)

# 添加测试子目录
add_subdirectory(TokenTest)

# 启用测试
enable_testing()