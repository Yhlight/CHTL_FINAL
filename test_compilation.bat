@echo off
REM CHTL编译测试脚本 (Windows版本)

echo 🔧 CHTL编译测试脚本
echo ====================

REM 检查Cargo是否安装
cargo --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ 错误: 未找到Cargo，请先安装Rust
    echo    访问: https://rustup.rs/
    pause
    exit /b 1
)

echo ✅ 找到Cargo: 
cargo --version

REM 清理之前的构建
echo.
echo 🧹 清理之前的构建...
cargo clean

REM 检查编译
echo.
echo 🔍 检查编译状态...
cargo check

if %errorlevel% equ 0 (
    echo ✅ 编译检查通过
) else (
    echo ❌ 编译检查失败
    echo.
    echo 🔧 尝试修复常见问题...
    
    REM 检查依赖
    echo 📦 检查依赖...
    cargo tree
    
    REM 尝试更新依赖
    echo 🔄 更新依赖...
    cargo update
    
    REM 再次检查
    echo 🔍 重新检查编译...
    cargo check
)

REM 尝试构建
echo.
echo 🏗️ 尝试构建项目...
cargo build

if %errorlevel% equ 0 (
    echo ✅ 构建成功
) else (
    echo ❌ 构建失败
    echo.
    echo 📋 常见修复方法:
    echo 1. 检查Rust版本: rustc --version
    echo 2. 更新Rust: rustup update
    echo 3. 清理构建: cargo clean
    echo 4. 检查依赖: cargo tree
    echo 5. 查看详细错误: cargo build --verbose
)

REM 尝试运行简单测试
echo.
echo 🧪 尝试运行简单测试...
cargo run --bin simple_test

if %errorlevel% equ 0 (
    echo ✅ 简单测试运行成功
    
    REM 检查测试输出
    if exist "simple_test_outputs" (
        echo 📁 测试输出目录存在
        dir /b simple_test_outputs\*.html 2>nul | find /c /v "" > temp_count.txt
        set /p file_count=<temp_count.txt
        del temp_count.txt
        echo 📄 输出文件数量: %file_count%
        
        REM 显示输出文件
        echo 📋 输出文件:
        dir /b simple_test_outputs\*.html 2>nul | head -5
    ) else (
        echo ⚠️ 测试输出目录不存在
    )
) else (
    echo ❌ 简单测试运行失败
)

REM 尝试运行单元测试
echo.
echo 🧪 尝试运行单元测试...
cargo test --lib

if %errorlevel% equ 0 (
    echo ✅ 单元测试通过
) else (
    echo ❌ 单元测试失败
)

echo.
echo 📊 编译测试完成！
echo.
echo 📖 查看修复指南:
echo    type COMPILATION_FIXES.md
echo.
echo 🔧 如果仍有问题，请检查具体的错误信息
pause