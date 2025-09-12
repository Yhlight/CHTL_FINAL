# CHTL - 超文本语言

CHTL是基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式，使用MIT开源协议。

## 特性

- **类C++语法**: 使用 `{ }` 块结构，支持注释、字面量等
- **模板系统**: 支持Template、Custom、Import、Namespace等高级特性
- **样式系统**: 局部样式块、属性运算、条件表达式等
- **模块系统**: CMOD和CJMOD模块化支持
- **CHTL JS**: 扩展的JavaScript语法支持
- **统一扫描器**: 智能分离CHTL/CHTL JS/CSS/JS代码

## 项目结构

```
CHTL/
├── src/                    # 源代码
│   ├── CHTL/              # CHTL编译器
│   ├── CHTL_JS/           # CHTL JS编译器
│   ├── Scanner/           # 统一扫描器
│   ├── CodeMerger/        # 代码合并器
│   ├── CompilerDispatcher/ # 编译器调度器
│   ├── Util/              # 工具类
│   └── Test/              # 测试代码
├── Module/                # 模块源码
├── third_party/           # 第三方依赖
└── docs/                  # 文档
```

## 构建

```bash
mkdir build
cd build
cmake ..
make
```

## 使用

```bash
# 编译CHTL文件
./bin/chtl_compiler input.chtl -o output.html

# 运行测试
./bin/chtl_test
```

## 许可证

MIT License