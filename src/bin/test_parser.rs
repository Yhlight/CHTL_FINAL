//! Test the CHTL parser
//! 
//! This is a simple test program to verify the parser works correctly.

use chtl::{CHTLParser, CHTLLexer, Document, Node, CustomModification, CHTLContext};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let source = r#"[div] {
    [text] "Hello World"
}"#.to_string();

    // Create parser with source code
    let mut parser = CHTLParser::new(source).map_err(|e| format!("Parser creation error: {:?}", e))?;
    let ast = parser.parse().map_err(|e| format!("Parse error: {:?}", e))?;

    println!("Parsed AST with {} children:", ast.children.len());
    
    for (i, child) in ast.children.iter().enumerate() {
        println!("{}: {:?}", i, child);
    }

    // Count different node types
    let mut counts = std::collections::HashMap::new();
    count_nodes(&ast, &mut counts);
    
    println!("\nNode counts:");
    for (node_type, count) in &counts {
        println!("  {}: {}", node_type, count);
    }

    Ok(())
}

fn count_nodes(document: &Document, counts: &mut std::collections::HashMap<String, usize>) {
    for child in &document.children {
        count_node(child, counts);
    }
}

fn count_node(node: &Node, counts: &mut std::collections::HashMap<String, usize>) {
    let type_name = match node {
        Node::Element(_) => "Element",
        Node::Text(_) => "Text",
        Node::Style(_) => "Style",
        Node::Template(_) => "Template",
        Node::Custom(_) => "Custom",
        Node::Import(_) => "Import",
        Node::Namespace(_) => "Namespace",
        Node::Configuration(_) => "Configuration",
        Node::Origin(_) => "Origin",
        Node::Comment(_) => "Comment",
        Node::Script(_) => "Script",
    };
    
    *counts.entry(type_name.to_string()).or_insert(0) += 1;
    
    // Recursively count child nodes
    match node {
        Node::Element(element) => {
            for child in &element.children {
                count_node(child, counts);
            }
        }
        Node::Template(template) => {
            for child in &template.content {
                count_node(child, counts);
            }
        }
        Node::Custom(custom) => {
            for modification in &custom.modifications {
                match modification {
                    CustomModification::AddStyle(style) => {
                        count_node(&Node::Style(style.clone()), counts);
                    }
                    CustomModification::AddElement(element) => {
                        count_node(&Node::Element(element.clone()), counts);
                    }
                    _ => {}
                }
            }
        }
        Node::Namespace(namespace) => {
            for child in &namespace.content {
                count_node(child, counts);
            }
        }
        _ => {}
    }
}