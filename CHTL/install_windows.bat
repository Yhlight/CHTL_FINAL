@echo off
REM CHTL Windows 安装脚本

echo === CHTL Windows 安装程序 ===
echo.

REM 检查是否存在二进制文件
if not exist "build\bin\chtl.exe" (
    echo 错误: 未找到 CHTL 编译器二进制文件
    echo 请先编译项目: make -j4
    pause
    exit /b 1
)

REM 创建安装目录
set "INSTALL_DIR=%USERPROFILE%\CHTL"
echo 安装目录: %INSTALL_DIR%

if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
    echo 创建安装目录: %INSTALL_DIR%
)

REM 复制二进制文件
echo 复制二进制文件...
copy "build\bin\chtl.exe" "%INSTALL_DIR%\"
copy "build\lib\*.a" "%INSTALL_DIR%\lib\"
if not exist "%INSTALL_DIR%\lib" mkdir "%INSTALL_DIR%\lib"

REM 复制测试套件
echo 复制测试套件...
xcopy "test_suite\*" "%INSTALL_DIR%\test_suite\" /E /I

REM 复制文档
echo 复制文档...
copy "README.md" "%INSTALL_DIR%\"
copy "PROJECT_SUMMARY.md" "%INSTALL_DIR%\"

REM 创建启动脚本
echo 创建启动脚本...
echo @echo off > "%INSTALL_DIR%\chtl.bat"
echo "%INSTALL_DIR%\chtl.exe" %%* >> "%INSTALL_DIR%\chtl.bat"

REM 创建测试脚本
echo 创建测试脚本...
echo @echo off > "%INSTALL_DIR%\run_tests.bat"
echo cd /d "%INSTALL_DIR%\test_suite" >> "%INSTALL_DIR%\run_tests.bat"
echo run_tests.bat >> "%INSTALL_DIR%\run_tests.bat"

REM 添加到 PATH
echo 添加到 PATH...
setx PATH "%PATH%;%INSTALL_DIR%" /M
if %errorlevel% neq 0 (
    echo 警告: 无法自动添加到系统 PATH
    echo 请手动添加 %INSTALL_DIR% 到系统 PATH
)

echo.
echo === 安装完成 ===
echo CHTL 已安装到: %INSTALL_DIR%
echo.
echo 使用方法:
echo   chtl input.chtl                    # 纯净输出
echo   chtl --default-struct input.chtl  # 完整 HTML 文档
echo   chtl --help                        # 查看帮助
echo.
echo 运行测试:
echo   run_tests.bat
echo.
echo 测试文件位置: %INSTALL_DIR%\test_suite\
echo.

REM 运行测试
set /p "run_tests=是否现在运行测试? (y/n): "
if /i "%run_tests%"=="y" (
    echo.
    echo 运行测试套件...
    cd /d "%INSTALL_DIR%\test_suite"
    run_tests.bat
)

echo.
echo 安装完成！按任意键退出...
pause > nul