# CHTL - C++ Hypertext Language

CHTL是一个基于C++的现代超文本语言，旨在简化Web开发并提供强大的模板系统、模块化支持和JavaScript扩展功能。

## 特性

### 核心特性
- **HTML-like语法**: 直观的HTML结构，支持所有HTML元素
- **属性语法**: `attribute: value;` 格式的属性定义
- **文本节点**: `text {}` 节点用于文本内容
- **字面量支持**: 支持字符串、数字和未修饰字符串
- **注释系统**: 支持 `//`、`/**/` 和 `#` 注释

### 局部样式块
- **内联样式**: `style {}` 块用于CSS样式
- **自动化类名/ID**: 自动生成唯一的类名和ID
- **上下文推导**: 使用 `&` 符号进行上下文推导
- **属性算术**: 支持CSS属性的算术运算
- **引用属性**: 引用其他属性的值
- **条件表达式**: 支持属性条件表达式

### 模板系统
- **Template模板**: `[Template]` 用于可重用组件
- **Custom自定义**: `[Custom]` 用于样式定制
- **继承支持**: 模板支持继承和参数化
- **约束系统**: 支持模板约束和条件

### CHTL JS
- **声明式语法**: 简化的JavaScript语法
- **脚本加载器**: `ScriptLoader {}` 用于模块加载
- **局部脚本块**: `script {}` 用于内联JavaScript
- **增强选择器**: `{{CSS选择器}}` 语法
- **增强监听器**: `Listen {}` 用于事件处理
- **事件绑定**: `&->` 操作符用于事件绑定
- **事件委托**: `Delegate {}` 用于事件委托
- **动画系统**: `Animate {}` 用于动画定义
- **虚对象**: `Vir` 用于虚拟对象
- **路由系统**: `Router {}` 用于页面路由
- **工具函数**: `util` 用于工具函数定义

### 模块系统
- **CMOD**: CHTL模块，支持 `[Info]` 和 `[Export]` 块
- **CJMOD**: CHTL JS模块，扩展CHTL JS语法
- **混合模块**: 同时支持CHTL和CHTL JS
- **模块导入**: 支持多种文件类型的导入
- **官方模块**: 支持 `chtl::` 前缀的官方模块

### 其他特性
- **命名空间**: `[Namespace]` 用于模块污染预防
- **导入系统**: `[Import]` 用于文件导入
- **配置系统**: `[Configuration]` 用于编译器配置
- **原始嵌入**: `[Origin]` 用于嵌入原始HTML/CSS/JS
- **约束系统**: `except` 关键字用于作用域约束

## 安装

### 依赖
- C++17 或更高版本
- CMake 3.10 或更高版本
- Google Test (可选，用于测试)

### 编译
```bash
mkdir build
cd build
cmake ..
make
```

### 安装
```bash
sudo make install
```

## 使用方法

### 基本用法
```bash
chtl compile input.chtl -o output.html
```

### 高级用法
```bash
chtl compile input.chtl -o output.html --minify --source-map --watch
```

### 项目模式
```bash
chtl init my-project
cd my-project
chtl build
chtl serve
```

## 语法示例

### 基本HTML结构
```chtl
html {
    head {
        title { "My CHTL Page" }
        link {
            rel: "stylesheet";
            href: "style.css";
        }
    }
    body {
        div {
            id: mainContent;
            class: container;
            text { "Hello, CHTL!" }
        }
    }
}
```

### 局部样式块
```chtl
div {
    class: button;
    text { "Click me" }
    style {
        background-color: #007bff;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        
        &:hover {
            background-color: #0056b3;
        }
    }
}
```

### 模板系统
```chtl
[Template] @Style button {
    background-color: {{color}};
    color: {{textColor}};
    padding: {{padding}};
}

[Custom] @Style button {
    delete background-color;
    insert border: 2px solid {{borderColor}};
}
```

### CHTL JS
```chtl
script {
    Vir {
        name: "appState";
        properties: {
            currentUser: null;
            isLoggedIn: false;
        };
    }
    
    Listen {
        event: "click";
        handler: function(e) {
            console.log("Button clicked!");
        };
    }
    
    Animate {
        name: "fadeIn";
        properties: {
            duration: 1000;
            easing: "ease-in-out";
        };
    }
}
```

### 模块系统
```chtl
[Import] @Chtl {
    from: "components/header.chtl";
    as: "Header";
}

[Import] @Css {
    from: "styles/main.css";
}

[Import] @JavaScript {
    from: "scripts/main.js";
}
```

### 命名空间
```chtl
[Namespace] MyApp {
    html {
        head {
            title { "{{pageTitle}} - My App" }
        }
        body {
            Header {}
            div {
                class: container;
                text { "{{content}}" }
            }
        }
    }
}
```

## 项目结构

```
chtl/
├── src/
│   ├── CHTL/                 # CHTL核心组件
│   │   ├── CHTLLexer.h/cpp  # 词法分析器
│   │   ├── CHTLParser.h/cpp # 语法分析器
│   │   ├── CHTLGenerator.h/cpp # 代码生成器
│   │   ├── TemplateSystem.h/cpp # 模板系统
│   │   ├── ModuleSystem.h/cpp # 模块系统
│   │   └── CHTLNode/        # AST节点
│   ├── CHTL_JS/             # CHTL JS组件
│   │   ├── CHTLJSCompiler.h/cpp # CHTL JS编译器
│   │   └── CHTLJSContext.h/cpp # CHTL JS上下文
│   ├── Scanner/              # 统一扫描器
│   │   └── UnifiedScanner.h/cpp
│   ├── CLI/                  # 命令行工具
│   │   └── CHTLCli.h/cpp
│   └── Util/                 # 工具函数
├── test/                     # 测试文件
├── examples/                 # 示例文件
├── docs/                     # 文档
└── CMakeLists.txt           # CMake配置
```

## 开发

### 运行测试
```bash
cd build
make test
```

### 代码格式化
```bash
clang-format -i src/**/*.cpp src/**/*.h
```

### 调试
```bash
chtl compile input.chtl --debug --verbose
```

## 贡献

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 致谢

- 感谢所有贡献者的支持
- 感谢开源社区提供的优秀工具和库
- 特别感谢 C++ 社区和 Web 开发社区

## 联系方式

- 项目主页: [https://github.com/chtl/chtl](https://github.com/chtl/chtl)
- 问题报告: [https://github.com/chtl/chtl/issues](https://github.com/chtl/chtl/issues)
- 讨论区: [https://github.com/chtl/chtl/discussions](https://github.com/chtl/chtl/discussions)

---

**CHTL** - 让Web开发更简单、更强大！