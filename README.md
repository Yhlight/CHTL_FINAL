# CHTL - 超文本语言

CHTL是基于C++17实现的超文本语言，提供了一种更符合开发者编写HTML代码的方式。

## 特性

- **类C++语法**：使用类似C++的语法结构，更符合开发者习惯
- **模板系统**：支持样式组模板、元素模板、变量组模板
- **自定义系统**：支持自定义样式组、元素、变量组，具有特例化功能
- **CHTL JS**：独立的JavaScript扩展语法
- **模块系统**：CMOD和CJMOD模块化支持
- **统一扫描器**：智能代码分离，支持多种语言混合

## 项目结构

```
CHTL/
├── include/                 # 头文件
│   ├── CHTL/               # CHTL编译器核心
│   ├── Scanner/            # 统一扫描器
│   ├── CodeMerger/         # 代码合并器
│   ├── CompilerDispatcher/ # 编译器调度器
│   └── Util/               # 工具类
├── src/                    # 源文件
│   ├── CHTL/               # CHTL编译器实现
│   ├── Scanner/            # 统一扫描器实现
│   ├── CodeMerger/         # 代码合并器实现
│   ├── CompilerDispatcher/ # 编译器调度器实现
│   └── main.cpp            # 主程序
├── test/                   # 测试文件
├── CMakeLists.txt          # CMake配置
└── README.md               # 项目说明
```

## 构建

### 依赖

- C++17 编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16+
- Google Test (用于测试)

### 构建步骤

```bash
mkdir build
cd build
cmake ..
make
```

### 运行测试

```bash
cd build
make test
```

## 使用示例

### 基本元素

```chtl
div
{
    id: "container";
    class: "main";
    
    text
    {
        Hello World
    }
}
```

### 模板系统

```chtl
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
    line-height: 1.6;
}

div
{
    @Style DefaultText;
}
```

### 自定义元素

```chtl
[Custom] @Element Box
{
    div
    {
        style
        {
            width: 200px;
            height: 200px;
            background-color: red;
        }
    }
}

body
{
    @Element Box;
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
        }
        
        &:hover
        {
            background-color: blue;
        }
    }
}
```

## 开发状态

当前项目处于开发阶段，已实现：

- ✅ 项目基础结构
- ✅ 统一扫描器核心算法
- ✅ CHTL词法分析器
- ✅ CHTL语法分析器基础框架
- ✅ 编译器上下文管理
- ✅ 基础测试框架

正在开发：

- 🔄 CHTL代码生成器
- 🔄 CHTL JS编译器
- 🔄 模块系统
- 🔄 CLI工具
- 🔄 VSCode扩展

## 贡献

欢迎贡献代码！请确保：

1. 代码符合C++17标准
2. 添加适当的测试用例
3. 遵循项目的代码风格
4. 提交前运行所有测试

## 许可证

MIT License

## 联系方式

如有问题或建议，请提交Issue或Pull Request。