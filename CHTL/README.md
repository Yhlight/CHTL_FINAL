# CHTL - 超文本语言

CHTL 是基于 C++ 语言实现的超文本语言，旨在提供一种更符合开发者编写 HTML 代码的方式。

## 特性

- **简洁语法**: 使用类似 C++ 的语法结构，更符合开发者习惯
- **模板系统**: 支持样式组模板、元素模板、变量组模板
- **自定义系统**: 支持自定义样式组、元素、变量组，具有极高的灵活性
- **模块化**: 支持 CMOD 和 CJMOD 模块系统
- **命名空间**: 防止模块污染，支持嵌套命名空间
- **CHTL JS**: 独立的 JavaScript 扩展语法
- **配置系统**: 支持自定义关键字和编译器行为

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL 编译器核心
│   ├── CHTLContext/         # 编译上下文
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLLoader/         # 文件加载器
│   ├── CHTLManage/         # 管理器
│   ├── CHTLNode/           # AST 节点
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 状态管理
│   ├── CHTLIOStream/       # IO 流
│   └── CMODSystem/         # CMOD 模块系统
├── CHTL_JS/                # CHTL JS 编译器
├── CSS/                    # CSS 编译器
├── JS/                     # JavaScript 编译器
├── Scanner/                # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
├── Util/                   # 工具类
├── Test/                   # 测试套件
└── Module/                 # 模块源码

```

## 编译

```bash
mkdir build
cd build
cmake ..
make
```

## 使用

```bash
./chtl input.chtl -o output.html
```

## 许可证

MIT License