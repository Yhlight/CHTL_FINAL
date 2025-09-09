//! 全面特征测试 - 测试CHTL.md中出现的所有特征

use chtl::{ChtlCompiler, Result};
use std::fs;
use std::path::Path;

/// 全面特征测试套件
pub struct ComprehensiveFeatureTest {
    compiler: ChtlCompiler,
    test_dir: String,
}

impl ComprehensiveFeatureTest {
    pub fn new() -> Self {
        Self {
            compiler: ChtlCompiler::new(),
            test_dir: "test_outputs".to_string(),
        }
    }
    
    /// 运行所有特征测试
    pub fn run_all_tests(&mut self) -> Result<()> {
        println!("🚀 开始运行CHTL全面特征测试...");
        
        // 创建测试输出目录
        fs::create_dir_all(&self.test_dir)?;
        
        // 1. 基础语法测试
        self.test_basic_syntax()?;
        
        // 2. 注释系统测试
        self.test_comment_system()?;
        
        // 3. 文本节点测试
        self.test_text_nodes()?;
        
        // 4. 字面量测试
        self.test_literals()?;
        
        // 5. CE等价性测试
        self.test_ce_equivalence()?;
        
        // 6. HTML元素测试
        self.test_html_elements()?;
        
        // 7. 局部样式块测试
        self.test_local_style_blocks()?;
        
        // 8. 样式选择器测试
        self.test_style_selectors()?;
        
        // 9. 自动化功能测试
        self.test_automation_features()?;
        
        // 10. 属性条件表达式测试
        self.test_attribute_conditional_expressions()?;
        
        // 11. 模板系统测试
        self.test_template_system()?;
        
        // 12. 自定义系统测试
        self.test_custom_system()?;
        
        // 13. 原始嵌入测试
        self.test_raw_embedding()?;
        
        // 14. 导入系统测试
        self.test_import_system()?;
        
        // 15. 命名空间测试
        self.test_namespace_system()?;
        
        // 16. 约束系统测试
        self.test_constraint_system()?;
        
        // 17. 配置组测试
        self.test_configuration_group()?;
        
        // 18. CHTL JS扩展测试
        self.test_chtl_js_extension()?;
        
        // 19. 模块系统测试
        self.test_module_system()?;
        
        // 20. 高级特性测试
        self.test_advanced_features()?;
        
        println!("✅ 所有特征测试完成！");
        Ok(())
    }
    
    /// 1. 基础语法测试
    fn test_basic_syntax(&mut self) -> Result<()> {
        println!("📝 测试基础语法...");
        
        let test_cases = vec![
            // 基本元素结构
            r#"
            div {
                text: "Hello World";
            }
            "#,
            
            // 嵌套元素
            r#"
            html {
                head {
                    title { text: "My Page"; }
                }
                body {
                    h1 { text: "Welcome"; }
                    p { text: "This is a paragraph."; }
                }
            }
            "#,
            
            // 属性设置
            r#"
            div {
                class: "container";
                id: "main";
                data-value: "123";
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "基础语法测试 {} 失败", i + 1);
            self.save_test_output(&format!("basic_syntax_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 基础语法测试通过");
        Ok(())
    }
    
    /// 2. 注释系统测试
    fn test_comment_system(&mut self) -> Result<()> {
        println!("💬 测试注释系统...");
        
        let test_cases = vec![
            // 行注释
            r#"
            div {
                // 这是行注释
                text: "Hello";
            }
            "#,
            
            // 块注释
            r#"
            div {
                /* 这是块注释
                   可以跨多行 */
                text: "Hello";
            }
            "#,
            
            // SQL风格注释
            r#"
            div {
                -- 这是SQL风格注释
                text: "Hello";
            }
            "#,
            
            // 混合注释
            r#"
            div {
                // 行注释
                /* 块注释 */
                -- SQL注释
                text: "Hello";
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "注释系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("comment_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 注释系统测试通过");
        Ok(())
    }
    
    /// 3. 文本节点测试
    fn test_text_nodes(&mut self) -> Result<()> {
        println!("📄 测试文本节点...");
        
        let test_cases = vec![
            // text {} 语法
            r#"
            div {
                text {
                    content: "Hello World";
                }
            }
            "#,
            
            // text: 语法
            r#"
            div {
                text: "Hello World";
            }
            "#,
            
            // 嵌套文本
            r#"
            div {
                text: "Hello ";
                span {
                    text: "World";
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "文本节点测试 {} 失败", i + 1);
            self.save_test_output(&format!("text_nodes_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 文本节点测试通过");
        Ok(())
    }
    
    /// 4. 字面量测试
    fn test_literals(&mut self) -> Result<()> {
        println!("🔢 测试字面量...");
        
        let test_cases = vec![
            // 无引号字面量
            r#"
            div {
                class: container;
                id: main;
            }
            "#,
            
            // 双引号字面量
            r#"
            div {
                class: "container";
                id: "main";
            }
            "#,
            
            // 单引号字面量
            r#"
            div {
                class: 'container';
                id: 'main';
            }
            "#,
            
            // 数字字面量
            r#"
            div {
                data-count: 123;
                data-price: 45.67;
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "字面量测试 {} 失败", i + 1);
            self.save_test_output(&format!("literals_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 字面量测试通过");
        Ok(())
    }
    
    /// 5. CE等价性测试
    fn test_ce_equivalence(&mut self) -> Result<()> {
        println!("🔄 测试CE等价性...");
        
        let test_cases = vec![
            // : 语法
            r#"
            div {
                class: "container";
                id: "main";
            }
            "#,
            
            // = 语法
            r#"
            div {
                class = "container";
                id = "main";
            }
            "#,
            
            // 混合使用
            r#"
            div {
                class: "container";
                id = "main";
                data-value: "123";
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "CE等价性测试 {} 失败", i + 1);
            self.save_test_output(&format!("ce_equivalence_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ CE等价性测试通过");
        Ok(())
    }
    
    /// 6. HTML元素测试
    fn test_html_elements(&mut self) -> Result<()> {
        println!("🏷️ 测试HTML元素...");
        
        let test_cases = vec![
            // 基本HTML结构
            r#"
            html {
                head {
                    title { text: "My Page"; }
                    meta { charset: "UTF-8"; }
                }
                body {
                    h1 { text: "Welcome"; }
                    p { text: "Hello World"; }
                }
            }
            "#,
            
            // 自闭合标签
            r#"
            div {
                img { src: "image.jpg"; alt: "Image"; }
                br;
                hr;
            }
            "#,
            
            // 表单元素
            r#"
            form {
                input { type: "text"; name: "username"; }
                input { type: "password"; name: "password"; }
                button { type: "submit"; text: "Submit"; }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "HTML元素测试 {} 失败", i + 1);
            self.save_test_output(&format!("html_elements_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ HTML元素测试通过");
        Ok(())
    }
    
    /// 7. 局部样式块测试
    fn test_local_style_blocks(&mut self) -> Result<()> {
        println!("🎨 测试局部样式块...");
        
        let test_cases = vec![
            // 基本样式块
            r#"
            div {
                style {
                    color: #ff0000;
                    font-size: 16px;
                    margin: 10px;
                }
            }
            "#,
            
            // 嵌套样式
            r#"
            div {
                style {
                    background-color: #f0f0f0;
                    padding: 20px;
                }
                p {
                    style {
                        color: #333;
                        font-weight: bold;
                    }
                }
            }
            "#,
            
            // 复杂样式
            r#"
            div {
                style {
                    width: 100%;
                    height: 200px;
                    background: linear-gradient(45deg, #ff0000, #00ff00);
                    border-radius: 10px;
                    box-shadow: 0 4px 8px rgba(0,0,0,0.1);
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "局部样式块测试 {} 失败", i + 1);
            self.save_test_output(&format!("local_style_blocks_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 局部样式块测试通过");
        Ok(())
    }
    
    /// 8. 样式选择器测试
    fn test_style_selectors(&mut self) -> Result<()> {
        println!("🎯 测试样式选择器...");
        
        let test_cases = vec![
            // 类选择器
            r#"
            div {
                class: "container";
                style {
                    background-color: #f0f0f0;
                }
            }
            "#,
            
            // ID选择器
            r#"
            div {
                id: "main";
                style {
                    width: 100%;
                    height: 100vh;
                }
            }
            "#,
            
            // 伪类选择器
            r#"
            button {
                style {
                    background-color: #007bff;
                    color: white;
                }
                hover {
                    background-color: #0056b3;
                }
            }
            "#,
            
            // 伪元素选择器
            r#"
            p {
                style {
                    position: relative;
                }
                before {
                    content: "→ ";
                    color: #007bff;
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "样式选择器测试 {} 失败", i + 1);
            self.save_test_output(&format!("style_selectors_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 样式选择器测试通过");
        Ok(())
    }
    
    /// 9. 自动化功能测试
    fn test_automation_features(&mut self) -> Result<()> {
        println!("🤖 测试自动化功能...");
        
        let test_cases = vec![
            // 自动类添加
            r#"
            div {
                style {
                    color: red;
                }
                // 应该自动添加类名
            }
            "#,
            
            // 自动ID添加
            r#"
            div {
                id: "unique-element";
                style {
                    font-weight: bold;
                }
            }
            "#,
            
            // 上下文推断
            r#"
            div {
                class: "parent";
                div {
                    class: "child";
                    // 应该推断父级上下文
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "自动化功能测试 {} 失败", i + 1);
            self.save_test_output(&format!("automation_features_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 自动化功能测试通过");
        Ok(())
    }
    
    /// 10. 属性条件表达式测试
    fn test_attribute_conditional_expressions(&mut self) -> Result<()> {
        println!("🔀 测试属性条件表达式...");
        
        let test_cases = vec![
            // 三元运算符
            r#"
            div {
                style {
                    opacity: width > 200 ? 0.8 : 1.0;
                    display: isVisible ? block : none;
                }
            }
            "#,
            
            // 链式调用
            r#"
            div {
                style {
                    transform: translateX(10px) rotate(45deg) scale(1.2);
                    background: linear-gradient(45deg, #ff0000, #00ff00);
                }
            }
            "#,
            
            // 逻辑运算符
            r#"
            div {
                style {
                    opacity: (width > 200) && (height > 100) ? 0.8 : 1.0;
                    display: isVisible || isActive ? block : none;
                }
            }
            "#,
            
            // 指向属性
            r#"
            div {
                data-width: "200";
                style {
                    width: data-width + "px";
                    height: data-width * 2 + "px";
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "属性条件表达式测试 {} 失败", i + 1);
            self.save_test_output(&format!("attribute_conditional_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 属性条件表达式测试通过");
        Ok(())
    }
    
    /// 11. 模板系统测试
    fn test_template_system(&mut self) -> Result<()> {
        println!("📋 测试模板系统...");
        
        let test_cases = vec![
            // 样式组模板
            r#"
            [Template] @Style primary-button {
                background-color: #007bff;
                color: white;
                padding: 10px 20px;
                border: none;
                border-radius: 4px;
            }
            "#,
            
            // 元素模板
            r#"
            [Template] @Element button {
                param-type: string;
                param-text: string;
                
                button {
                    class: "btn btn-{type}";
                    text: "{text}";
                }
            }
            "#,
            
            // 变量组模板
            r#"
            [Template] @Var colors {
                primary: #007bff;
                secondary: #6c757d;
                success: #28a745;
                danger: #dc3545;
            }
            "#,
            
            // 模板使用
            r#"
            div {
                button {
                    type: "primary";
                    text: "Click me";
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "模板系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("template_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 模板系统测试通过");
        Ok(())
    }
    
    /// 12. 自定义系统测试
    fn test_custom_system(&mut self) -> Result<()> {
        println!("🔧 测试自定义系统...");
        
        let test_cases = vec![
            // 自定义样式组
            r#"
            [Custom] @Style my-theme {
                primary-color: #ff6b6b;
                secondary-color: #4ecdc4;
                font-family: "Arial", sans-serif;
            }
            "#,
            
            // 自定义元素
            r#"
            [Custom] @Element card {
                param-title: string;
                param-content: string;
                
                div {
                    class: "card";
                    div {
                        class: "card-header";
                        text: "{title}";
                    }
                    div {
                        class: "card-body";
                        text: "{content}";
                    }
                }
            }
            "#,
            
            // 特例化
            r#"
            [Custom] @Element special-card {
                inherit: card;
                delete-property: background-color;
                add-property: border: 2px solid #ff6b6b;
            }
            "#,
            
            // 索引访问
            r#"
            div {
                data-items: ["item1", "item2", "item3"];
                ul {
                    li { text: data-items[0]; }
                    li { text: data-items[1]; }
                    li { text: data-items[2]; }
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "自定义系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("custom_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 自定义系统测试通过");
        Ok(())
    }
    
    /// 13. 原始嵌入测试
    fn test_raw_embedding(&mut self) -> Result<()> {
        println!("📦 测试原始嵌入...");
        
        let test_cases = vec![
            // HTML原始嵌入
            r#"
            div {
                [Origin] @Html {
                    <div class="raw-html">
                        <p>This is raw HTML content</p>
                    </div>
                }
            }
            "#,
            
            // CSS原始嵌入
            r#"
            div {
                [Origin] @Style {
                    .custom-class {
                        background-color: #f0f0f0;
                        padding: 20px;
                    }
                }
            }
            "#,
            
            // JavaScript原始嵌入
            r#"
            div {
                [Origin] @JavaScript {
                    console.log("Raw JavaScript");
                    function myFunction() {
                        return "Hello World";
                    }
                }
            }
            "#,
            
            // 命名原始嵌入
            r#"
            div {
                [Origin] @Html my-component {
                    <my-component>
                        <slot></slot>
                    </my-component>
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "原始嵌入测试 {} 失败", i + 1);
            self.save_test_output(&format!("raw_embedding_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 原始嵌入测试通过");
        Ok(())
    }
    
    /// 14. 导入系统测试
    fn test_import_system(&mut self) -> Result<()> {
        println!("📥 测试导入系统...");
        
        let test_cases = vec![
            // CHTL文件导入
            r#"
            [Import] "components.chtl" as components;
            [Import] "styles.chtl" as styles;
            "#,
            
            // HTML文件导入
            r#"
            [Import] "header.html" as header;
            [Import] "footer.html" as footer;
            "#,
            
            // CSS文件导入
            r#"
            [Import] "bootstrap.css" as bootstrap;
            [Import] "custom.css" as custom;
            "#,
            
            // JavaScript文件导入
            r#"
            [Import] "jquery.js" as jquery;
            [Import] "app.js" as app;
            "#,
            
            // 精确导入
            r#"
            [Import] "utils.chtl" { Button, Card, Modal } as ui;
            "#,
            
            // 通配符导入
            r#"
            [Import] "components/*.chtl" as all-components;
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "导入系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("import_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 导入系统测试通过");
        Ok(())
    }
    
    /// 15. 命名空间测试
    fn test_namespace_system(&mut self) -> Result<()> {
        println!("🏷️ 测试命名空间系统...");
        
        let test_cases = vec![
            // 基本命名空间
            r#"
            [Namespace] ui {
                Button {
                    class: "btn";
                    text: "Button";
                }
                
                Card {
                    class: "card";
                    text: "Card";
                }
            }
            "#,
            
            // 嵌套命名空间
            r#"
            [Namespace] components {
                [Namespace] forms {
                    Input {
                        type: "text";
                        class: "form-control";
                    }
                    
                    TextArea {
                        class: "form-control";
                        rows: 4;
                    }
                }
            }
            "#,
            
            // 命名空间导入
            r#"
            [Namespace] my-app {
                [Import] "ui.chtl" as ui;
                [Import] "utils.chtl" as utils;
                
                div {
                    ui.Button { text: "Click me"; }
                    utils.Alert { text: "Hello"; }
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "命名空间测试 {} 失败", i + 1);
            self.save_test_output(&format!("namespace_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 命名空间测试通过");
        Ok(())
    }
    
    /// 16. 约束系统测试
    fn test_constraint_system(&mut self) -> Result<()> {
        println!("🔒 测试约束系统...");
        
        let test_cases = vec![
            // 精确约束
            r#"
            [Constraint] except Button {
                // 除了Button之外的所有元素都不能使用这些属性
                data-sensitive: "true";
            }
            "#,
            
            // 类型约束
            r#"
            [Constraint] except [Custom] {
                // 除了自定义元素之外的所有元素都不能使用这些属性
                data-internal: "true";
            }
            "#,
            
            // 全局约束
            r#"
            [Constraint] except * {
                // 全局约束，所有元素都不能使用这些属性
                data-deprecated: "true";
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "约束系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("constraint_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 约束系统测试通过");
        Ok(())
    }
    
    /// 17. 配置组测试
    fn test_configuration_group(&mut self) -> Result<()> {
        println!("⚙️ 测试配置组...");
        
        let test_cases = vec![
            // 基本配置
            r#"
            [Configuration] {
                HTML5_DOCTYPE: true;
                DEBUG_MODE: false;
                INDEX_INITIAL_COUNT: 0;
            }
            "#,
            
            // 命名配置
            r#"
            [Configuration] production {
                DEBUG_MODE: false;
                MINIFY_OUTPUT: true;
                COMPRESS_ASSETS: true;
            }
            
            [Configuration] development {
                DEBUG_MODE: true;
                MINIFY_OUTPUT: false;
                COMPRESS_ASSETS: false;
            }
            "#,
            
            // 导入配置
            r#"
            [Configuration] {
                [Import] "base-config.chtl" as base;
                [Import] "theme-config.chtl" as theme;
            }
            "#,
            
            // 自动化规则
            r#"
            [Configuration] {
                AUTO_ADD_CLASSES: true;
                AUTO_ADD_IDS: true;
                INFER_CONTEXT: true;
                DEFAULT_NAMESPACE: "app";
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "配置组测试 {} 失败", i + 1);
            self.save_test_output(&format!("configuration_group_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 配置组测试通过");
        Ok(())
    }
    
    /// 18. CHTL JS扩展测试
    fn test_chtl_js_extension(&mut self) -> Result<()> {
        println!("⚡ 测试CHTL JS扩展...");
        
        let test_cases = vec![
            // 虚拟对象
            r#"
            script {
                vir MyComponent {
                    name: "MyComponent";
                    render: function() { return this.name; };
                    methods: {
                        greet: function() { return "Hello " + this.name; };
                    };
                }
            }
            "#,
            
            // 事件监听
            r#"
            script {
                listen .button {
                    click: function() { console.log("Button clicked!"); };
                    mouseover: function() { this.style.backgroundColor = "red"; };
                }
            }
            "#,
            
            // 事件委托
            r#"
            script {
                delegate .container {
                    target: .button, .link;
                    click: function() { console.log("Delegated click"); };
                }
            }
            "#,
            
            // 动画
            r#"
            script {
                animate fadeIn {
                    target: .fade-element;
                    duration: 1000;
                    easing: ease-in-out;
                    begin: { opacity: 0; };
                    end: { opacity: 1; };
                }
            }
            "#,
            
            // 路由
            r#"
            script {
                router {
                    url: "/";
                    page: "home";
                    url: "/about";
                    page: "about";
                    mode: "history";
                }
            }
            "#,
            
            // 文件加载器
            r#"
            script {
                fileloader {
                    load: "jquery.js", "bootstrap.js";
                    async: true;
                    cache: true;
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "CHTL JS扩展测试 {} 失败", i + 1);
            self.save_test_output(&format!("chtl_js_extension_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ CHTL JS扩展测试通过");
        Ok(())
    }
    
    /// 19. 模块系统测试
    fn test_module_system(&mut self) -> Result<()> {
        println!("📦 测试模块系统...");
        
        let test_cases = vec![
            // CMOD模块
            r#"
            // 这是一个CMOD模块示例
            [Template] @Element module-button {
                param-text: string;
                
                button {
                    class: "module-btn";
                    text: "{text}";
                }
            }
            "#,
            
            // CJMOD模块
            r#"
            // 这是一个CJMOD模块示例
            class Syntax {
                function analyze(code) {
                    return "Analyzed: " + code;
                }
            }
            
            class Arg {
                function bind(value) {
                    return "Bound: " + value;
                }
            }
            "#,
            
            // 模块信息
            r#"
            [Info] {
                name: "my-module";
                version: "1.0.0";
                author: "CHTL Team";
                description: "A sample module";
            }
            "#,
            
            // 导出表
            r#"
            [Export] {
                functions: ["Button", "Card", "Modal"];
                variables: ["primaryColor", "secondaryColor"];
                classes: ["Syntax", "Arg"];
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "模块系统测试 {} 失败", i + 1);
            self.save_test_output(&format!("module_system_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 模块系统测试通过");
        Ok(())
    }
    
    /// 20. 高级特性测试
    fn test_advanced_features(&mut self) -> Result<()> {
        println!("🚀 测试高级特性...");
        
        let test_cases = vec![
            // 复杂嵌套结构
            r#"
            html {
                head {
                    title { text: "Advanced CHTL"; }
                    meta { charset: "UTF-8"; }
                    style {
                        body { margin: 0; padding: 20px; }
                        .container { max-width: 1200px; margin: 0 auto; }
                    }
                }
                body {
                    div {
                        class: "container";
                        header {
                            h1 { text: "Welcome to CHTL"; }
                            nav {
                                ul {
                                    li { a { href: "/"; text: "Home"; } }
                                    li { a { href: "/about"; text: "About"; } }
                                    li { a { href: "/contact"; text: "Contact"; } }
                                }
                            }
                        }
                        main {
                            section {
                                class: "hero";
                                h2 { text: "Advanced Features"; }
                                p { text: "CHTL provides powerful features for modern web development."; }
                                button {
                                    class: "btn btn-primary";
                                    text: "Get Started";
                                }
                            }
                        }
                    }
                }
            }
            "#,
            
            // 响应式设计
            r#"
            div {
                class: "responsive-grid";
                style {
                    display: grid;
                    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                    gap: 20px;
                }
                div {
                    class: "card";
                    style {
                        padding: 20px;
                        border: 1px solid #ddd;
                        border-radius: 8px;
                    }
                    h3 { text: "Card 1"; }
                    p { text: "This is a responsive card."; }
                }
                div {
                    class: "card";
                    style {
                        padding: 20px;
                        border: 1px solid #ddd;
                        border-radius: 8px;
                    }
                    h3 { text: "Card 2"; }
                    p { text: "Another responsive card."; }
                }
            }
            "#,
            
            // 交互式组件
            r#"
            div {
                class: "interactive-demo";
                style {
                    padding: 20px;
                    background: #f8f9fa;
                    border-radius: 8px;
                }
                h2 { text: "Interactive Demo"; }
                div {
                    class: "controls";
                    button {
                        id: "btn-1";
                        class: "btn";
                        text: "Button 1";
                    }
                    button {
                        id: "btn-2";
                        class: "btn";
                        text: "Button 2";
                    }
                }
                div {
                    id: "output";
                    class: "output";
                    text: "Click a button to see output";
                }
                script {
                    document.getElementById('btn-1').addEventListener('click', function() {
                        document.getElementById('output').textContent = 'Button 1 clicked!';
                    });
                    document.getElementById('btn-2').addEventListener('click', function() {
                        document.getElementById('output').textContent = 'Button 2 clicked!';
                    });
                }
            }
            "#,
        ];
        
        for (i, source) in test_cases.iter().enumerate() {
            let result = self.compiler.compile(source);
            assert!(result.is_ok(), "高级特性测试 {} 失败", i + 1);
            self.save_test_output(&format!("advanced_features_{}.html", i + 1), &result.unwrap())?;
        }
        
        println!("✅ 高级特性测试通过");
        Ok(())
    }
    
    /// 保存测试输出
    fn save_test_output(&self, filename: &str, content: &str) -> Result<()> {
        let path = Path::new(&self.test_dir).join(filename);
        fs::write(path, content)?;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_comprehensive_features() {
        let mut test_suite = ComprehensiveFeatureTest::new();
        test_suite.run_all_tests().unwrap();
    }
}