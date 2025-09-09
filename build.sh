#!/bin/bash

# CHTL 编译器构建脚本
# 支持多种编译选项和外部依赖

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_message() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

# 检查依赖
check_dependencies() {
    print_message $BLUE "检查构建依赖..."
    
    # 检查 CMake
    if ! command -v cmake &> /dev/null; then
        print_message $RED "错误: CMake 未安装"
        exit 1
    fi
    
    # 检查编译器
    if ! command -v g++ &> /dev/null; then
        print_message $RED "错误: g++ 编译器未安装"
        exit 1
    fi
    
    # 检查 pkg-config
    if ! command -v pkg-config &> /dev/null; then
        print_message $RED "错误: pkg-config 未安装"
        exit 1
    fi
    
    print_message $GREEN "依赖检查通过"
}

# 安装外部依赖
install_dependencies() {
    print_message $BLUE "安装外部依赖..."
    
    # 检查操作系统
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Ubuntu/Debian
        if command -v apt-get &> /dev/null; then
            print_message $YELLOW "检测到 Ubuntu/Debian 系统"
            sudo apt-get update
            sudo apt-get install -y \
                cmake \
                build-essential \
                pkg-config \
                libantlr4-runtime-dev \
                libsass-dev \
                libv8-dev \
                nlohmann-json3-dev
        # CentOS/RHEL
        elif command -v yum &> /dev/null; then
            print_message $YELLOW "检测到 CentOS/RHEL 系统"
            sudo yum install -y \
                cmake \
                gcc-c++ \
                pkgconfig \
                antlr4-runtime-devel \
                libsass-devel \
                v8-devel \
                nlohmann-json3-devel
        # Arch Linux
        elif command -v pacman &> /dev/null; then
            print_message $YELLOW "检测到 Arch Linux 系统"
            sudo pacman -S --noconfirm \
                cmake \
                gcc \
                pkg-config \
                antlr4-runtime \
                libsass \
                v8 \
                nlohmann-json
        else
            print_message $YELLOW "未识别的 Linux 发行版，请手动安装依赖"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        print_message $YELLOW "检测到 macOS 系统"
        if command -v brew &> /dev/null; then
            brew install cmake pkg-config antlr4-runtime libsass v8 nlohmann-json
        else
            print_message $RED "错误: 请先安装 Homebrew"
            exit 1
        fi
    else
        print_message $YELLOW "未识别的操作系统，请手动安装依赖"
    fi
    
    print_message $GREEN "依赖安装完成"
}

# 创建构建目录
create_build_dir() {
    print_message $BLUE "创建构建目录..."
    
    if [ -d "build" ]; then
        print_message $YELLOW "清理现有构建目录..."
        rm -rf build
    fi
    
    mkdir -p build
    cd build
    print_message $GREEN "构建目录创建完成"
}

# 配置 CMake
configure_cmake() {
    print_message $BLUE "配置 CMake..."
    
    # 检查是否安装了所有依赖
    local missing_deps=()
    
    if ! pkg-config --exists libsass; then
        missing_deps+=("libsass")
    fi
    
    if ! pkg-config --exists v8; then
        missing_deps+=("v8")
    fi
    
    if [ ${#missing_deps[@]} -gt 0 ]; then
        print_message $YELLOW "警告: 以下依赖未找到: ${missing_deps[*]}"
        print_message $YELLOW "将使用简化版本编译"
    fi
    
    # 配置 CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_STANDARD=17 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    
    print_message $GREEN "CMake 配置完成"
}

# 编译项目
build_project() {
    print_message $BLUE "开始编译项目..."
    
    # 获取 CPU 核心数
    local cores=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    # 编译
    make -j$cores
    
    print_message $GREEN "编译完成"
}

# 运行测试
run_tests() {
    print_message $BLUE "运行测试..."
    
    if [ -f "test/CHTL_Compiler_test" ]; then
        ./test/CHTL_Compiler_test
        print_message $GREEN "测试通过"
    else
        print_message $YELLOW "测试文件未找到，跳过测试"
    fi
}

# 创建测试包
create_test_package() {
    print_message $BLUE "创建测试包..."
    
    local test_dir="../test-package"
    
    # 创建测试包目录
    if [ -d "$test_dir" ]; then
        rm -rf "$test_dir"
    fi
    mkdir -p "$test_dir"
    
    # 复制二进制文件
    cp bin/CHTL_Compiler "$test_dir/"
    
    # 复制测试文件
    cp -r ../test "$test_dir/"
    
    # 创建测试脚本
    cat > "$test_dir/test-runner.sh" << 'EOF'
#!/bin/bash

# CHTL 测试运行器

echo "CHTL 编译器测试包"
echo "=================="

# 检查二进制文件
if [ ! -f "CHTL_Compiler" ]; then
    echo "错误: CHTL_Compiler 二进制文件未找到"
    exit 1
fi

# 设置执行权限
chmod +x CHTL_Compiler

echo "测试 1: 基础语法测试"
./CHTL_Compiler compile test/simple.chtl -o test/simple.html
if [ $? -eq 0 ]; then
    echo "✓ 基础语法测试通过"
else
    echo "✗ 基础语法测试失败"
fi

echo "测试 2: 模板系统测试"
./CHTL_Compiler compile test/template.chtl -o test/template.html
if [ $? -eq 0 ]; then
    echo "✓ 模板系统测试通过"
else
    echo "✗ 模板系统测试失败"
fi

echo "测试 3: 自定义系统测试"
./CHTL_Compiler compile test/custom.chtl -o test/custom.html
if [ $? -eq 0 ]; then
    echo "✓ 自定义系统测试通过"
else
    echo "✗ 自定义系统测试失败"
fi

echo "测试 4: 原始嵌入测试"
./CHTL_Compiler compile test/origin.chtl -o test/origin.html
if [ $? -eq 0 ]; then
    echo "✓ 原始嵌入测试通过"
else
    echo "✗ 原始嵌入测试失败"
fi

echo "测试 5: 导入系统测试"
./CHTL_Compiler compile test/import.chtl -o test/import.html
if [ $? -eq 0 ]; then
    echo "✓ 导入系统测试通过"
else
    echo "✗ 导入系统测试失败"
fi

echo "测试 6: 约束系统测试"
./CHTL_Compiler compile test/constraints.chtl -o test/constraints.html
if [ $? -eq 0 ]; then
    echo "✓ 约束系统测试通过"
else
    echo "✗ 约束系统测试失败"
fi

echo "测试 7: 配置组测试"
./CHTL_Compiler compile test/config.chtl -o test/config.html
if [ $? -eq 0 ]; then
    echo "✓ 配置组测试通过"
else
    echo "✗ 配置组测试失败"
fi

echo "测试 8: CHTL JS 测试"
./CHTL_Compiler compile test/chtljs.chtl -o test/chtljs.html
if [ $? -eq 0 ]; then
    echo "✓ CHTL JS 测试通过"
else
    echo "✗ CHTL JS 测试失败"
fi

echo "测试 9: 模块系统测试"
./CHTL_Compiler compile test/module.cmod -o test/module.html
if [ $? -eq 0 ]; then
    echo "✓ 模块系统测试通过"
else
    echo "✗ 模块系统测试失败"
fi

echo "测试 10: CJMOD API 测试"
./CHTL_Compiler compile test/cjmodapi.chtl -o test/cjmodapi.html
if [ $? -eq 0 ]; then
    echo "✓ CJMOD API 测试通过"
else
    echo "✗ CJMOD API 测试失败"
fi

echo "测试 11: VSCode 扩展测试"
./CHTL_Compiler compile test/vscode-extension.chtl -o test/vscode-extension.html
if [ $? -eq 0 ]; then
    echo "✓ VSCode 扩展测试通过"
else
    echo "✗ VSCode 扩展测试失败"
fi

echo "测试 12: SPA 示例测试"
./CHTL_Compiler compile test/spa-example.chtl -o test/spa-example.html
if [ $? -eq 0 ]; then
    echo "✓ SPA 示例测试通过"
else
    echo "✗ SPA 示例测试失败"
fi

echo "测试 13: 传统网页测试"
./CHTL_Compiler compile test/traditional-page.chtl -o test/traditional-page.html --default-struct
if [ $? -eq 0 ]; then
    echo "✓ 传统网页测试通过"
else
    echo "✗ 传统网页测试失败"
fi

echo "测试 14: 综合语法测试"
./CHTL_Compiler compile test/comprehensive-syntax.chtl -o test/comprehensive-syntax.html
if [ $? -eq 0 ]; then
    echo "✓ 综合语法测试通过"
else
    echo "✗ 综合语法测试失败"
fi

echo ""
echo "测试完成！"
echo "生成的 HTML 文件位于 test/ 目录中"
echo "您可以在浏览器中打开这些文件查看结果"
EOF
    
    chmod +x "$test_dir/test-runner.sh"
    
    # 创建 README
    cat > "$test_dir/README.md" << 'EOF'
# CHTL 编译器测试包

这是 CHTL 编译器的完整测试包，包含了所有语法特性的测试文件。

## 文件结构

- `CHTL_Compiler` - 主编译器二进制文件
- `test/` - 测试文件目录
- `test-runner.sh` - 测试运行脚本
- `README.md` - 本文件

## 使用方法

1. 运行测试脚本：
   ```bash
   ./test-runner.sh
   ```

2. 手动编译单个文件：
   ```bash
   ./CHTL_Compiler compile test/simple.chtl -o test/simple.html
   ```

3. 使用默认结构编译：
   ```bash
   ./CHTL_Compiler compile test/simple.chtl -o test/simple.html --default-struct
   ```

## 测试文件说明

- `simple.chtl` - 基础语法测试
- `template.chtl` - 模板系统测试
- `custom.chtl` - 自定义系统测试
- `origin.chtl` - 原始嵌入测试
- `import.chtl` - 导入系统测试
- `constraints.chtl` - 约束系统测试
- `config.chtl` - 配置组测试
- `chtljs.chtl` - CHTL JS 扩展语法测试
- `module.cmod` - 模块系统测试
- `cjmodapi.chtl` - CJMOD API 测试
- `vscode-extension.chtl` - VSCode 扩展测试
- `spa-example.chtl` - SPA 示例测试
- `traditional-page.chtl` - 传统网页测试
- `comprehensive-syntax.chtl` - 综合语法测试

## 外部依赖文件

- `external-styles.css` - 外部 CSS 文件
- `external-script.js` - 外部 JavaScript 文件
- `components.chtl` - 组件库文件
- `utils.chtl` - 工具函数库文件

## 功能特性

CHTL 编译器支持以下功能：

- 完整的 CHTL 语法解析
- 模板系统（样式组、元素、变量组模板）
- 自定义系统（样式组、元素、变量组自定义）
- 原始嵌入（HTML、CSS、JavaScript 原始代码嵌入）
- 导入系统（文件导入、组件导入）
- 约束系统（元素约束、样式约束）
- 配置组系统（自定义配置、关键字重命名）
- CHTL JS 扩展语法（增强选择器、事件监听、动画、虚对象、路由）
- 模块系统（CMOD、CJMOD）
- CJMOD API（语法分析、参数绑定、代码生成）
- 统一扫描器（代码切割、占位符机制）
- 命令行工具（编译、验证、格式化、模块管理）
- 测试系统（单元测试、集成测试、功能测试）
- VSCode IDE 支持（语法高亮、代码提示、预览）
- 外部编译器集成（ANTLR4、libsass、V8）

## 编译选项

- `--default-struct` - 生成带有默认 HTML 结构的完整页面
- `-o <output>` - 指定输出文件
- `-h, --help` - 显示帮助信息
- `-v, --version` - 显示版本信息

## 注意事项

1. 默认情况下，CHTL 编译器生成纯净的内容，不包含默认的 HTML 结构
2. 使用 `--default-struct` 选项可以生成完整的 HTML 页面
3. 生成的 HTML 文件可以直接在浏览器中打开查看
4. 所有测试文件都包含了详细的注释和说明

## 技术支持

如果您遇到问题，请查看：
1. 编译器错误信息
2. 测试文件中的注释
3. 项目文档

祝您使用愉快！
EOF
    
    print_message $GREEN "测试包创建完成: $test_dir"
}

# 主函数
main() {
    print_message $GREEN "CHTL 编译器构建脚本"
    print_message $GREEN "===================="
    
    # 解析命令行参数
    INSTALL_DEPS=false
    SKIP_TESTS=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --install-deps)
                INSTALL_DEPS=true
                shift
                ;;
            --skip-tests)
                SKIP_TESTS=true
                shift
                ;;
            -h|--help)
                echo "用法: $0 [选项]"
                echo "选项:"
                echo "  --install-deps    安装外部依赖"
                echo "  --skip-tests      跳过测试"
                echo "  -h, --help        显示帮助信息"
                exit 0
                ;;
            *)
                print_message $RED "未知选项: $1"
                exit 1
                ;;
        esac
    done
    
    # 执行构建步骤
    check_dependencies
    
    if [ "$INSTALL_DEPS" = true ]; then
        install_dependencies
    fi
    
    create_build_dir
    configure_cmake
    build_project
    
    if [ "$SKIP_TESTS" = false ]; then
        run_tests
    fi
    
    create_test_package
    
    print_message $GREEN "构建完成！"
    print_message $GREEN "测试包位置: test-package/"
    print_message $GREEN "二进制文件: test-package/CHTL_Compiler"
}

# 运行主函数
main "$@"