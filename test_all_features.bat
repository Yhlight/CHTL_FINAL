@echo off
REM CHTL特征测试脚本 (Windows版本)
REM 用于测试CHTL.md中描述的所有特征

echo 🎯 CHTL特征测试脚本
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

REM 创建测试输出目录
if not exist "feature_test_outputs" mkdir feature_test_outputs
echo 📁 创建测试输出目录: feature_test_outputs

REM 运行测试
echo.
echo 🚀 开始运行特征测试...

REM 方法1: 运行Cargo测试
echo.
echo 📝 方法1: 运行Cargo测试
cargo test --test comprehensive_features_test -- --nocapture

REM 方法2: 运行测试运行器
echo.
echo 📝 方法2: 运行测试运行器
cargo run --bin run_feature_tests

REM 方法3: 运行单元测试
echo.
echo 📝 方法3: 运行单元测试
cargo test --lib

REM 方法4: 运行集成测试
echo.
echo 📝 方法4: 运行集成测试
cargo test --test integration_tests

REM 检查测试结果
echo.
echo 📊 检查测试结果...

if exist "feature_test_outputs" (
    echo ✅ 测试输出目录存在
    dir /b feature_test_outputs\*.html 2>nul | find /c /v "" > temp_count.txt
    set /p file_count=<temp_count.txt
    del temp_count.txt
    echo 📁 输出文件数量: %file_count%
    
    if exist "feature_test_outputs\test_report.md" (
        echo ✅ 测试报告已生成
        echo 📄 测试报告: feature_test_outputs\test_report.md
    ) else (
        echo ⚠️  测试报告未生成
    )
) else (
    echo ❌ 测试输出目录不存在
)

REM 显示测试输出文件
echo.
echo 📋 测试输出文件:
if exist "feature_test_outputs" (
    dir /b feature_test_outputs\*.html 2>nul | head -10
)

echo.
echo ✅ 特征测试完成！
echo.
echo 📖 查看测试包使用指南:
echo    type TEST_PACKAGE_GUIDE.md
echo.
echo 📊 查看测试报告:
echo    type feature_test_outputs\test_report.md
echo.
echo 🎉 所有测试已运行完成！
pause