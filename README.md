# CHTL Project

CHTL (CHTL HyperText Language) 是一个创新的模板语言，旨在简化HTML、CSS和JavaScript的开发过程。本项目提供了完整的CHTL语言实现，包括编译器、模块系统、CLI工具和VSCode扩展。

## 🚀 主要特性

- **统一语法** - 简洁的语法结构，支持HTML、CSS、JavaScript的统一编写
- **模块化系统** - 完整的CMOD和CJMOD模块系统支持
- **实时编译** - 快速编译和热重载支持
- **IDE集成** - 完整的VSCode扩展支持
- **CLI工具** - 功能丰富的命令行工具
- **类型安全** - 强类型系统和错误检查
- **高性能** - 优化的编译器和运行时

## 📁 项目结构

```
CHTL/
├── include/                    # 头文件
│   ├── CHTL/                  # CHTL核心头文件
│   ├── CHTL_JS/               # CHTL JS头文件
│   ├── Scanner/               # 扫描器头文件
│   ├── Compiler/              # 编译器头文件
│   ├── Module/                # 模块系统头文件
│   └── CLI/                   # CLI工具头文件
├── src/                       # 源文件
│   ├── CHTL/                  # CHTL核心实现
│   ├── CHTL_JS/               # CHTL JS实现
│   ├── Scanner/                # 统一扫描器
│   ├── Compiler/               # 编译器调度器
│   ├── Module/                 # 模块系统
│   └── CLI/                    # CLI工具
├── test/                      # 测试文件
├── examples/                  # 示例文件
├── vscode-extension/          # VSCode扩展
├── build/                     # 构建目录
└── CMakeLists.txt            # CMake配置文件
```

## 🛠️ 构建说明

### 系统要求

- C++17 或更高版本
- CMake 3.10 或更高版本
- GCC 7.0+ 或 Clang 5.0+ 或 MSVC 2017+

### 构建步骤

```bash
# 克隆项目
git clone <repository-url>
cd CHTL

# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake ..

# 编译项目
make -j4

# 运行测试
make test
```

### 安装

```bash
# 安装到系统
sudo make install

# 或者使用包管理器
# 具体安装方法取决于你的系统
```

## 🚀 快速开始

### 1. 基本使用

创建一个简单的CHTL文件 `hello.chtl`：

```chtl
html {
    head {
        title { text { "Hello CHTL!" } }
    }
    body {
        div {
            text { "Hello from CHTL!" }
            style {
                color: blue;
                font-size: 24px;
                text-align: center;
            }
        }
    }
}
```

编译文件：

```bash
chtl compile hello.chtl -o hello.html
```

### 2. 使用CLI工具

```bash
# 编译文件
chtl compile input.chtl -o output.html

# 监视文件变化
chtl watch src/ -o dist/

# 初始化项目
chtl init my-project

# 构建项目
chtl build

# 启动开发服务器
chtl serve -p 8080
```

### 3. 使用VSCode扩展

1. 安装CHTL Language扩展
2. 打开CHTL文件
3. 享受语法高亮、代码补全和错误检查
4. 使用右键菜单进行编译和预览

## 📚 语言特性

### CHTL语法

```chtl
// 基本HTML结构
html {
    head {
        title { text { "页面标题" } }
        style {
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 20px;
            }
        }
    }
    body {
        div {
            text { "Hello CHTL!" }
            style {
                color: blue;
                font-size: 24px;
            }
        }
    }
}

// 模板定义
[Template] Button {
    button {
        text { "点击我" }
        style {
            background-color: #007bff;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
        }
    }
}

// 模块导入
[Import] @Chtl from "my-module"
```

### CHTL JS语法

```chtl
// 事件监听
listen {
    click: function() {
        console.log("按钮被点击了!");
    }
}

// 动画
animate {
    duration: 1000,
    easing: "ease-in-out"
}

// 路由
router {
    path: "/home",
    component: "HomeComponent"
}
```

## 🔧 配置选项

### CLI配置

```bash
# 设置输出目录
chtl compile input.chtl -o dist/output.html

# 启用调试模式
chtl compile input.chtl --debug

# 启用监视模式
chtl watch src/ --output dist/

# 压缩输出
chtl compile input.chtl --minify
```

### VSCode配置

```json
{
    "chtl.compilerPath": "chtl",
    "chtl.outputDirectory": "dist",
    "chtl.watchMode": false,
    "chtl.minify": false,
    "chtl.debug": false,
    "chtl.autoCompile": true,
    "chtl.previewPort": 8080
}
```

## 📦 模块系统

### CMOD模块

```chtl
// 模块信息
[Info] {
    name = "my-module";
    version = "1.0.0";
    description = "我的CHTL模块";
    author = "开发者";
    license = "MIT";
    category = "theme";
}

// 导出内容
[Export] {
    button: "Button组件";
    card: "Card组件";
}
```

### CJMOD模块

```chtl
// CJMOD API使用
Arg args = Syntax::analyze("$ ** $");
Arg result = CJMODScanner::scan(args, "**");
std::string output = CJMODGenerator::exportResult(result);
```

## 🧪 测试

```bash
# 运行所有测试
make test

# 运行特定测试
./build/bin/test_compiler

# 运行VSCode扩展测试
cd vscode-extension
npm test
```

## 📖 文档

- [CHTL语言规范](CHTL.md)
- [API文档](docs/api.md)
- [示例代码](examples/)
- [VSCode扩展文档](vscode-extension/README.md)

## 🤝 贡献

欢迎贡献代码！请查看 [贡献指南](CONTRIBUTING.md) 了解如何参与项目开发。

## 📄 许可证

本项目采用 MIT 许可证。详情请查看 [LICENSE](LICENSE) 文件。

## 🆘 支持

如果你遇到问题或有任何疑问，请：

1. 查看 [常见问题](docs/faq.md)
2. 提交 [Issue](https://github.com/chtl-project/chtl/issues)
3. 参与 [讨论](https://github.com/chtl-project/chtl/discussions)

## 🎯 路线图

- [x] 核心编译器实现
- [x] 模块系统
- [x] CLI工具
- [x] VSCode扩展
- [ ] 更多IDE支持
- [ ] 性能优化
- [ ] 更多语言特性
- [ ] 社区生态

---

**CHTL** - 让Web开发更简单、更高效！