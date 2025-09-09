# CHTL 项目文档

欢迎使用 CHTL (Chtholly HyperText Language) 项目文档。本文档提供了完整的 CHTL 语言规范、API 参考、使用指南和示例。

## 目录

- [快速开始](quickstart.md) - 快速上手 CHTL
- [语言规范](language-specification.md) - 完整的 CHTL 语言规范
- [API 参考](api-reference.md) - 详细的 API 文档
- [使用指南](user-guide.md) - 详细的使用指南
- [示例集合](examples.md) - 丰富的代码示例
- [最佳实践](best-practices.md) - 开发最佳实践
- [故障排除](troubleshooting.md) - 常见问题解决
- [贡献指南](contributing.md) - 如何参与项目开发

## 项目概述

CHTL 是一个基于 C++ 17 的超文本语言，用于生成 HTML、CSS 和 JavaScript 代码。它提供了强大的模板系统、自定义系统、模块系统和扩展语法，使开发者能够更高效地创建和维护 Web 应用程序。

### 主要特性

- **完整的 HTML 支持**: 支持所有 HTML 元素和属性
- **强大的模板系统**: 支持样式组、元素和变量组模板
- **灵活的自定义系统**: 支持模板的特例化和定制
- **原始代码嵌入**: 支持直接嵌入 HTML、CSS、JavaScript 代码
- **模块化设计**: 支持 CMOD 和 CJMOD 模块系统
- **CHTL JS 扩展**: 提供增强的 JavaScript 语法
- **VSCode 集成**: 完整的 IDE 支持
- **高性能**: 基于 C++ 17 的高性能实现

### 系统要求

- **编译器**: 支持 C++ 17 的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- **操作系统**: Windows, macOS, Linux
- **内存**: 至少 512MB RAM
- **磁盘空间**: 至少 100MB 可用空间

### 安装

#### 从源码编译

```bash
# 克隆仓库
git clone https://github.com/chtl/chtl.git
cd chtl

# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
make -j4

# 安装 (可选)
sudo make install
```

#### 使用包管理器

```bash
# Ubuntu/Debian
sudo apt install chtl

# macOS (Homebrew)
brew install chtl

# Windows (Chocolatey)
choco install chtl
```

### 快速示例

```chtl
// 简单的 CHTL 示例
html
{
    head
    {
        title { "Hello CHTL!" }
        style
        {
            body {
                font-family: Arial, sans-serif;
                margin: 0;
                padding: 20px;
            }
        }
    }
    body
    {
        h1 { "Welcome to CHTL!" }
        p { "This is a simple CHTL page." }
    }
}
```

### 获取帮助

- **文档**: 查看本文档的各个章节
- **示例**: 查看 `examples/` 目录中的示例代码
- **测试**: 运行 `make test` 查看测试用例
- **问题报告**: 在 GitHub 上提交 Issue
- **讨论**: 加入我们的社区讨论

### 许可证

本项目采用 MIT 许可证。详情请参阅 [LICENSE](../LICENSE) 文件。

### 贡献

我们欢迎社区贡献！请参阅 [贡献指南](contributing.md) 了解如何参与项目开发。

---

*最后更新: 2024年*