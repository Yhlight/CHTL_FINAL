# CHTL 模块系统库
add_library(CHTLModule STATIC
    ModuleManager.cpp
)

target_include_directories(CHTLModule PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLNode
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLTemplate
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLCustom
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLOrigin
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLImport
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLNamespace
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLConstraint
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLConfig
)

target_link_libraries(CHTLModule PUBLIC
    CHTLNode
    CHTLTemplate
    CHTLCustom
    CHTLOrigin
    CHTLImport
    CHTLNamespace
    CHTLConstraint
    CHTLConfig
)

target_compile_features(CHTLModule PUBLIC cxx_std_17)

# 设置编译选项
target_compile_definitions(CHTLModule PRIVATE
    CHTL_MODULE_VERSION_MAJOR=1
    CHTL_MODULE_VERSION_MINOR=0
    CHTL_MODULE_VERSION_PATCH=0
)

# 添加测试
if(BUILD_TESTING)
    add_executable(module_test module_test.cpp)
    target_link_libraries(module_test PRIVATE CHTLModule)
    add_test(NAME module_test COMMAND module_test)
endif()