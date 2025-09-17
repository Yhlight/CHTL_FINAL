# CHTL - C++ HyperText Language

CHTL是一个基于C++实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。

## 特性

### 核心特性
- **简洁语法**: 使用`{}`代替`<>`，更符合编程习惯
- **字面量支持**: 支持无修饰字面量、双引号、单引号字符串
- **CE对等式**: `:`和`=`完全等价
- **注释系统**: 支持`//`、`/**/`、`#`三种注释类型

### 高级特性
- **局部样式块**: 在元素内部嵌套`style {}`
- **模板系统**: `[Template]`支持样式组、元素、变量组模板
- **自定义系统**: `[Custom]`提供更灵活的扩展
- **原始嵌入**: `[Origin]`支持HTML/CSS/JS代码嵌入
- **导入系统**: `[Import]`支持模块导入
- **命名空间**: `[Namespace]`防止模块污染
- **CHTL JS**: 独立的JavaScript扩展语法

## 项目结构

```
CHTL/
├── src/
│   ├── CHTL/                 # CHTL编译器核心
│   │   ├── CHTLNode/        # AST节点
│   │   ├── Token.h          # 词法单元
│   │   ├── CHTLLexer.h      # 词法分析器
│   │   ├── CHTLParser.h     # 语法分析器
│   │   └── CHTLGenerator.h  # 代码生成器
│   ├── CHTL_JS/             # CHTL JS编译器
│   ├── Scanner/             # 统一扫描器
│   ├── CodeMerger/          # 代码合并器
│   ├── CompilerDispatcher/  # 编译器调度器
│   ├── Util/                # 工具类
│   └── main.cpp             # 主程序
├── test/                    # 测试用例
├── example.chtl            # 示例文件
└── CMakeLists.txt          # 构建配置
```

## 构建

### 依赖
- C++17 或更高版本
- CMake 3.16 或更高版本
- ANTLR4 (可选，用于CSS/JS解析)

### 构建步骤

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make

# 运行测试
make test
```

## 使用方法

### 基本用法

```bash
# 编译CHTL文件
./chtl example.chtl

# 指定输出文件
./chtl example.chtl -o output.html

# 内联CSS和JS
./chtl example.chtl --inline

# 压缩输出
./chtl example.chtl --minify

# 包含默认HTML结构
./chtl example.chtl --default-struct
```

### 命令行选项

- `-o, --output <file>`: 输出文件 (默认: index.html)
- `-c, --css <file>`: CSS输出文件 (默认: style.css)
- `-j, --js <file>`: JS输出文件 (默认: script.js)
- `--inline`: 内联CSS和JS到HTML
- `--inline-css`: 内联CSS到HTML
- `--inline-js`: 内联JS到HTML
- `--minify`: 压缩输出
- `--default-struct`: 包含默认HTML结构
- `-d, --debug`: 启用调试模式
- `-h, --help`: 显示帮助信息
- `-v, --version`: 显示版本信息

## 语法示例

### 基本元素

```chtl
div
{
    id: box;
    class: welcome;
    
    text
    {
        Hello World
    }
}
```

### 局部样式

```chtl
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }
}
```

### 模板

```chtl
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

[Template] @Element Box
{
    div
    {
        style
        {
            .box
            {
                width: 200px;
                height: 200px;
                background-color: red;
            }
        }
    }
}
```

### 自定义

```chtl
[Custom] @Element Card
{
    div
    {
        style
        {
            .card
            {
                width: 300px;
                height: 200px;
                background-color: white;
                border: 1px solid #ccc;
                border-radius: 8px;
            }
        }
    }
}
```

### 导入

```chtl
[Import] @Chtl from "module.chtl"
[Import] @Style from "styles.css"
[Import] @JavaScript from "script.js"
```

### 命名空间

```chtl
[Namespace] space
{
    [Custom] @Element Box
    {
        div
        {
            style
            {
                .box
                {
                    width: 200px;
                    height: 200px;
                }
            }
        }
    }
}
```

## 开发状态

当前实现状态：
- ✅ 词法分析器 (CHTLLexer)
- ✅ 语法分析器 (CHTLParser)
- ✅ 统一扫描器 (UnifiedScanner)
- ✅ 代码生成器 (CHTLGenerator)
- ✅ 基础节点类型
- ✅ 命令行工具
- ✅ 测试框架
- ⏳ CHTL JS编译器
- ⏳ 模块系统 (CMOD/CJMOD)
- ⏳ 高级特性

## 贡献

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目
2. 创建特性分支
3. 提交更改
4. 推送到分支
5. 创建 Pull Request

## 许可证

本项目使用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 创建 Issue
- 发送邮件
- 参与讨论

---

**注意**: 这是一个实验性项目，正在积极开发中。API可能会发生变化。