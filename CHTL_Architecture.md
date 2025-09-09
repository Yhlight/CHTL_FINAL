# CHTL 编译器架构设计

## 项目概述
CHTL (C++ HyperText Language) 是一个基于 C++ 17 实现的超文本语言，旨在提供更符合开发者编写 HTML 代码的方式。

## 核心架构

### 1. 项目结构
```
CHTL/
├── src/
│   ├── CHTL/                    # CHTL 编译器核心
│   │   ├── CHTLContext.h/cpp    # 编译上下文管理
│   │   ├── CHTLGenerator.h/cpp  # 代码生成器
│   │   ├── CHTLLexer/           # 词法分析器
│   │   │   ├── Lexer.h/cpp
│   │   │   ├── GlobalMap.h/cpp
│   │   │   └── Token.h/cpp
│   │   ├── CHTLLoader.h/cpp     # 文件加载器
│   │   ├── CHTLManage.h/cpp     # 管理器
│   │   ├── CHTLNode/            # AST 节点
│   │   │   ├── BaseNode.h/cpp
│   │   │   ├── ElementNode.h/cpp
│   │   │   ├── TextNode.h/cpp
│   │   │   ├── CommentNode.h/cpp
│   │   │   ├── TemplateNode.h/cpp
│   │   │   ├── CustomNode.h/cpp
│   │   │   ├── StyleNode.h/cpp
│   │   │   ├── ScriptNode.h/cpp
│   │   │   ├── OriginNode.h/cpp
│   │   │   ├── ImportNode.h/cpp
│   │   │   ├── ConfigNode.h/cpp
│   │   │   ├── NamespaceNode.h/cpp
│   │   │   └── OperatorNode.h/cpp
│   │   ├── CHTLParser.h/cpp     # 语法分析器
│   │   ├── CHTLState.h/cpp      # 状态管理
│   │   ├── CHTLIOStream.h/cpp   # IO 流
│   │   └── CMODSystem.h/cpp     # CMOD 模块系统
│   ├── CHTLJS/                  # CHTL JS 编译器
│   │   ├── CHTLJSContext.h/cpp
│   │   ├── CHTLJSGenerator.h/cpp
│   │   ├── CHTLJSLexer/
│   │   ├── CHTLJSLoader.h/cpp
│   │   ├── CHTLJSManage.h/cpp
│   │   ├── CHTLJSNode/
│   │   ├── CHTLJSParser.h/cpp
│   │   ├── CHTLJSState.h/cpp
│   │   ├── CHTLJSIOStream.h/cpp
│   │   └── CJMODSystem.h/cpp
│   ├── CSS/                     # CSS 编译器
│   ├── JS/                      # JavaScript 编译器
│   ├── Scanner/                 # 统一扫描器
│   ├── CodeMerger/              # 代码合并器
│   ├── CompilerDispatcher/      # 编译器调度器
│   ├── ThirdParty/              # 第三方库
│   ├── Util/                    # 工具类
│   │   ├── FileSystem/
│   │   ├── ZipUtil/
│   │   └── StringUtil/
│   └── Test/                    # 测试套件
│       ├── UtilTest/
│       ├── TokenTest/
│       └── AstTest/
├── Module/                      # 模块源码
├── include/                     # 头文件
├── lib/                         # 库文件
├── bin/                         # 可执行文件
├── docs/                        # 文档
└── CMakeLists.txt              # 构建配置
```

### 2. 核心组件设计

#### 2.1 词法分析器 (CHTLLexer)
- **功能**：将 CHTL 源代码转换为 Token 序列
- **支持语法**：
  - 注释：`//`、`/**/`、`--`
  - 文本节点：`text { }`、`text:`
  - 字面量：无修饰、双引号、单引号
  - 元素节点：HTML 元素
  - 属性：`属性名 : "属性值"`
  - 局部样式块：`style { }`
  - 模板：`[Template] @Style/Element/Var`
  - 自定义：`[Custom] @Style/Element/Var`
  - 原始嵌入：`[Origin] @Html/Style/JavaScript`
  - 导入：`[Import] @Chtl/Html/Style/JavaScript`
  - 配置：`[Configuration]`
  - 命名空间：`[Namespace]`

#### 2.2 语法分析器 (CHTLParser)
- **功能**：将 Token 序列转换为抽象语法树 (AST)
- **核心算法**：递归下降解析
- **支持语法结构**：
  - 元素节点解析
  - 属性解析
  - 局部样式块解析
  - 模板系统解析
  - 自定义系统解析
  - 导入系统解析
  - 约束系统解析
  - 配置组解析

#### 2.3 代码生成器 (CHTLGenerator)
- **功能**：将 AST 转换为目标代码
- **输出格式**：
  - HTML 结构
  - CSS 样式
  - JavaScript 代码
- **特性**：
  - 模板展开
  - 自定义特例化
  - 样式优化
  - 代码压缩

#### 2.4 统一扫描器 (Scanner)
- **功能**：代码切割和语法边界识别
- **核心算法**：
  - 可变长度切片
  - 智能扩增
  - 占位符机制
  - 宽判严判
- **支持分离**：
  - CHTL 代码
  - CHTL JS 代码
  - CSS 代码
  - JavaScript 代码

#### 2.5 编译器调度器 (CompilerDispatcher)
- **功能**：协调各个编译器的执行
- **工作流程**：
  1. 接收源代码
  2. 调用统一扫描器进行代码切割
  3. 分发代码片段到对应编译器
  4. 收集编译结果
  5. 调用代码合并器生成最终输出

### 3. 数据流设计

```
源代码输入
    ↓
统一扫描器 (代码切割)
    ↓
┌─────────┬─────────┬─────────┬─────────┐
│ CHTL    │ CHTL JS │  CSS    │  JS     │
│ 编译器   │ 编译器   │ 编译器   │ 编译器   │
└─────────┴─────────┴─────────┴─────────┘
    ↓
代码合并器
    ↓
最终输出 (HTML + CSS + JS)
```

### 4. 核心数据结构

#### 4.1 Token 结构
```cpp
enum class TokenType {
    // 基础类型
    IDENTIFIER, STRING, NUMBER, SYMBOL,
    
    // 注释类型
    LINE_COMMENT, BLOCK_COMMENT, GENERATOR_COMMENT,
    
    // 关键字
    TEMPLATE, CUSTOM, ORIGIN, IMPORT, CONFIGURATION, NAMESPACE,
    STYLE, ELEMENT, VAR, HTML, JAVASCRIPT, CHTL,
    
    // 操作符
    COLON, EQUALS, SEMICOLON, COMMA,
    LBRACE, RBRACE, LBRACKET, RBRACKET, LPAREN, RPAREN,
    
    // 特殊符号
    AT_SYMBOL, HASH, DOT, AMPERSAND
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
};
```

#### 4.2 AST 节点基类
```cpp
class BaseNode {
public:
    enum class NodeType {
        ELEMENT, TEXT, COMMENT, TEMPLATE, CUSTOM,
        STYLE, SCRIPT, ORIGIN, IMPORT, CONFIG,
        NAMESPACE, OPERATOR
    };
    
    virtual ~BaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual std::string generate() = 0;
    
protected:
    NodeType nodeType;
    size_t line;
    size_t column;
};
```

### 5. 模块系统设计

#### 5.1 CMOD 模块
- **结构**：src/ + info/ 目录结构
- **信息文件**：`[Info]` 和 `[Export]` 块
- **支持功能**：模板、自定义、原始嵌入

#### 5.2 CJMOD 模块
- **结构**：C++ 源码 + info/ 目录
- **API**：CJMOD API 用于扩展 CHTL JS 语法
- **支持功能**：虚对象、增强选择器、动画等

### 6. 错误处理机制

#### 6.1 错误类型
- **词法错误**：非法字符、未闭合字符串等
- **语法错误**：语法结构错误、缺少分号等
- **语义错误**：未定义变量、类型不匹配等
- **运行时错误**：模块加载失败、文件不存在等

#### 6.2 错误报告
- **位置信息**：行号、列号、文件路径
- **错误描述**：详细的错误信息
- **建议修复**：可能的修复建议

### 7. 性能优化策略

#### 7.1 编译时优化
- **模板内联**：小模板直接内联
- **死代码消除**：移除未使用的代码
- **常量折叠**：编译时计算常量表达式

#### 7.2 运行时优化
- **模块缓存**：缓存已加载的模块
- **增量编译**：只重新编译修改的文件
- **并行编译**：多线程编译独立模块

### 8. 扩展性设计

#### 8.1 插件系统
- **CJMOD API**：用于扩展 CHTL JS 语法
- **自定义节点**：支持用户自定义 AST 节点
- **自定义生成器**：支持自定义代码生成逻辑

#### 8.2 配置系统
- **关键字自定义**：支持自定义关键字名称
- **编译选项**：支持各种编译选项
- **模块配置**：支持模块特定配置

## 实现优先级

1. **第一阶段**：基础词法和语法分析器
2. **第二阶段**：基础代码生成器
3. **第三阶段**：模板和自定义系统
4. **第四阶段**：模块系统
5. **第五阶段**：CHTL JS 扩展
6. **第六阶段**：工具和 IDE 支持

## 技术选型

- **编程语言**：C++ 17
- **构建系统**：CMake
- **测试框架**：Google Test
- **文档生成**：Doxygen
- **版本控制**：Git
- **持续集成**：GitHub Actions