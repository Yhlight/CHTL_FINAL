//! Test the CHTL lexer
//! 
//! This is a simple test program to verify the lexer works correctly.

use chtl::chtl::{CHTLLexer, TokenType};

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let source = r#"
// CHTL test file
[Template] @Style TestStyle {
    color: red;
    background: blue;
}

div {
    @TestStyle
    text: "Hello World";
    
    span {
        text: 'Nested element';
    }
}
"#.to_string();

    let mut lexer = CHTLLexer::new(source);
    let tokens = lexer.tokenize()?;

    println!("Tokenized {} tokens:", tokens.len());
    
    for (i, token) in tokens.iter().enumerate() {
        println!("{}: {:?} = '{}'", i, token.token_type, token.value);
        
        // Stop after 50 tokens to avoid too much output
        if i >= 50 {
            println!("... (truncated)");
            break;
        }
    }

    // Count different token types
    let mut counts = std::collections::HashMap::new();
    for token in &tokens {
        let type_name = match &token.token_type {
            TokenType::SingleLineComment(_) => "Comment",
            TokenType::MultiLineComment(_) => "MultiComment",
            TokenType::GeneratorComment(_) => "GenComment",
            TokenType::String(_) => "String",
            TokenType::Number(_) => "Number",
            TokenType::Identifier(_) => "Identifier",
            TokenType::LeftBracket => "LeftBracket",
            TokenType::RightBracket => "RightBracket",
            TokenType::LeftBrace => "LeftBrace",
            TokenType::RightBrace => "RightBrace",
            TokenType::LeftParen => "LeftParen",
            TokenType::RightParen => "RightParen",
            TokenType::Colon => "Colon",
            TokenType::Semicolon => "Semicolon",
            TokenType::At => "At",
            TokenType::Template => "Template",
            TokenType::Custom => "Custom",
            _ => "Other",
        };
        *counts.entry(type_name).or_insert(0) += 1;
    }

    println!("\nToken counts:");
    for (token_type, count) in &counts {
        println!("  {}: {}", token_type, count);
    }

    Ok(())
}