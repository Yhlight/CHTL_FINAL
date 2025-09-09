//! Test script for CHTL compiler

use chtl::ChtlCompiler;
use std::fs;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    println!("Testing CHTL Compiler...");
    
    // Test basic compilation
    let source = r#"html
{
    head
    {
        title
        {
            text: "Hello CHTL";
        }
    }
    
    body
    {
        h1
        {
            text: "Welcome to CHTL!";
        }
        
        p
        {
            text: "This is a basic CHTL document.";
        }
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let html = compiler.compile(source)?;
    
    println!("Generated HTML:");
    println!("{}", html);
    
    // Test with example files
    if let Ok(hello_source) = fs::read_to_string("examples/hello.chtl") {
        println!("\nCompiling hello.chtl...");
        let hello_html = compiler.compile(&hello_source)?;
        println!("Generated HTML from hello.chtl:");
        println!("{}", hello_html);
    }
    
    if let Ok(template_source) = fs::read_to_string("examples/template.chtl") {
        println!("\nCompiling template.chtl...");
        let template_html = compiler.compile(&template_source)?;
        println!("Generated HTML from template.chtl:");
        println!("{}", template_html);
    }
    
    println!("\nCHTL Compiler test completed successfully!");
    Ok(())
}