//! 简化的CHTL测试

use chtl::ChtlCompiler;
use std::fs;
use std::path::Path;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("🎯 CHTL简化测试");
    println!("================");
    
    let mut compiler = ChtlCompiler::new();
    let test_output_dir = "simple_test_outputs";
    
    // 创建输出目录
    fs::create_dir_all(test_output_dir)?;
    
    // 运行基本测试
    run_basic_tests(&mut compiler, test_output_dir)?;
    
    println!("✅ 基本测试完成！");
    println!("📁 测试输出目录: {}", test_output_dir);
    
    Ok(())
}

/// 运行基本测试
fn run_basic_tests(compiler: &mut ChtlCompiler, output_dir: &str) -> Result<(), Box<dyn std::error::Error>> {
    println!("📝 运行基本测试...");
    
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
        ("comments", r#"
            div {
                // 这是行注释
                text: "Hello";
                /* 这是块注释 */
            }
        "#),
        ("text_nodes", r#"
            div {
                text: "Hello World";
            }
        "#),
        ("literals", r#"
            div {
                class: container;        // 无引号
                id: "main";             // 双引号
                data-value: '123';      // 单引号
            }
        "#),
        ("ce_equivalence", r#"
            div {
                class: "container";     // 冒号语法
                id = "main";           // 等号语法
            }
        "#),
        ("html_elements", r#"
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
        ("style_blocks", r#"
            div {
                style {
                    color: #ff0000;
                    font-size: 16px;
                    margin: 10px;
                }
            }
        "#),
        ("complex_example", r#"
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
                        button {
                            class: "btn btn-primary";
                            text: "Click me";
                        }
                    }
                }
            }
        "#),
    ];
    
    for (name, source) in tests {
        run_single_test(compiler, output_dir, name, source)?;
    }
    
    println!("✅ 基本测试完成");
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