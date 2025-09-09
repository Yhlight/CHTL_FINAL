//! Utility tests
//! 
//! This module contains tests for utility functions.

use crate::util::*;

/// Run utility tests
pub fn run_tests() -> Result<(), Box<dyn std::error::Error>> {
    println!("Running utility tests...");
    
    test_escape_html();
    test_unescape_html();
    test_normalize_whitespace();
    test_is_valid_identifier();
    test_generate_id();
    
    println!("Utility tests passed!");
    Ok(())
}

fn test_escape_html() {
    let input = r#"<div class="test">Hello & World</div>"#;
    let expected = r#"&lt;div class=&quot;test&quot;&gt;Hello &amp; World&lt;/div&gt;"#;
    let result = Util::escape_html(input);
    assert_eq!(result, expected);
}

fn test_unescape_html() {
    let input = r#"&lt;div class=&quot;test&quot;&gt;Hello &amp; World&lt;/div&gt;"#;
    let expected = r#"<div class="test">Hello & World</div>"#;
    let result = Util::unescape_html(input);
    assert_eq!(result, expected);
}

fn test_normalize_whitespace() {
    let input = "  hello   world  \n  test  ";
    let expected = "hello world test";
    let result = Util::normalize_whitespace(input);
    assert_eq!(result, expected);
}

fn test_is_valid_identifier() {
    assert!(Util::is_valid_identifier("hello"));
    assert!(Util::is_valid_identifier("_hello"));
    assert!(Util::is_valid_identifier("hello123"));
    assert!(Util::is_valid_identifier("_hello_123"));
    
    assert!(!Util::is_valid_identifier(""));
    assert!(!Util::is_valid_identifier("123hello"));
    assert!(!Util::is_valid_identifier("hello-world"));
    assert!(!Util::is_valid_identifier("hello world"));
}

fn test_generate_id() {
    let id1 = Util::generate_id();
    let id2 = Util::generate_id();
    
    assert!(!id1.is_empty());
    assert!(!id2.is_empty());
    assert_ne!(id1, id2);
}