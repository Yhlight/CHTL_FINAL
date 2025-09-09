//! AST tests
//! 
//! This module contains tests for the parser and AST system.

use crate::chtl::lexer::*;
use crate::chtl::parser::*;
use crate::chtl::node::*;

/// Run AST tests
pub fn run_tests() -> Result<(), Box<dyn std::error::Error>> {
    println!("Running AST tests...");
    
    test_text_node_parsing();
    test_element_node_parsing();
    test_style_node_parsing();
    test_script_node_parsing();
    
    println!("AST tests passed!");
    Ok(())
}

fn test_text_node_parsing() {
    let lexer = ChtlLexer::new();
    let parser = ChtlParser::new();
    
    let source = r#"text { "Hello World" }"#;
    let tokens = lexer.tokenize(source).unwrap();
    let ast = parser.parse(tokens).unwrap();
    
    assert_eq!(ast.nodes.len(), 1);
    
    if let AstNode::Text(text_node) = &ast.nodes[0] {
        assert_eq!(text_node.content, "Hello World");
    } else {
        panic!("Expected text node");
    }
}

fn test_element_node_parsing() {
    let lexer = ChtlLexer::new();
    let parser = ChtlParser::new();
    
    let source = r#"div { id: "test"; class: "container"; }"#;
    let tokens = lexer.tokenize(source).unwrap();
    let ast = parser.parse(tokens).unwrap();
    
    assert_eq!(ast.nodes.len(), 1);
    
    if let AstNode::Element(element_node) = &ast.nodes[0] {
        assert_eq!(element_node.tag_name, "div");
        assert_eq!(element_node.attributes.len(), 2);
        
        assert_eq!(element_node.attributes[0].name, "id");
        assert_eq!(element_node.attributes[0].value, "test");
        
        assert_eq!(element_node.attributes[1].name, "class");
        assert_eq!(element_node.attributes[1].value, "container");
    } else {
        panic!("Expected element node");
    }
}

fn test_style_node_parsing() {
    let lexer = ChtlLexer::new();
    let parser = ChtlParser::new();
    
    let source = r#"style { color: red; font-size: 16px; }"#;
    let tokens = lexer.tokenize(source).unwrap();
    let ast = parser.parse(tokens).unwrap();
    
    assert_eq!(ast.nodes.len(), 1);
    
    if let AstNode::Style(style_node) = &ast.nodes[0] {
        assert_eq!(style_node.properties.len(), 2);
        
        assert_eq!(style_node.properties[0].name, "color");
        assert_eq!(style_node.properties[0].value, "red");
        
        assert_eq!(style_node.properties[1].name, "font-size");
        assert_eq!(style_node.properties[1].value, "16px");
    } else {
        panic!("Expected style node");
    }
}

fn test_script_node_parsing() {
    let lexer = ChtlLexer::new();
    let parser = ChtlParser::new();
    
    let source = r#"script { console.log("Hello World"); }"#;
    let tokens = lexer.tokenize(source).unwrap();
    let ast = parser.parse(tokens).unwrap();
    
    assert_eq!(ast.nodes.len(), 1);
    
    if let AstNode::Script(script_node) = &ast.nodes[0] {
        assert!(script_node.content.contains("console.log"));
    } else {
        panic!("Expected script node");
    }
}