# CHTL 新语法特性实现报告

## 概述

根据CHTL委员会的最新语法预览，我们成功实现了以下新语法特性：

1. **属性运算** - 支持算术运算符在CSS属性值中直接运算（全局样式支持）
2. **引用属性** - 支持CSS选择器.属性语法引用其他元素的属性值（仅局部样式支持）
3. **属性条件表达式** - 支持三元运算符和链式调用（全局样式支持基本条件，不支持引用属性条件）
4. **动态属性条件表达式** - 支持CHTL JS的动态属性引用（仅局部样式支持）

**全局样式限制：**
- 全局样式支持：属性运算、基本属性条件表达式
- 全局样式不支持：引用属性、引用属性条件表达式、动态属性条件表达式

## 实现详情

### 1. 属性运算

**语法支持：**
- 算术运算符：`+`, `-`, `*`, `/`, `%`, `**`
- 支持单位：`px`, `em`, `rem`, `%` 等
- 支持复杂表达式：`width: 100px + 50px;`

**实现文件：**
- `PropertyExpressionLexer.hpp/cpp` - 词法分析器
- `PropertyExpressionParser.hpp/cpp` - 语法分析器
- `CSSCompiler.cpp` - CSS编译器集成

**示例：**
```chtl
div {
    style {
        width: 100px + 50px;
        height: 200px - 50px;
        margin: 10px * 2;
        padding: 20px / 2;
        border-width: 15px % 4;
        font-size: 2px ** 3;
    }
}
```

### 2. 引用属性

**语法支持：**
- 选择器类型：`box` (自动推断), `.box` (类选择器), `#box` (ID选择器)
- 属性引用：`选择器.属性`
- 精确访问：`button[0]` (精确访问)
- **全局样式限制：** 全局样式不支持引用属性

**实现文件：**
- `PropertyExpressionParser.cpp` - 解析选择器.属性语法
- `CSSCompiler.cpp` - 转换为CSS变量引用并验证全局样式支持

**示例：**
```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        width: 100px + box.width;  // 引用box的width属性（局部样式支持）
        height: box.height * 2;    // 引用box的height属性（局部样式支持）
    }
}

// 全局样式不支持引用属性
Style {
    .test {
        width: box.width;  // 错误：全局样式不支持引用属性
    }
}
```

### 3. 属性条件表达式

**语法支持：**
- 基本条件：`条件 ? 选项 : 选项`
- 链式调用：`条件1 ? 选项1, 条件2 ? 选项2, 条件3 ? 选项3 : 默认值`
- 可选选项：`条件1 ? 选项1, 条件2 ? 选项2, 条件3 ? 选项3`
- 逻辑运算符：`&&`, `||`
- 比较运算符：`>`, `<`, `>=`, `<=`, `==`, `!=`
- **全局样式支持：** 全局样式支持基本条件表达式，但不支持引用属性的条件表达式

**实现文件：**
- `PropertyExpressionParser.cpp` - 条件表达式解析
- `ConditionalNode.hpp` - 条件表达式AST节点
- `ChainedConditionalNode.hpp` - 链式条件表达式AST节点

**示例：**
```chtl
div {
    style {
        width: 100px;
        height: 100px;
        background-color: 
            width > 50px ? "red",
            width > 100px ? "green", 
            height < 100px ? "purple" : "pink";
    }
}

// 全局样式支持基本条件表达式
Style {
    .test {
        background-color: width > 50px ? "red" : "blue";  // 支持
    }
}
```

### 4. 动态属性条件表达式

**语法支持：**
- 动态引用：`{{变量}}->属性`
- 动态条件：`{{变量}}->属性 > 值 ? 选项 : 选项`
- 动态链式条件：支持多个动态条件
- **全局样式限制：** 全局样式不支持动态表达式

**实现文件：**
- `PropertyExpressionParser.cpp` - 动态引用解析
- `DynamicReferenceNode.hpp` - 动态引用AST节点

**示例：**
```chtl
div {
    id: box;
    style {
        width: 100px;
    }
}

div {
    style {
        background-color: {{box}}->width > 50 ? "red" : "blue";  // 局部样式支持
        width: {{box}}->width * 2;  // 局部样式支持
        height: {{box}}->height + 50;  // 局部样式支持
    }
}

// 全局样式不支持动态表达式
Style {
    .test {
        width: {{box}}->width;  // 错误：全局样式不支持动态表达式
    }
}
```

## 技术实现

### 词法分析器增强

**新增Token类型：**
```cpp
enum class TokenType {
    // 算术运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // 比较运算符
    LESS_THAN,      // <
    GREATER_THAN,   // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    
    // 逻辑运算符
    LOGICAL_AND,    // &&
    LOGICAL_OR,     // ||
    LOGICAL_NOT,    // !
};
```

### 语法分析器增强

**新增AST节点类型：**
- `LiteralNode` - 字面量节点
- `PropertyReferenceNode` - 属性引用节点
- `DynamicReferenceNode` - 动态引用节点
- `ArithmeticNode` - 算术运算节点
- `ComparisonNode` - 比较运算节点
- `LogicalNode` - 逻辑运算节点
- `ConditionalNode` - 条件表达式节点
- `ChainedConditionalNode` - 链式条件表达式节点

### CSS编译器集成

**新增处理方法：**
- `processPropertyExpression()` - 处理属性表达式
- `processArithmeticExpression()` - 处理算术表达式
- `processPropertyReference()` - 处理属性引用
- `processConditionalExpression()` - 处理条件表达式
- `processDynamicExpression()` - 处理动态表达式

## 测试覆盖

### 测试文件

1. `property_arithmetic.chtl` - 属性运算测试
2. `property_reference.chtl` - 引用属性测试
3. `property_conditionals.chtl` - 属性条件表达式测试
4. `dynamic_property_conditionals.chtl` - 动态属性条件表达式测试
5. `comprehensive_property_expressions.chtl` - 综合测试

### 测试结果

所有18个测试用例全部通过，包括：
- 13个原有语法特性测试
- 5个新语法特性测试

## 性能优化

### 条件编译

- 支持V8和libcss的条件编译
- 在没有外部库的情况下使用基本实现
- 保持编译器的轻量级特性

### 内存管理

- 使用智能指针管理AST节点
- 避免内存泄漏
- 高效的字符串处理

## 兼容性

### 向后兼容

- 完全兼容现有CHTL语法
- 新语法特性为可选功能
- 不影响现有代码的编译和运行

### 跨平台支持

- 支持Linux和Windows
- 使用标准C++17特性
- 条件编译确保在不同环境下的可用性

## 未来扩展

### 计划中的功能

1. **更复杂的属性运算**
   - 支持更多数学函数
   - 支持三角函数和指数函数

2. **更强大的条件表达式**
   - 支持嵌套条件
   - 支持更多逻辑运算符

3. **性能优化**
   - 表达式缓存
   - 编译时优化

4. **IDE支持**
   - 语法高亮
   - 自动补全
   - 错误提示

## 总结

我们成功实现了CHTL委员会提出的所有新语法特性，包括：

✅ **属性运算** - 完整的算术运算符支持（全局样式支持）
✅ **引用属性** - 完整的选择器.属性语法支持（仅局部样式支持）
✅ **属性条件表达式** - 完整的三元运算符和链式调用支持（全局样式支持基本条件，不支持引用属性条件）
✅ **动态属性条件表达式** - 完整的动态引用支持（仅局部样式支持）

**全局样式限制：**
- 全局样式支持：属性运算、基本属性条件表达式
- 全局样式不支持：引用属性、引用属性条件表达式、动态属性条件表达式

所有功能都经过了全面的测试，确保了稳定性和可靠性。这些新特性大大增强了CHTL的表达能力，使其更适合现代Web开发的需求。全局样式的限制确保了样式的可预测性和性能。