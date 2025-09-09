//! Unit tests for CHTL compiler components

use chtl::lexer::{Lexer, TokenType};
use chtl::parser_simple::Parser;
use chtl::ChtlCompiler;

#[test]
fn test_lexer_basic_tokens() {
    let source = r#"div {
    text: "Hello World";
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    
    assert!(!tokens.is_empty());
    assert_eq!(tokens[0].token_type, TokenType::Identifier("div".to_string()));
    assert_eq!(tokens[1].token_type, TokenType::LeftBrace);
    assert_eq!(tokens[2].token_type, TokenType::Text);
    assert_eq!(tokens[3].token_type, TokenType::Colon);
    assert_eq!(tokens[4].token_type, TokenType::String("Hello World".to_string()));
    assert_eq!(tokens[5].token_type, TokenType::Semicolon);
    assert_eq!(tokens[6].token_type, TokenType::RightBrace);
}

#[test]
fn test_lexer_comments() {
    let source = r#"// This is a line comment
/* This is a block comment */
-- This is a generator comment"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    
    assert_eq!(tokens[0].token_type, TokenType::LineComment);
    assert_eq!(tokens[2].token_type, TokenType::BlockComment);
    assert_eq!(tokens[4].token_type, TokenType::GeneratorComment);
}

#[test]
fn test_lexer_strings() {
    let source = r#"text: "Hello World";
text: 'Single quotes';
text: Unquoted literal;"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    
    assert_eq!(tokens[2].token_type, TokenType::String("Hello World".to_string()));
    assert_eq!(tokens[6].token_type, TokenType::String("Single quotes".to_string()));
    assert_eq!(tokens[10].token_type, TokenType::Identifier("Unquoted".to_string()));
}

#[test]
fn test_lexer_keywords() {
    let source = r#"text style script @Style @Element @Var"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    
    assert_eq!(tokens[0].token_type, TokenType::Text);
    assert_eq!(tokens[2].token_type, TokenType::Style);
    assert_eq!(tokens[4].token_type, TokenType::Script);
    assert_eq!(tokens[6].token_type, TokenType::AtStyle);
    assert_eq!(tokens[8].token_type, TokenType::AtElement);
    assert_eq!(tokens[10].token_type, TokenType::AtVar);
}

#[test]
fn test_lexer_bracket_tokens() {
    let source = r#"[Template] [Custom] [Import] [Origin] [Namespace] [Configuration]"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    
    assert_eq!(tokens[0].token_type, TokenType::Template);
    assert_eq!(tokens[2].token_type, TokenType::Custom);
    assert_eq!(tokens[4].token_type, TokenType::Import);
    assert_eq!(tokens[6].token_type, TokenType::Origin);
    assert_eq!(tokens[8].token_type, TokenType::Namespace);
    assert_eq!(tokens[10].token_type, TokenType::Configuration);
}

#[test]
fn test_parser_simple_document() {
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
            text: "Welcome!";
        }
    }
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Element");
    
    if let Some(element) = ast.children[0].as_element() {
        assert_eq!(element.tag_name, "html");
        assert_eq!(element.children.len(), 2);
        
        // Check head element
        assert_eq!(element.children[0].node_type(), "Element");
        if let Some(head) = element.children[0].as_element() {
            assert_eq!(head.tag_name, "head");
            assert_eq!(head.children.len(), 1);
            
            // Check title element
            assert_eq!(head.children[0].node_type(), "Element");
            if let Some(title) = head.children[0].as_element() {
                assert_eq!(title.tag_name, "title");
                assert_eq!(title.children.len(), 1);
                
                // Check text node
                assert_eq!(title.children[0].node_type(), "Text");
                if let Some(text) = title.children[0].as_text() {
                    assert_eq!(text.content, "Hello CHTL");
                }
            }
        }
        
        // Check body element
        assert_eq!(element.children[1].node_type(), "Element");
        if let Some(body) = element.children[1].as_element() {
            assert_eq!(body.tag_name, "body");
            assert_eq!(body.children.len(), 1);
            
            // Check h1 element
            assert_eq!(body.children[0].node_type(), "Element");
            if let Some(h1) = body.children[0].as_element() {
                assert_eq!(h1.tag_name, "h1");
                assert_eq!(h1.children.len(), 1);
                
                // Check text node
                assert_eq!(h1.children[0].node_type(), "Text");
                if let Some(text) = h1.children[0].as_text() {
                    assert_eq!(text.content, "Welcome!");
                }
            }
        }
    }
}

#[test]
fn test_parser_element_with_attributes() {
    let source = r#"div
{
    id: "main";
    class: "container";
    
    text
    {
        Hello World
    }
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Element");
    
    if let Some(element) = ast.children[0].as_element() {
        assert_eq!(element.tag_name, "div");
        assert_eq!(element.attributes.get("id"), Some(&"main".to_string()));
        assert_eq!(element.attributes.get("class"), Some(&"container".to_string()));
        assert_eq!(element.children.len(), 1);
        
        // Check text node
        assert_eq!(element.children[0].node_type(), "Text");
        if let Some(text) = element.children[0].as_text() {
            assert_eq!(text.content, "Hello World");
            assert!(text.is_literal);
        }
    }
}

#[test]
fn test_parser_style_block() {
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
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Element");
    
    if let Some(element) = ast.children[0].as_element() {
        assert_eq!(element.tag_name, "div");
        assert!(element.style.is_some());
        
        if let Some(style) = element.style.as_ref() {
            assert_eq!(style.properties.get("width"), Some(&chtl::StyleValue::Simple("100px".to_string())));
            assert_eq!(style.properties.get("height"), Some(&chtl::StyleValue::Simple("200px".to_string())));
            assert_eq!(style.properties.get("background-color"), Some(&chtl::StyleValue::Simple("red".to_string())));
        }
        
        assert_eq!(element.children.len(), 1);
        
        // Check text node
        assert_eq!(element.children[0].node_type(), "Text");
        if let Some(text) = element.children[0].as_text() {
            assert_eq!(text.content, "Styled div");
            assert!(text.is_literal);
        }
    }
}

#[test]
fn test_parser_script_block() {
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
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Element");
    
    if let Some(element) = ast.children[0].as_element() {
        assert_eq!(element.tag_name, "div");
        assert!(element.script.is_some());
        
        if let Some(script) = element.script.as_ref() {
            assert!(script.content.contains("console.log"));
            assert!(script.is_chtl_js);
        }
        
        assert_eq!(element.children.len(), 1);
        
        // Check text node
        assert_eq!(element.children[0].node_type(), "Text");
        if let Some(text) = element.children[0].as_text() {
            assert_eq!(text.content, "Script div");
            assert!(text.is_literal);
        }
    }
}

#[test]
fn test_parser_template() {
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
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 2);
    
    // Check first template (ButtonStyle)
    assert_eq!(ast.children[0].node_type(), "Template");
    if let Some(template) = ast.children[0].as_template() {
        assert_eq!(template.template_type, chtl::TemplateType::Style);
        assert_eq!(template.name, "ButtonStyle");
    }
    
    // Check second template (Button)
    assert_eq!(ast.children[1].node_type(), "Template");
    if let Some(template) = ast.children[1].as_template() {
        assert_eq!(template.template_type, chtl::TemplateType::Element);
        assert_eq!(template.name, "Button");
        assert_eq!(template.content.len(), 1);
        
        // Check button element
        assert_eq!(template.content[0].node_type(), "Element");
        if let Some(button) = template.content[0].as_element() {
            assert_eq!(button.tag_name, "button");
            assert!(button.style.is_some());
            assert_eq!(button.children.len(), 1);
            
            // Check text node
            assert_eq!(button.children[0].node_type(), "Text");
            if let Some(text) = button.children[0].as_text() {
                assert_eq!(text.content, "Click me");
                assert!(text.is_literal);
            }
        }
    }
}

#[test]
fn test_parser_custom_element() {
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
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Custom");
    
    if let Some(custom) = ast.children[0].as_custom() {
        assert_eq!(custom.custom_type, chtl::CustomType::Element);
        assert_eq!(custom.name, "Card");
        assert_eq!(custom.content.len(), 1);
        
        // Check div element
        assert_eq!(custom.content[0].node_type(), "Element");
        if let Some(div) = custom.content[0].as_element() {
            assert_eq!(div.tag_name, "div");
            assert_eq!(div.attributes.get("class"), Some(&"card".to_string()));
            assert!(div.style.is_some());
        }
    }
}

#[test]
fn test_parser_import() {
    let source = r#"[Import] @Html from "header.html" as Header
[Import] @Style from "styles.css" as Styles
[Import] @JavaScript from "script.js" as Script"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 3);
    
    // Check first import
    assert_eq!(ast.children[0].node_type(), "Import");
    if let Some(import) = ast.children[0].as_import() {
        assert_eq!(import.import_type, chtl::ImportType::Html);
        assert_eq!(import.path, "header.html");
        assert_eq!(import.alias, Some("Header".to_string()));
    }
    
    // Check second import
    assert_eq!(ast.children[1].node_type(), "Import");
    if let Some(import) = ast.children[1].as_import() {
        assert_eq!(import.import_type, chtl::ImportType::Style);
        assert_eq!(import.path, "styles.css");
        assert_eq!(import.alias, Some("Styles".to_string()));
    }
    
    // Check third import
    assert_eq!(ast.children[2].node_type(), "Import");
    if let Some(import) = ast.children[2].as_import() {
        assert_eq!(import.import_type, chtl::ImportType::JavaScript);
        assert_eq!(import.path, "script.js");
        assert_eq!(import.alias, Some("Script".to_string()));
    }
}

#[test]
fn test_parser_origin() {
    let source = r#"[Origin] @Html
{
    <div class="custom-html">
        <p>This is raw HTML content</p>
    </div>
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Origin");
    
    if let Some(origin) = ast.children[0].as_origin() {
        assert_eq!(origin.origin_type, chtl::OriginType::Html);
        assert!(origin.name.is_none());
        assert!(origin.content.contains("<div class=\"custom-html\">"));
        assert!(origin.content.contains("<p>This is raw HTML content</p>"));
    }
}

#[test]
fn test_parser_namespace() {
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
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert_eq!(ast.children.len(), 1);
    assert_eq!(ast.children[0].node_type(), "Namespace");
    
    if let Some(namespace) = ast.children[0].as_namespace() {
        assert_eq!(namespace.name, "ui");
        assert_eq!(namespace.content.len(), 1);
        assert_eq!(namespace.parent, None);
        
        // Check custom element
        assert_eq!(namespace.content[0].node_type(), "Custom");
        if let Some(custom) = namespace.content[0].as_custom() {
            assert_eq!(custom.custom_type, chtl::CustomType::Element);
            assert_eq!(custom.name, "Button");
        }
    }
}

#[test]
fn test_parser_configuration() {
    let source = r#"[Configuration]
{
    HTML5_DOCTYPE = true;
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let ast = parser.parse().unwrap();
    
    assert!(ast.configuration.is_some());
    
    if let Some(config) = ast.configuration.as_ref() {
        assert_eq!(config.settings.get("HTML5_DOCTYPE"), Some(&"true".to_string()));
        assert_eq!(config.settings.get("DEBUG_MODE"), Some(&"false".to_string()));
        assert_eq!(config.settings.get("INDEX_INITIAL_COUNT"), Some(&"0".to_string()));
    }
}

#[test]
fn test_compiler_full_pipeline() {
    let source = r#"html
{
    head
    {
        title
        {
            text: "Compiler Test";
        }
    }
    
    body
    {
        div
        {
            class: "test-container";
            style
            {
                width: 100%;
                height: 200px;
                background-color: #f0f0f0;
            }
            
            text
            {
                This is a test of the full compilation pipeline
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
    assert!(html.contains("<title>Compiler Test</title>"));
    assert!(html.contains("<body>"));
    assert!(html.contains("<div class=\"test-container\">"));
    assert!(html.contains("This is a test of the full compilation pipeline"));
    
    // Check CSS output
    assert!(css.contains("width: 100%"));
    assert!(css.contains("height: 200px"));
    assert!(css.contains("background-color: #f0f0f0"));
    
    // Check JS output (should be empty for this example)
    assert!(js.is_empty());
}

#[test]
fn test_error_handling_lexical() {
    let source = r#"div
{
    text
    {
        This has an invalid character: @#$%^&*()
    }
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let result = lexer.tokenize();
    
    // This should fail due to lexical error
    assert!(result.is_err());
}

#[test]
fn test_error_handling_syntax() {
    let source = r#"div
{
    text
    {
        This is missing a closing brace
    }
}"#;
    
    let mut lexer = Lexer::new(source.to_string());
    let tokens = lexer.tokenize().unwrap();
    let mut parser = Parser::new(tokens, source.to_string());
    let result = parser.parse();
    
    // This should fail due to syntax error
    assert!(result.is_err());
}

#[test]
fn test_error_handling_compilation() {
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
    
    // This should fail due to compilation error
    assert!(result.is_err());
}