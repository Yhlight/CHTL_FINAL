# CHTL - 超文本语言

CHTL 是一个基于 C++ 实现的超文本语言，旨在提供更符合开发者编写 HTML 代码的方式。

## 特性

- **简洁的语法**: 使用花括号和缩进，类似 C++ 的语法风格
- **模板系统**: 支持样式组模板、元素模板、变量组模板
- **自定义系统**: 支持自定义样式组、元素、变量组，具有特例化功能
- **局部样式块**: 在元素内部嵌套样式，支持自动化类名/ID 添加
- **属性条件表达式**: 支持基于元素属性的条件样式
- **命名空间**: 防止模块污染
- **导入系统**: 支持文件导入和模块管理
- **CHTL JS 扩展**: 独立的编程语言，最终转换为 JavaScript

## 项目结构

```
CHTL/
├── include/CHTL/          # 头文件
│   ├── CHTLCommon.h       # 通用定义
│   ├── CHTLNode.h         # AST 节点定义
│   ├── CHTLVisitor.h      # 访问者模式
│   ├── CHTLLexer.h        # 词法分析器
│   ├── CHTLParser.h       # 语法分析器
│   ├── CHTLGenerator.h    # 代码生成器
│   └── CHTLContext.h      # 编译上下文
├── src/CHTL/              # 源文件
│   ├── CHTLNode.cpp
│   ├── CHTLLexer.cpp
│   ├── CHTLParser.cpp
│   ├── CHTLGenerator.cpp
│   ├── CHTLContext.cpp
│   └── CHTLVisitor.cpp
├── src/main.cpp           # 主程序
├── CMakeLists.txt         # CMake 配置
├── build.sh              # 构建脚本
└── test.chtl             # 测试文件
```

## 构建

### 依赖

- C++17 编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- Boost 库

### 构建步骤

1. 克隆项目
```bash
git clone <repository-url>
cd CHTL
```

2. 安装依赖
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libboost-all-dev

# CentOS/RHEL
sudo yum install gcc-c++ cmake boost-devel

# macOS
brew install cmake boost
```

3. 构建项目
```bash
./build.sh
```

## 使用方法

### 基本语法

```chtl
html
{
    head
    {
        title
        {
            text
            {
                My Page
            }
        }
    }
    
    body
    {
        div
        {
            class: container;
            id: main-content;
            
            h1
            {
                text
                {
                    Welcome to CHTL!
                }
            }
            
            p
            {
                text
            {
                This is a paragraph.
            }
        }
    }
}
```

### 局部样式块

```chtl
div
{
    style
    {
        .container
        {
            width: 100%;
            max-width: 800px;
            margin: 0 auto;
        }
        
        #main-content
        {
            background-color: #f5f5f5;
            padding: 20px;
        }
    }
}
```

### 模板系统

```chtl
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
    line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;
    }
}
```

### 自定义系统

```chtl
[Custom] @Element Button
{
    button
    {
        class: btn;
        text
        {
            Click me
        }
    }
}

body
{
    @Element Button;
}
```

## 命令行工具

```bash
# 编译 CHTL 文件为 HTML
./build/bin/chtl input.chtl output.html

# 查看帮助
./build/bin/chtl --help
```

## 开发

### 添加新功能

1. 在 `include/CHTL/` 中添加头文件
2. 在 `src/CHTL/` 中实现功能
3. 更新 `CMakeLists.txt`
4. 添加测试用例

### 测试

项目包含基本的测试用例，运行 `./build.sh` 会自动执行测试。

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！

## 路线图

- [x] 基础语法解析
- [x] HTML 生成
- [x] 模板系统
- [x] 自定义系统
- [ ] CSS 生成器
- [ ] JavaScript 生成器
- [ ] CHTL JS 编译器
- [ ] 模块系统
- [ ] VSCode 扩展
- [ ] 文档网站