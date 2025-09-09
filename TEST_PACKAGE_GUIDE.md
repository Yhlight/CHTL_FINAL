# CHTL特征测试包使用指南

## 概述

CHTL特征测试包是一个专门设计的测试套件，用于验证CHTL.md文档中描述的所有特征。该测试包包含17个主要测试类别，覆盖了CHTL语言的所有核心功能。

## 测试包结构

```
tests/
├── comprehensive_features_test.rs      # 全面特征测试
├── chtl_features_test_package.rs      # CHTL特征测试包
└── run_feature_tests.rs               # 简单测试运行器
```

## 快速开始

### 1. 运行完整测试包

```bash
# 使用Cargo运行测试
cargo test --test comprehensive_features_test

# 或者运行简单测试运行器
cargo run --bin run_feature_tests
```

### 2. 运行特定测试类别

```rust
use chtl::ChtlCompiler;

// 创建编译器实例
let mut compiler = ChtlCompiler::new();

// 运行特定测试
let result = compiler.compile(r#"
    div {
        text: "Hello World";
    }
"#);

assert!(result.is_ok());
```

## 测试类别详解

### 1. 基础语法测试 📝
测试CHTL的基本语法结构：
- 元素结构
- 嵌套元素
- 属性设置
- 自闭合标签

**示例测试**:
```chtl
div {
    text: "Hello World";
}
```

### 2. 注释系统测试 💬
测试三种注释格式：
- 行注释 (`//`)
- 块注释 (`/* */`)
- SQL风格注释 (`--`)

**示例测试**:
```chtl
div {
    // 这是行注释
    /* 这是块注释 */
    -- 这是SQL注释
    text: "Hello";
}
```

### 3. 文本节点测试 📄
测试文本节点的两种语法：
- `text {}` 语法
- `text:` 语法

**示例测试**:
```chtl
div {
    text: "Hello World";
}
```

### 4. 字面量测试 🔢
测试不同类型的字面量：
- 无引号字面量
- 双引号字面量
- 单引号字面量
- 数字字面量

**示例测试**:
```chtl
div {
    class: container;        // 无引号
    id: "main";             // 双引号
    data-value: '123';      // 单引号
    count: 42;              // 数字
}
```

### 5. CE等价性测试 🔄
测试冒号和等号的等价性：
- `:` 语法
- `=` 语法
- 混合使用

**示例测试**:
```chtl
div {
    class: "container";     // 冒号语法
    id = "main";           // 等号语法
}
```

### 6. HTML元素测试 🏷️
测试HTML元素支持：
- 基本HTML结构
- 表单元素
- 列表元素

**示例测试**:
```chtl
html {
    head {
        title { text: "My Page"; }
    }
    body {
        h1 { text: "Welcome"; }
    }
}
```

### 7. 样式系统测试 🎨
测试局部样式块和样式功能：
- 局部样式块
- 样式选择器
- 伪选择器
- 条件样式
- 链式样式

**示例测试**:
```chtl
div {
    style {
        color: #ff0000;
        font-size: 16px;
        opacity: width > 200 ? 0.8 : 1.0;
    }
}
```

### 8. 模板系统测试 📋
测试模板系统功能：
- 样式组模板
- 元素模板
- 变量组模板
- 模板使用

**示例测试**:
```chtl
[Template] @Element button {
    param-type: string;
    param-text: string;
    
    button {
        class: "btn btn-{type}";
        text: "{text}";
    }
}
```

### 9. 自定义系统测试 🔧
测试自定义系统功能：
- 自定义样式组
- 自定义元素
- 特例化
- 索引访问

**示例测试**:
```chtl
[Custom] @Element card {
    param-title: string;
    param-content: string;
    
    div {
        class: "card";
        div {
            class: "card-header";
            text: "{title}";
        }
    }
}
```

### 10. 原始嵌入测试 📦
测试原始代码嵌入：
- HTML嵌入
- CSS嵌入
- JavaScript嵌入
- 命名嵌入

**示例测试**:
```chtl
div {
    [Origin] @Html {
        <div class="raw-html">
            <p>This is raw HTML content</p>
        </div>
    }
}
```

### 11. 导入系统测试 📥
测试导入系统功能：
- CHTL文件导入
- HTML文件导入
- CSS文件导入
- JavaScript文件导入
- 精确导入
- 通配符导入

**示例测试**:
```chtl
[Import] "components.chtl" as components;
[Import] "styles.chtl" as styles;
[Import] "utils.chtl" { Button, Card } as ui;
```

### 12. 命名空间测试 🏷️
测试命名空间功能：
- 基本命名空间
- 嵌套命名空间
- 命名空间导入

**示例测试**:
```chtl
[Namespace] ui {
    Button {
        class: "btn";
        text: "Button";
    }
}
```

### 13. 约束系统测试 🔒
测试约束系统功能：
- 精确约束
- 类型约束
- 全局约束

**示例测试**:
```chtl
[Constraint] except Button {
    data-sensitive: "true";
}
```

### 14. 配置组测试 ⚙️
测试配置组功能：
- 基本配置
- 命名配置
- 导入配置
- 自动化规则

**示例测试**:
```chtl
[Configuration] {
    HTML5_DOCTYPE: true;
    DEBUG_MODE: false;
    INDEX_INITIAL_COUNT: 0;
}
```

### 15. CHTL JS扩展测试 ⚡
测试CHTL JS扩展功能：
- 虚拟对象
- 事件监听
- 事件委托
- 动画
- 路由
- 文件加载器

**示例测试**:
```chtl
script {
    vir MyComponent {
        name: "MyComponent";
        render: function() { return this.name; };
    }
    
    listen .button {
        click: function() { console.log("Button clicked!"); };
    }
}
```

### 16. 模块系统测试 📦
测试模块系统功能：
- CMOD模块
- CJMOD模块
- 模块信息
- 导出表

**示例测试**:
```chtl
[Template] @Element module-button {
    param-text: string;
    
    button {
        class: "module-btn";
        text: "{text}";
    }
}
```

### 17. 高级特性测试 🚀
测试高级特性：
- 复杂嵌套结构
- 响应式设计
- 交互式组件

**示例测试**:
```chtl
html {
    head {
        title { text: "Advanced CHTL"; }
        meta { charset: "UTF-8"; }
    }
    body {
        div {
            class: "container";
            h1 { text: "Welcome to CHTL"; }
            p { text: "This is a complex example."; }
        }
    }
}
```

## 运行测试

### 方法1: 使用Cargo测试

```bash
# 运行所有测试
cargo test

# 运行特定测试
cargo test comprehensive_features_test

# 运行测试并显示输出
cargo test -- --nocapture
```

### 方法2: 使用测试运行器

```bash
# 运行简单测试运行器
cargo run --bin run_feature_tests

# 或者直接运行
rustc run_feature_tests.rs && ./run_feature_tests
```

### 方法3: 在代码中使用

```rust
use chtl::ChtlCompiler;

fn main() {
    let mut compiler = ChtlCompiler::new();
    
    // 测试基础语法
    let source = r#"
        div {
            text: "Hello World";
        }
    "#;
    
    match compiler.compile(source) {
        Ok(html) => println!("编译成功: {}", html),
        Err(e) => println!("编译失败: {}", e),
    }
}
```

## 测试输出

### 输出目录结构

```
feature_test_outputs/
├── basic_syntax_1.html
├── basic_syntax_2.html
├── comment_system_1.html
├── text_nodes_1.html
├── literals_1.html
├── ce_equivalence_1.html
├── html_elements_1.html
├── style_system_1.html
├── template_system_1.html
├── custom_system_1.html
├── raw_embedding_1.html
├── import_system_1.html
├── namespace_1.html
├── constraint_1.html
├── configuration_1.html
├── chtl_js_1.html
├── module_system_1.html
├── advanced_features_1.html
└── test_report.md
```

### 测试报告

测试完成后会生成详细的测试报告，包括：
- 测试概述
- 测试覆盖的特征
- 测试结果统计
- 结论和建议

## 自定义测试

### 添加新测试

```rust
// 在测试文件中添加新测试
#[test]
fn test_my_feature() {
    let mut compiler = ChtlCompiler::new();
    let source = r#"
        // 你的CHTL代码
    "#;
    
    let result = compiler.compile(source);
    assert!(result.is_ok());
}
```

### 创建测试组

```rust
// 创建新的测试组
fn run_my_feature_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🧪 运行我的特征测试...");
    
    let tests = vec![
        ("test1", r#"/* 测试代码 */"#),
        ("test2", r#"/* 测试代码 */"#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 我的特征测试完成");
    Ok(())
}
```

## 故障排除

### 常见问题

1. **编译错误**
   - 检查CHTL语法是否正确
   - 确保所有括号匹配
   - 验证属性值格式

2. **测试失败**
   - 查看错误信息
   - 检查测试代码
   - 验证编译器状态

3. **输出问题**
   - 检查输出目录权限
   - 确保有足够的磁盘空间
   - 验证文件路径

### 调试技巧

```rust
// 启用详细输出
let result = compiler.compile(source);
match result {
    Ok(html) => println!("成功: {}", html),
    Err(e) => {
        println!("错误: {}", e);
        // 添加更多调试信息
    }
}
```

## 贡献指南

### 添加新测试

1. 在相应的测试函数中添加新测试用例
2. 确保测试覆盖了新的特征
3. 更新测试文档
4. 运行测试确保通过

### 改进现有测试

1. 识别需要改进的测试
2. 修改测试代码
3. 验证测试仍然通过
4. 更新相关文档

## 总结

CHTL特征测试包提供了完整的测试覆盖，确保CHTL编译器的所有功能都按预期工作。通过使用这个测试包，您可以：

- 验证CHTL.md中描述的所有特征
- 确保编译器的稳定性和可靠性
- 快速识别和修复问题
- 为新功能添加测试

测试包设计灵活，易于扩展，是CHTL项目开发的重要工具。