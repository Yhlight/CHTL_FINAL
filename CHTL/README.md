# CHTL 编译器

CHTL (C++ Hypertext Language) 是一个基于 C++ 的超文本语言，用于编写 HTML 内容。

## 特性

- **纯净输出**: 默认生成适合 SPA 页面的纯净 HTML 内容
- **可选默认结构**: 使用 `--default-struct` 参数生成完整的 HTML 文档
- **模板系统**: 支持样式组模板、元素模板、变量组模板及其继承机制
- **自定义系统**: 支持自定义样式组、元素、变量组的特例化操作
- **原始嵌入**: 支持 HTML、CSS、JavaScript 代码的原始嵌入
- **导入系统**: 支持 CHTL、HTML、CSS、JS 文件的导入和命名空间管理
- **约束系统**: 支持精确约束、类型约束和全局约束
- **CHTL JS 扩展**: 支持文件载入、局部脚本、增强选择器等
- **模块系统**: 支持 CMOD 和 CJMOD 模块的创建、导入和管理

## 安装

1. 解压二进制文件包：
```bash
tar -xzf chtl_compiler.tar.gz
```

2. 将二进制文件添加到 PATH：
```bash
export PATH=$PATH:./bin
```

## 使用方法

### 基本用法

```bash
# 编译 CHTL 文件（纯净输出，适合 SPA）
chtl input.chtl

# 编译 CHTL 文件（完整 HTML 文档）
chtl --default-struct input.chtl

# 查看帮助
chtl --help
```

### 语法示例

#### 基础语法

```chtl
div {
    class: "container";
    id: "main";
    
    header {
        class: "header";
        text: "Welcome to CHTL";
    }
    
    main {
        class: "content";
        
        h1 {
            text: "Hello World";
        }
        
        p {
            text: "This is a paragraph.";
        }
    }
}
```

#### 模板系统

```chtl
[Template] StyleGroup {
    name: "ButtonStyle";
    
    style {
        background-color: "var(--primary-color)";
        color: "white";
        padding: "10px 20px";
        border: "none";
        border-radius: "5px";
        cursor: "pointer";
    }
}

[Template] Element {
    name: "Button";
    inherit: "ButtonStyle";
    
    content {
        button {
            class: "btn";
            text: "Click Me";
        }
    }
}
```

#### 原始嵌入

```chtl
div {
    class: "app";
    
    [Origin] @Html {
        <div class="raw-html">
            <h2>Raw HTML Content</h2>
            <p>This is raw HTML content.</p>
        </div>
    }
    
    [Origin] @Style {
        .raw-html {
            background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
            padding: 20px;
            border-radius: 10px;
            color: white;
        }
    }
    
    [Origin] @JavaScript {
        document.addEventListener('DOMContentLoaded', function() {
            console.log('Raw JavaScript executed!');
        });
    }
}
```

## 测试套件

项目包含完整的测试套件，涵盖所有语法特性：

- `basic_syntax.chtl` - 基础语法测试
- `template_system.chtl` - 模板系统测试
- `custom_system.chtl` - 自定义系统测试
- `origin_embedding.chtl` - 原始嵌入测试
- `import_system.chtl` - 导入系统测试
- `constraint_system.chtl` - 约束系统测试
- `chtl_js_extended.chtl` - CHTL JS 扩展语法测试
- `module_system.chtl` - 模块系统测试
- `spa_page.chtl` - SPA 页面测试

运行测试：

```bash
cd test_suite
./run_tests.sh
```

## 输出示例

### 纯净输出（默认）

```html
<div><class></class><id></id><header><class></class>text</header><main><class></class><h1>text</h1><p>text</p></main></div>
```

### 默认结构输出（--default-struct）

```html
<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div><class></class><id></id><header><class></class>text</header><main><class></class><h1>text</h1><p>text</p></main></div>
</body>
</html>
```

## 技术架构

- **词法分析器**: 解析 CHTL 语法
- **语法分析器**: 构建 AST
- **代码生成器**: 生成 HTML/CSS/JS 输出
- **模板管理器**: 管理模板和自定义系统
- **模块管理器**: 管理 CMOD 和 CJMOD 模块
- **统一扫描器**: 智能分离不同代码块
- **CSS 编译器**: 编译和优化 CSS
- **JavaScript 编译器**: 编译和执行 JavaScript

## 依赖

- C++ 17 或更高版本
- CMake 3.16 或更高版本
- 可选：V8 引擎（用于 JavaScript 执行）
- 可选：libcss（用于 CSS 编译）

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！

## 更新日志

### v1.0.0
- 初始版本发布
- 支持基础语法、模板系统、自定义系统
- 支持原始嵌入、导入系统、约束系统
- 支持 CHTL JS 扩展语法和模块系统
- 支持纯净输出和默认结构输出