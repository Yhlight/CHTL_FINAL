//! CHTL特征测试运行器

use chtl::ChtlCompiler;
use std::fs;
use std::path::Path;

/// 简单的特征测试运行器
fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🎯 CHTL特征测试运行器");
    println!("========================");
    
    let mut compiler = ChtlCompiler::new();
    let test_output_dir = "feature_test_outputs";
    
    // 创建输出目录
    fs::create_dir_all(test_output_dir)?;
    
    // 运行所有特征测试
    run_all_feature_tests(&mut compiler, test_output_dir)?;
    
    println!("✅ 所有特征测试完成！");
    println!("📁 测试输出目录: {}", test_output_dir);
    
    Ok(())
}

/// 运行所有特征测试
fn run_all_feature_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    // 1. 基础语法测试
    run_basic_syntax_tests(compiler, output_dir)?;
    
    // 2. 注释系统测试
    run_comment_tests(compiler, output_dir)?;
    
    // 3. 文本节点测试
    run_text_node_tests(compiler, output_dir)?;
    
    // 4. 字面量测试
    run_literal_tests(compiler, output_dir)?;
    
    // 5. CE等价性测试
    run_ce_equivalence_tests(compiler, output_dir)?;
    
    // 6. HTML元素测试
    run_html_element_tests(compiler, output_dir)?;
    
    // 7. 样式系统测试
    run_style_system_tests(compiler, output_dir)?;
    
    // 8. 模板系统测试
    run_template_system_tests(compiler, output_dir)?;
    
    // 9. 自定义系统测试
    run_custom_system_tests(compiler, output_dir)?;
    
    // 10. 原始嵌入测试
    run_raw_embedding_tests(compiler, output_dir)?;
    
    // 11. 导入系统测试
    run_import_system_tests(compiler, output_dir)?;
    
    // 12. 命名空间测试
    run_namespace_tests(compiler, output_dir)?;
    
    // 13. 约束系统测试
    run_constraint_tests(compiler, output_dir)?;
    
    // 14. 配置组测试
    run_configuration_tests(compiler, output_dir)?;
    
    // 15. CHTL JS扩展测试
    run_chtl_js_tests(compiler, output_dir)?;
    
    // 16. 模块系统测试
    run_module_system_tests(compiler, output_dir)?;
    
    // 17. 高级特性测试
    run_advanced_feature_tests(compiler, output_dir)?;
    
    Ok(())
}

/// 基础语法测试
fn run_basic_syntax_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📝 运行基础语法测试...");
    
    let tests = vec![
        ("simple_element", r#"
            div {
                text: "Hello World";
            }
        "#),
        ("nested_elements", r#"
            html {
                head {
                    title { text: "My Page"; }
                }
                body {
                    h1 { text: "Welcome"; }
                    p { text: "This is a paragraph."; }
                }
            }
        "#),
        ("attributes", r#"
            div {
                class: "container";
                id: "main";
                data-value: "123";
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 基础语法测试完成");
    Ok(())
}

/// 注释系统测试
fn run_comment_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("💬 运行注释系统测试...");
    
    let tests = vec![
        ("line_comments", r#"
            div {
                // 这是行注释
                text: "Hello";
            }
        "#),
        ("block_comments", r#"
            div {
                /* 这是块注释
                   可以跨多行 */
                text: "Hello";
            }
        "#),
        ("sql_comments", r#"
            div {
                -- 这是SQL风格注释
                text: "Hello";
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 注释系统测试完成");
    Ok(())
}

/// 文本节点测试
fn run_text_node_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📄 运行文本节点测试...");
    
    let tests = vec![
        ("text_block_syntax", r#"
            div {
                text {
                    content: "Hello World";
                }
            }
        "#),
        ("text_colon_syntax", r#"
            div {
                text: "Hello World";
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 文本节点测试完成");
    Ok(())
}

/// 字面量测试
fn run_literal_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔢 运行字面量测试...");
    
    let tests = vec![
        ("unquoted_literals", r#"
            div {
                class: container;
                id: main;
            }
        "#),
        ("double_quoted_literals", r#"
            div {
                class: "container";
                id: "main";
            }
        "#),
        ("single_quoted_literals", r#"
            div {
                class: 'container';
                id: 'main';
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 字面量测试完成");
    Ok(())
}

/// CE等价性测试
fn run_ce_equivalence_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔄 运行CE等价性测试...");
    
    let tests = vec![
        ("colon_syntax", r#"
            div {
                class: "container";
                id: "main";
            }
        "#),
        ("equals_syntax", r#"
            div {
                class = "container";
                id = "main";
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ CE等价性测试完成");
    Ok(())
}

/// HTML元素测试
fn run_html_element_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🏷️ 运行HTML元素测试...");
    
    let tests = vec![
        ("basic_html_structure", r#"
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
        "#),
        ("form_elements", r#"
            form {
                input { type: "text"; name: "username"; }
                input { type: "password"; name: "password"; }
                button { type: "submit"; text: "Submit"; }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ HTML元素测试完成");
    Ok(())
}

/// 样式系统测试
fn run_style_system_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🎨 运行样式系统测试...");
    
    let tests = vec![
        ("local_style_blocks", r#"
            div {
                style {
                    color: #ff0000;
                    font-size: 16px;
                    margin: 10px;
                }
            }
        "#),
        ("style_selectors", r#"
            div {
                class: "container";
                style {
                    background-color: #f0f0f0;
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 样式系统测试完成");
    Ok(())
}

/// 模板系统测试
fn run_template_system_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📋 运行模板系统测试...");
    
    let tests = vec![
        ("style_group_template", r#"
            [Template] @Style primary-button {
                background-color: #007bff;
                color: white;
                padding: 10px 20px;
                border: none;
                border-radius: 4px;
            }
        "#),
        ("element_template", r#"
            [Template] @Element button {
                param-type: string;
                param-text: string;
                
                button {
                    class: "btn btn-{type}";
                    text: "{text}";
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 模板系统测试完成");
    Ok(())
}

/// 自定义系统测试
fn run_custom_system_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔧 运行自定义系统测试...");
    
    let tests = vec![
        ("custom_style_group", r#"
            [Custom] @Style my-theme {
                primary-color: #ff6b6b;
                secondary-color: #4ecdc4;
                font-family: "Arial", sans-serif;
            }
        "#),
        ("custom_element", r#"
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
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 自定义系统测试完成");
    Ok(())
}

/// 原始嵌入测试
fn run_raw_embedding_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📦 运行原始嵌入测试...");
    
    let tests = vec![
        ("html_embedding", r#"
            div {
                [Origin] @Html {
                    <div class="raw-html">
                        <p>This is raw HTML content</p>
                    </div>
                }
            }
        "#),
        ("css_embedding", r#"
            div {
                [Origin] @Style {
                    .custom-class {
                        background-color: #f0f0f0;
                        padding: 20px;
                    }
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 原始嵌入测试完成");
    Ok(())
}

/// 导入系统测试
fn run_import_system_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📥 运行导入系统测试...");
    
    let tests = vec![
        ("chtl_imports", r#"
            [Import] "components.chtl" as components;
            [Import] "styles.chtl" as styles;
        "#),
        ("html_imports", r#"
            [Import] "header.html" as header;
            [Import] "footer.html" as footer;
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 导入系统测试完成");
    Ok(())
}

/// 命名空间测试
fn run_namespace_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🏷️ 运行命名空间测试...");
    
    let tests = vec![
        ("basic_namespace", r#"
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
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 命名空间测试完成");
    Ok(())
}

/// 约束系统测试
fn run_constraint_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🔒 运行约束系统测试...");
    
    let tests = vec![
        ("precise_constraints", r#"
            [Constraint] except Button {
                data-sensitive: "true";
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 约束系统测试完成");
    Ok(())
}

/// 配置组测试
fn run_configuration_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("⚙️ 运行配置组测试...");
    
    let tests = vec![
        ("basic_configuration", r#"
            [Configuration] {
                HTML5_DOCTYPE: true;
                DEBUG_MODE: false;
                INDEX_INITIAL_COUNT: 0;
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 配置组测试完成");
    Ok(())
}

/// CHTL JS扩展测试
fn run_chtl_js_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("⚡ 运行CHTL JS扩展测试...");
    
    let tests = vec![
        ("virtual_objects", r#"
            script {
                vir MyComponent {
                    name: "MyComponent";
                    render: function() { return this.name; };
                }
            }
        "#),
        ("event_listeners", r#"
            script {
                listen .button {
                    click: function() { console.log("Button clicked!"); };
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ CHTL JS扩展测试完成");
    Ok(())
}

/// 模块系统测试
fn run_module_system_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📦 运行模块系统测试...");
    
    let tests = vec![
        ("cmod_module", r#"
            [Template] @Element module-button {
                param-text: string;
                
                button {
                    class: "module-btn";
                    text: "{text}";
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 模块系统测试完成");
    Ok(())
}

/// 高级特性测试
fn run_advanced_feature_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("🚀 运行高级特性测试...");
    
    let tests = vec![
        ("complex_nested_structure", r#"
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
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 高级特性测试完成");
    Ok(())
}

/// 运行单个测试
fn run_single_test(compiler: &mut ChtlCompiler, output_dir: &str, test_name: &str, source: &str) -> Result<(), Box<dyn std::error::Error>> {
    let result = compiler.compile(source);
    
    if let Ok(html) = result {
        let filename = format!("{}.html", test_name);
        let path = Path::new(output_dir).join(&filename);
        fs::write(path, html)?;
        println!("  ✅ {} - 测试通过", test_name);
    } else {
        println!("  ❌ {} - 测试失败: {:?}", test_name, result.err());
    }
    
    Ok(())
}