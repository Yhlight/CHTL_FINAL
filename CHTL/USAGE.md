# CHTL 使用指南

## 快速开始

### 1. 编译项目

```bash
mkdir build
cd build
cmake ..
make
```

### 2. 运行测试

```bash
# 词法分析器测试
./test_simple

# 语法分析器测试
./test_parser

# 完整演示
./demo
```

### 3. 编译 CHTL 文件

```bash
./chtl example.chtl -o output.html
```

## CHTL 语法示例

### 基本 HTML 结构

```chtl
html
{
    head
    {
        title
        {
            text
            {
                "页面标题"
            }
        }
    }
    
    body
    {
        h1
        {
            text
            {
                "欢迎使用 CHTL"
            }
        }
    }
}
```

### 带属性的元素

```chtl
div
{
    class: "container";
    id: "main-content";
    
    p
    {
        text
        {
            "这是一个段落"
        }
    }
}
```

### 样式块

```chtl
div
{
    style
    {
        width: 100%;
        height: 200px;
        background-color: #f0f0f0;
        margin: 10px;
        padding: 20px;
    }
    
    text
    {
        "带样式的元素"
    }
}
```

### 脚本块

```chtl
button
{
    text
    {
        "点击我"
    }
    
    script
    {
        console.log("按钮被点击了");
    }
}
```

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL 编译器核心
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLNode/           # AST 节点
│   ├── CHTLParser/         # 语法分析器
│   └── CHTLGenerator/      # 代码生成器
├── build/                  # 构建目录
├── test.chtl              # 测试文件
├── example.chtl           # 示例文件
└── README.md              # 项目说明
```

## 开发状态

当前项目处于开发阶段，已实现：

- ✅ 完整的词法分析器
- ✅ 基础语法分析器
- ✅ AST 节点系统
- ✅ 代码生成框架

正在开发：

- 🔄 完整语法分析器
- 🔄 模板系统
- 🔄 自定义系统
- 🔄 CHTL JS 编译器

## 贡献

欢迎贡献代码！请查看 `PROJECT_STATUS.md` 了解当前开发状态和待实现功能。

## 许可证

MIT License