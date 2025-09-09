# CHTL - C++ HyperText Language

CHTL是一个基于C++实现的超文本语言，旨在提供一种更符合开发者编写HTML代码的方式。CHTL使用MIT开源协议。

## 特性

### 核心语法
- **注释系统**: 支持 `//` 单行注释、`/**/` 多行注释和 `--` 生成器识别注释
- **文本节点**: 使用 `text { }` 语法或 `text: "内容"` 属性语法
- **字面量支持**: 支持无修饰字面量、双引号字符串和单引号字符串
- **CE对等式**: `:` 与 `=` 完全等价
- **元素节点**: 支持所有HTML元素

### 局部样式块
- **内联样式**: 在元素内部嵌套 `style {}`
- **自动化类名/ID**: 自动添加类名和ID
- **上下文推导**: 使用 `&` 表示当前元素
- **属性条件表达式**: 支持条件样式和链式调用
- **逻辑运算符**: 支持 `&&` 和 `||`
- **指向属性条件表达式**: 引用其他元素属性

### 模板系统
- **样式组模板**: `[Template] @Style 名称`
- **元素模板**: `[Template] @Element 名称`
- **变量组模板**: `[Template] @Var 名称`

### 自定义系统
- **自定义样式组**: 支持无值样式组和特例化
- **自定义元素**: 支持元素特例化、索引访问、插入和删除
- **变量组特例化**: 支持变量覆盖

### 原始嵌入系统
- **HTML嵌入**: `[Origin] @Html`
- **CSS嵌入**: `[Origin] @Style`
- **JS嵌入**: `[Origin] @JavaScript`
- **自定义类型支持**

### 导入系统
- **文件导入**: 支持HTML、CSS、JS文件导入
- **CHTL文件导入**: 支持精确导入、类型导入、通配导入
- **模块导入**: 支持CMOD和CJMOD模块

### 命名空间系统
- **防止模块污染**: 支持命名空间嵌套
- **自动合并**: 同名命名空间自动合并

### 约束系统
- **精确约束**: 禁止特定元素或模板
- **类型约束**: 禁止特定类型
- **全局约束**: 命名空间级别的约束

### 配置系统
- **配置组**: 支持配置组和关键字自定义
- **命名配置组**: 支持多个配置组
- **导入配置组**: 支持配置组导入

### CHTL JS 扩展
- **文件载入**: AMD风格的JavaScript文件加载器
- **局部script**: 支持在元素内部编写JS代码
- **增强选择器**: 使用 `{{CSS选择器}}` 创建DOM对象
- **增强监听器**: 使用 `listen` 快捷绑定事件
- **事件委托**: 基于事件委托的增强语法
- **动画系统**: 封装 `requestAnimationFrame`
- **虚对象**: 提供访问CHTL JS函数的元信息能力
- **路由系统**: 快速创建SPA页面架构

## 项目结构

```
CHTL/
├── CHTL/                    # CHTL编译器核心
│   ├── CHTLContext/         # 上下文管理
│   ├── CHTLGenerator/       # 代码生成器
│   ├── CHTLLexer/          # 词法分析器
│   ├── CHTLLoader/         # 文件加载器
│   ├── CHTLManage/         # 管理器
│   ├── CHTLNode/           # AST节点
│   ├── CHTLParser/         # 语法分析器
│   ├── CHTLState/          # 状态管理
│   ├── CHTLIOStream/       # IO流
│   └── CMODSystem/         # CMOD模块系统
├── CHTL JS/                # CHTL JS编译器
│   ├── CHTLJSContext/      # JS上下文
│   ├── CHTLJSGenerator/    # JS代码生成器
│   ├── CHTLJSLexer/        # JS词法分析器
│   ├── CHTLJSLoader/       # JS文件加载器
│   ├── CHTLJSManage/       # JS管理器
│   ├── CHTLJSNode/         # JS AST节点
│   ├── CHTLJSParser/       # JS语法分析器
│   ├── CHTLJSState/        # JS状态管理
│   ├── CHTLJSIOStream/     # JS IO流
│   └── CJMODSystem/        # CJMOD模块系统
├── Scanner/                # 统一扫描器
├── CodeMerger/             # 代码合并器
├── CompilerDispatcher/     # 编译器调度器
├── Util/                   # 工具类
├── Test/                   # 测试套件
├── Module/                 # 模块源码
└── examples/               # 示例文件
```

## 编译流程

```
CHTL源代码 → 统一扫描器 → 代码片段分离 → 编译器调度器 → 
CHTL编译器 + CHTL JS编译器 → 代码合并器 → 
CSS编译器 + JS编译器 → 最终HTML输出
```

## 构建说明

### 依赖要求
- C++17 或更高版本
- CMake 3.16 或更高版本
- 支持C++17的编译器（GCC 7+, Clang 5+, MSVC 2017+）

### 构建步骤

1. 克隆仓库
```bash
git clone <repository-url>
cd CHTL
```

2. 创建构建目录
```bash
mkdir build
cd build
```

3. 配置CMake
```bash
cmake ..
```

4. 编译
```bash
make
```

5. 运行测试
```bash
./Test/TestBasic
```

## 使用示例

### 基本语法
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

### 局部样式
```chtl
div
{
    style
    {
        width: 100px;
        height: 100px;
        background-color: red;
        
        &:hover
        {
            background-color: blue;
        }
    }
}
```

### 模板使用
```chtl
[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    cursor: pointer;
}

button
{
    style
    {
        @Style ButtonStyle;
        background-color: #007bff;
        color: white;
    }
}
```

### CHTL JS
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
    
    script
    {
        {{box}}->addEventListener('click', () => {
            console.log('Box clicked!');
        });
    }
}
```

## 命令行使用

```bash
# 编译CHTL文件
./chtl input.chtl output.html

# 指定编译选项
./chtl input.chtl output.html --pretty-print --minify
```

## 开发状态

当前项目处于开发阶段，实现了基础的核心功能：

- ✅ 词法分析器 (CHTLLexer)
- ✅ 基础AST节点 (BaseNode, ElementNode, TextNode)
- ✅ 代码生成器 (CHTLGenerator)
- ✅ 编译器调度器 (CompilerDispatcher)
- ✅ 统一扫描器 (UnifiedScanner)
- 🔄 语法分析器 (CHTLParser) - 部分实现
- 🔄 模板系统 - 部分实现
- 🔄 自定义系统 - 待实现
- 🔄 导入系统 - 待实现
- 🔄 命名空间系统 - 待实现
- 🔄 约束系统 - 待实现
- 🔄 配置系统 - 待实现
- 🔄 CHTL JS编译器 - 待实现
- 🔄 模块系统 - 待实现
- 🔄 CLI工具 - 待实现
- 🔄 VSCode扩展 - 待实现

## 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

## 许可证

本项目使用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 联系方式

如有问题或建议，请通过以下方式联系：

- 创建 Issue
- 发送邮件
- 参与讨论

---

**注意**: 这是一个正在开发中的项目，API和功能可能会发生变化。请关注更新日志获取最新信息。