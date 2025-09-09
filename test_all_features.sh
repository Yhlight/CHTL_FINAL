#!/bin/bash

# CHTL特征测试脚本
# 用于测试CHTL.md中描述的所有特征

echo "🎯 CHTL特征测试脚本"
echo "===================="

# 检查Rust是否安装
if ! command -v cargo &> /dev/null; then
    echo "❌ 错误: 未找到Cargo，请先安装Rust"
    echo "   访问: https://rustup.rs/"
    exit 1
fi

echo "✅ 找到Cargo: $(cargo --version)"

# 创建测试输出目录
mkdir -p feature_test_outputs
echo "📁 创建测试输出目录: feature_test_outputs"

# 运行测试
echo ""
echo "🚀 开始运行特征测试..."

# 方法1: 运行Cargo测试
echo ""
echo "📝 方法1: 运行Cargo测试"
cargo test --test comprehensive_features_test -- --nocapture

# 方法2: 运行测试运行器
echo ""
echo "📝 方法2: 运行测试运行器"
cargo run --bin run_feature_tests

# 方法3: 运行单元测试
echo ""
echo "📝 方法3: 运行单元测试"
cargo test --lib

# 方法4: 运行集成测试
echo ""
echo "📝 方法4: 运行集成测试"
cargo test --test integration_tests

# 检查测试结果
echo ""
echo "📊 检查测试结果..."

if [ -d "feature_test_outputs" ]; then
    echo "✅ 测试输出目录存在"
    echo "📁 输出文件数量: $(ls -1 feature_test_outputs/*.html 2>/dev/null | wc -l)"
    
    if [ -f "feature_test_outputs/test_report.md" ]; then
        echo "✅ 测试报告已生成"
        echo "📄 测试报告: feature_test_outputs/test_report.md"
    else
        echo "⚠️  测试报告未生成"
    fi
else
    echo "❌ 测试输出目录不存在"
fi

# 显示测试输出文件
echo ""
echo "📋 测试输出文件:"
if [ -d "feature_test_outputs" ]; then
    ls -la feature_test_outputs/*.html 2>/dev/null | head -10
    if [ $(ls -1 feature_test_outputs/*.html 2>/dev/null | wc -l) -gt 10 ]; then
        echo "... 还有更多文件"
    fi
fi

echo ""
echo "✅ 特征测试完成！"
echo ""
echo "📖 查看测试包使用指南:"
echo "   cat TEST_PACKAGE_GUIDE.md"
echo ""
echo "📊 查看测试报告:"
echo "   cat feature_test_outputs/test_report.md"
echo ""
echo "🎉 所有测试已运行完成！"