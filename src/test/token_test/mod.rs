//! Token tests
//! 
//! This module contains tests for the lexer and token system.

use crate::chtl::lexer::*;

/// Run token tests
pub fn run_tests() -> Result<(), Box<dyn std::error::Error>> {
    println!("Running token tests...");
    
    test_basic_tokens();
    test_string_tokens();
    test_comment_tokens();
    test_identifier_tokens();
    test_number_tokens();
    
    println!("Token tests passed!");
    Ok(())
}

fn test_basic_tokens() {
    let lexer = ChtlLexer::new();
    let source = "{}[]();:=";
    let tokens = lexer.tokenize(source).unwrap();
    
    let expected_types = vec![
        TokenType::LeftBrace,
        TokenType::RightBrace,
        TokenType::LeftBracket,
        TokenType::RightBracket,
        TokenType::LeftParen,
        TokenType::RightParen,
        TokenType::Semicolon,
        TokenType::Colon,
        TokenType::Equals,
        TokenType::EOF,
    ];
    
    for (i, token) in tokens.iter().enumerate() {
        if i < expected_types.len() {
            assert_eq!(token.token_type, expected_types[i]);
        }
    }
}

fn test_string_tokens() {
    let lexer = ChtlLexer::new();
    let source = r#""hello world" 'test string'"#;
    let tokens = lexer.tokenize(source).unwrap();
    
    let string_tokens: Vec<_> = tokens.iter()
        .filter(|t| matches!(t.token_type, TokenType::String(_)))
        .collect();
    
    assert_eq!(string_tokens.len(), 2);
    
    if let TokenType::String(s) = &string_tokens[0].token_type {
        assert_eq!(s, "hello world");
    }
    
    if let TokenType::String(s) = &string_tokens[1].token_type {
        assert_eq!(s, "test string");
    }
}

fn test_comment_tokens() {
    let lexer = ChtlLexer::new();
    let source = "// line comment\n/* block comment */\n-- generator comment";
    let tokens = lexer.tokenize(source).unwrap();
    
    let comment_tokens: Vec<_> = tokens.iter()
        .filter(|t| matches!(t.token_type, TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment))
        .collect();
    
    assert_eq!(comment_tokens.len(), 3);
}

fn test_identifier_tokens() {
    let lexer = ChtlLexer::new();
    let source = "hello _world test123";
    let tokens = lexer.tokenize(source).unwrap();
    
    let identifier_tokens: Vec<_> = tokens.iter()
        .filter(|t| matches!(t.token_type, TokenType::Identifier(_)))
        .collect();
    
    assert_eq!(identifier_tokens.len(), 3);
    
    if let TokenType::Identifier(s) = &identifier_tokens[0].token_type {
        assert_eq!(s, "hello");
    }
    
    if let TokenType::Identifier(s) = &identifier_tokens[1].token_type {
        assert_eq!(s, "_world");
    }
    
    if let TokenType::Identifier(s) = &identifier_tokens[2].token_type {
        assert_eq!(s, "test123");
    }
}

fn test_number_tokens() {
    let lexer = ChtlLexer::new();
    let source = "123 45.67 0.5";
    let tokens = lexer.tokenize(source).unwrap();
    
    let number_tokens: Vec<_> = tokens.iter()
        .filter(|t| matches!(t.token_type, TokenType::Number(_)))
        .collect();
    
    assert_eq!(number_tokens.len(), 3);
    
    if let TokenType::Number(n) = &number_tokens[0].token_type {
        assert_eq!(n, "123");
    }
    
    if let TokenType::Number(n) = &number_tokens[1].token_type {
        assert_eq!(n, "45.67");
    }
    
    if let TokenType::Number(n) = &number_tokens[2].token_type {
        assert_eq!(n, "0.5");
    }
}