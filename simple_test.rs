use chtl::{CHTLParser, CHTLLexer};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let source = r#"div {
    text: "Hello World";
}"#.to_string();

    println!("Testing with simple source: {}", source);
    
    let mut parser = CHTLParser::new(source).map_err(|e| format!("Parser creation error: {:?}", e))?;
    let ast = parser.parse().map_err(|e| format!("Parse error: {:?}", e))?;

    println!("Success! Parsed AST with {} children:", ast.children.len());
    
    for (i, child) in ast.children.iter().enumerate() {
        println!("{}: {:?}", i, child);
    }

    Ok(())
}