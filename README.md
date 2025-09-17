# CHTL - 超文本语言

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)](https://cmake.org/)

CHTL是一个基于C++实现的超文本语言，旨在提供更符合开发者编写HTML代码的方式。项目采用MIT开源协议，具有完整的模块化架构。

## ✨ 特性

- **CHTL JS支持** - 声明式JavaScript语法，支持虚对象、增强选择器等特性
- **统一扫描器** - 智能分离CHTL、CHTL JS、CSS、JS代码片段
- **模块化架构** - 支持CMOD和CJMOD模块系统
- **现代C++** - 使用C++17标准，性能优秀
- **跨平台** - 支持Windows、Linux、macOS

## 🚀 快速开始

### 构建项目

```bash
mkdir build
cd build
cmake ..
make
```

### 运行演示

```bash
./demo
```

### 测试CHTL JS

```bash
./test_chtljs
```

## 📖 使用示例

### CHTL JS示例

```chtl
Vir test = Listen {
    click: () => {
        console.log("Hello from CHTL JS!");
        {{box}}->textContent = "Clicked!";
    }
};

const anim = Animate {
    target: {{box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0,
        transform: scale(0.8)
    },
    end: {
        opacity: 1,
        transform: scale(1.0)
    }
};
```

### CHTL示例

```chtl
html {
    head {
        title {
            text: "CHTL Demo";
        }
    }
    
    body {
        div {
            id: main;
            class: container;
            
            style {
                .container {
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }
            }
        }
    }
}
```

## 🏗️ 项目结构

```
CHTL/
├── CHTL/                    # CHTL核心编译器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLGenerator/      # 代码生成器
│   └── ...
├── CHTLJS/                 # CHTL JS编译器
│   ├── CHTLJSLexer/        # 词法分析器
│   ├── CHTLJSParser/       # 语法分析器
│   ├── CHTLJSGenerator/    # 代码生成器
│   └── ...
├── Scanner/                # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
└── ...
```

## 🔧 开发状态

### ✅ 已完成
- CHTL JS核心编译器
- CHTL核心编译器
- 统一扫描器

### 🚧 进行中
- CMOD模块系统

### ⏳ 待实现
- CJMOD模块系统
- CJMOD API
- CLI工具
- VSCode扩展
- 官方模块

## 📚 文档

- [项目状态报告](PROJECT_STATUS.md)
- [CHTL语法规范](CHTL.md)

## 🤝 贡献

欢迎贡献代码！请查看[贡献指南](CONTRIBUTING.md)了解详细信息。

## 📄 许可证

本项目采用MIT许可证。详情请查看[LICENSE](LICENSE)文件。

## 🙏 致谢

感谢所有为CHTL项目做出贡献的开发者们！

---

**注意**: 本项目仍在积极开发中，API可能会发生变化。