#!/bin/bash

# CHTL编译测试脚本

echo "🔧 CHTL编译测试脚本"
echo "===================="

# 检查Rust是否安装
if ! command -v cargo &> /dev/null; then
    echo "❌ 错误: 未找到Cargo，请先安装Rust"
    echo "   访问: https://rustup.rs/"
    exit 1
fi

echo "✅ 找到Cargo: $(cargo --version)"

# 清理之前的构建
echo ""
echo "🧹 清理之前的构建..."
cargo clean

# 检查编译
echo ""
echo "🔍 检查编译状态..."
cargo check

if [ $? -eq 0 ]; then
    echo "✅ 编译检查通过"
else
    echo "❌ 编译检查失败"
    echo ""
    echo "🔧 尝试修复常见问题..."
    
    # 检查依赖
    echo "📦 检查依赖..."
    cargo tree
    
    # 尝试更新依赖
    echo "🔄 更新依赖..."
    cargo update
    
    # 再次检查
    echo "🔍 重新检查编译..."
    cargo check
fi

# 尝试构建
echo ""
echo "🏗️ 尝试构建项目..."
cargo build

if [ $? -eq 0 ]; then
    echo "✅ 构建成功"
else
    echo "❌ 构建失败"
    echo ""
    echo "📋 常见修复方法:"
    echo "1. 检查Rust版本: rustc --version"
    echo "2. 更新Rust: rustup update"
    echo "3. 清理构建: cargo clean"
    echo "4. 检查依赖: cargo tree"
    echo "5. 查看详细错误: cargo build --verbose"
fi

# 尝试运行简单测试
echo ""
echo "🧪 尝试运行简单测试..."
cargo run --bin simple_test

if [ $? -eq 0 ]; then
    echo "✅ 简单测试运行成功"
    
    # 检查测试输出
    if [ -d "simple_test_outputs" ]; then
        echo "📁 测试输出目录存在"
        echo "📄 输出文件数量: $(ls -1 simple_test_outputs/*.html 2>/dev/null | wc -l)"
        
        # 显示输出文件
        echo "📋 输出文件:"
        ls -la simple_test_outputs/*.html 2>/dev/null | head -5
    else
        echo "⚠️ 测试输出目录不存在"
    fi
else
    echo "❌ 简单测试运行失败"
fi

# 尝试运行单元测试
echo ""
echo "🧪 尝试运行单元测试..."
cargo test --lib

if [ $? -eq 0 ]; then
    echo "✅ 单元测试通过"
else
    echo "❌ 单元测试失败"
fi

echo ""
echo "📊 编译测试完成！"
echo ""
echo "📖 查看修复指南:"
echo "   cat COMPILATION_FIXES.md"
echo ""
echo "🔧 如果仍有问题，请检查具体的错误信息"