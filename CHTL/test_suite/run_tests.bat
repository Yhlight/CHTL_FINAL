@echo off
REM CHTL 测试套件运行脚本 (Windows 版本)
REM 测试所有 CHTL 语法特性和功能

echo === CHTL 测试套件 ===
echo 开始运行所有测试...

REM 设置颜色
set "GREEN=[92m"
set "RED=[91m"
set "YELLOW=[93m"
set "NC=[0m"

REM 测试计数器
set /a TOTAL_TESTS=0
set /a PASSED_TESTS=0
set /a FAILED_TESTS=0

REM 测试函数
:run_test
set "test_name=%~1"
set "input_file=%~2"
set "expected_output=%~3"

echo.
echo %YELLOW%运行测试: %test_name%%NC%
echo 输入文件: %input_file%

set /a TOTAL_TESTS+=1

REM 运行 CHTL 编译器
..\build\bin\chtl.exe "%input_file%" > "output_%test_name%.html" 2>&1
if %errorlevel% equ 0 (
    echo %GREEN%✓ 编译成功%NC%
    set /a PASSED_TESTS+=1
    
    REM 显示输出内容
    echo 输出内容:
    type "output_%test_name%.html"
    echo.
) else (
    echo %RED%✗ 编译失败%NC%
    set /a FAILED_TESTS+=1
    
    REM 显示错误信息
    echo 错误信息:
    type "output_%test_name%.html"
    echo.
)
goto :eof

REM 运行所有测试
echo 开始运行基础语法测试...
call :run_test "basic_syntax" "basic_syntax.chtl" ""

echo 开始运行模板系统测试...
call :run_test "template_system" "template_system.chtl" ""

echo 开始运行自定义系统测试...
call :run_test "custom_system" "custom_system.chtl" ""

echo 开始运行原始嵌入系统测试...
call :run_test "origin_embedding" "origin_embedding.chtl" ""

echo 开始运行导入系统测试...
call :run_test "import_system" "import_system.chtl" ""

echo 开始运行约束系统测试...
call :run_test "constraint_system" "constraint_system.chtl" ""

echo 开始运行配置组系统测试...
call :run_test "config_system" "config_system.chtl" ""

echo 开始运行命名空间系统测试...
call :run_test "namespace_system" "namespace_system.chtl" ""

echo 开始运行 CHTL JS 扩展语法测试...
call :run_test "chtl_js_extended" "chtl_js_extended.chtl" ""

echo 开始运行模块系统测试...
call :run_test "module_system" "module_system.chtl" ""

echo 开始运行 SPA 页面测试...
call :run_test "spa_page" "spa_page.chtl" ""

echo 开始运行完整功能测试...
call :run_test "complete_features" "complete_features.chtl" ""

REM 测试默认结构输出
echo.
echo %YELLOW%运行默认结构测试: spa_page_default_struct%NC%
echo 输入文件: spa_page.chtl

set /a TOTAL_TESTS+=1

..\build\bin\chtl.exe --default-struct "spa_page.chtl" > "output_spa_page_default_struct.html" 2>&1
if %errorlevel% equ 0 (
    echo %GREEN%✓ 默认结构编译成功%NC%
    set /a PASSED_TESTS+=1
    
    REM 显示输出内容
    echo 输出内容:
    type "output_spa_page_default_struct.html"
    echo.
) else (
    echo %RED%✗ 默认结构编译失败%NC%
    set /a FAILED_TESTS+=1
    
    REM 显示错误信息
    echo 错误信息:
    type "output_spa_page_default_struct.html"
    echo.
)

REM 显示测试结果
echo.
echo === 测试结果 ===
echo 总测试数: %TOTAL_TESTS%
echo %GREEN%通过: %PASSED_TESTS%%NC%
echo %RED%失败: %FAILED_TESTS%%NC%

if %FAILED_TESTS% equ 0 (
    echo.
    echo %GREEN%🎉 所有测试通过！%NC%
    exit /b 0
) else (
    echo.
    echo %RED%❌ 有 %FAILED_TESTS% 个测试失败%NC%
    exit /b 1
)