# CHTL - 超文本语言编译器

CHTL 是基于 Go 语言实现的超文本语言编译器，提供一种更符合开发者编写 HTML 代码的方式。

## 特性

- 🚀 基于 Go 语言的高性能编译器
- 📝 支持完整的 CHTL 语法规范
- 🎨 局部样式块和自动化类名/ID
- 🔧 模板系统和自定义组件
- 📦 模块化支持 (CMOD/CJMOD)
- 🌐 CHTL JS 扩展语法
- ⚡ 统一扫描器和代码分离
- 🛠️ 完整的 CLI 工具

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL 编译器核心
│   ├── context/             # 编译上下文
│   ├── generator/           # 代码生成器
│   ├── lexer/               # 词法分析器
│   ├── loader/              # 文件加载器
│   ├── manage/              # 管理器
│   ├── node/                # AST 节点
│   ├── parser/              # 语法分析器
│   ├── state/               # 状态管理
│   └── iostream/            # IO 流
├── CHTLJS/                  # CHTL JS 编译器
├── CSS/                     # CSS 编译器
├── JS/                      # JS 编译器
├── Scanner/                 # 统一扫描器
├── CodeMerger/              # 代码合并器
├── CompilerDispatcher/      # 编译器调度器
├── ThirdParty/              # 第三方依赖
├── Util/                    # 工具类
├── Test/                    # 测试套件
└── Module/                  # 模块源码
```

## 快速开始

```bash
# 安装依赖
go mod tidy

# 编译项目
go build -o chtl ./cmd/chtl

# 运行示例
./chtl compile example.chtl
```

## 许可证

MIT License