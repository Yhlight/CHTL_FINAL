//! Integration tests for CHTL compiler

use chtl::ChtlCompiler;
use std::fs;

#[test]
fn test_basic_compilation() {
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
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<head>"));
    assert!(html.contains("<title>Hello CHTL</title>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<h1>Welcome to CHTL!</h1>"));
    assert!(html.contains("<p>This is a basic CHTL document.</p>"));
}

#[test]
fn test_element_with_attributes() {
    let source = r#"div
{
    id: "main";
    class: "container";
    
    text
    {
        Hello World
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("id=\"main\""));
    assert!(html.contains("class=\"container\""));
    assert!(html.contains("Hello World"));
}

#[test]
fn test_style_block() {
    let source = r#"div
{
    style
    {
        width: 100px;
        height: 200px;
        background-color: red;
    }
    
    text
    {
        Styled div
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<style>"));
    assert!(html.contains("width: 100px"));
    assert!(html.contains("height: 200px"));
    assert!(html.contains("background-color: red"));
    assert!(html.contains("</style>"));
}

#[test]
fn test_script_block() {
    let source = r#"div
{
    script
    {
        console.log("Hello from CHTL!");
    }
    
    text
    {
        Script div
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<script>"));
    assert!(html.contains("console.log(\"Hello from CHTL!\")"));
    assert!(html.contains("</script>"));
}

#[test]
fn test_template_system() {
    let source = r#"[Template] @Style ButtonStyle
{
    padding: 10px 20px;
    border: none;
    border-radius: 5px;
    background-color: #007bff;
    color: white;
    cursor: pointer;
}

[Template] @Element Button
{
    button
    {
        style
        {
            @Style ButtonStyle;
        }
        
        text
        {
            Click me
        }
    }
}

html
{
    body
    {
        @Element Button;
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<button>"));
    assert!(html.contains("Click me"));
    assert!(html.contains("</button>"));
}

#[test]
fn test_custom_element() {
    let source = r#"[Custom] @Element Card
{
    div
    {
        class: "card";
        style
        {
            border: 1px solid #ddd;
            border-radius: 8px;
            padding: 16px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
    }
}

html
{
    body
    {
        @Element Card;
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<div>"));
    assert!(html.contains("class=\"card\""));
}

#[test]
fn test_import_statement() {
    let source = r#"[Import] @Html from "header.html" as Header
[Import] @Style from "styles.css" as Styles
[Import] @JavaScript from "script.js" as Script

html
{
    head
    {
        @Element Header;
        @Element Styles;
    }
    
    body
    {
        @Element Script;
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<head>"));
    assert!(html.contains("<body>"));
}

#[test]
fn test_origin_embedding() {
    let source = r#"[Origin] @Html
{
    <div class="custom-html">
        <p>This is raw HTML content</p>
    </div>
}

html
{
    body
    {
        [Origin] @Html;
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<div class=\"custom-html\">"));
    assert!(html.contains("<p>This is raw HTML content</p>"));
}

#[test]
fn test_namespace() {
    let source = r#"[Namespace] ui
{
    [Custom] @Element Button
    {
        button
        {
            class: "ui-button";
            text
            {
                UI Button
            }
        }
    }
}

html
{
    body
    {
        @Element Button from ui;
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<html>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<button>"));
    assert!(html.contains("class=\"ui-button\""));
    assert!(html.contains("UI Button"));
}

#[test]
fn test_configuration() {
    let source = r#"[Configuration]
{
    HTML5_DOCTYPE = true;
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}

html
{
    head
    {
        title
        {
            text: "Configured Document";
        }
    }
    
    body
    {
        text
        {
            This document has configuration
        }
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    assert!(result.is_ok());
    let html = result.unwrap();
    assert!(html.contains("<!DOCTYPE html>"));
    assert!(html.contains("<html>"));
    assert!(html.contains("<head>"));
    assert!(html.contains("<title>Configured Document</title>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("This document has configuration"));
}

#[test]
fn test_full_compilation() {
    let source = r#"html
{
    head
    {
        title
        {
            text: "Full CHTL Example";
        }
    }
    
    body
    {
        style
        {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 20px;
        }
        
        div
        {
            class: "container";
            style
            {
                max-width: 800px;
                margin: 0 auto;
                background-color: white;
                padding: 20px;
                border-radius: 10px;
            }
            
            h1
            {
                text: "Full CHTL Example";
                style
                {
                    color: #333;
                    text-align: center;
                }
            }
            
            p
            {
                text: "This example demonstrates the full CHTL compilation pipeline.";
                style
                {
                    line-height: 1.6;
                    color: #666;
                }
            }
        }
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile_full(source);
    
    assert!(result.is_ok());
    let (html, css, js) = result.unwrap();
    
    // Check HTML output
    assert!(html.contains("<!DOCTYPE html>"));
    assert!(html.contains("<html>"));
    assert!(html.contains("<head>"));
    assert!(html.contains("<title>Full CHTL Example</title>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<div class=\"container\">"));
    assert!(html.contains("<h1>Full CHTL Example</h1>"));
    assert!(html.contains("<p>This example demonstrates the full CHTL compilation pipeline.</p>"));
    
    // Check CSS output
    assert!(css.contains("font-family: Arial, sans-serif"));
    assert!(css.contains("margin: 0"));
    assert!(css.contains("padding: 20px"));
    assert!(css.contains("max-width: 800px"));
    assert!(css.contains("background-color: white"));
    assert!(css.contains("border-radius: 10px"));
    assert!(css.contains("color: #333"));
    assert!(css.contains("text-align: center"));
    assert!(css.contains("line-height: 1.6"));
    assert!(css.contains("color: #666"));
    
    // Check JS output (should be empty for this example)
    assert!(js.is_empty());
}

#[test]
fn test_error_handling() {
    let source = r#"div
{
    // Missing closing brace
    text
    {
        This should cause an error
    }
"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    // This should fail due to syntax error
    assert!(result.is_err());
}

#[test]
fn test_lexer_error_handling() {
    let source = r#"div
{
    text
    {
        This has an invalid character: @#$%^&*()
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    // This should fail due to lexical error
    assert!(result.is_err());
}

#[test]
fn test_parser_error_handling() {
    let source = r#"div
{
    text
    {
        This is missing a closing brace
    }
}"#;
    
    let compiler = ChtlCompiler::new();
    let result = compiler.compile(source);
    
    // This should fail due to parse error
    assert!(result.is_err());
}