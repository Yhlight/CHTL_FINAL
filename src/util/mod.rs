//! Utility modules
//! 
//! This module contains utility functions and helpers.

pub mod file_system;
pub mod zip_util;
pub mod string_util;

/// Common utility functions
pub struct Util;

impl Util {
    /// Escape HTML special characters
    pub fn escape_html(text: &str) -> String {
        text.replace('&', "&amp;")
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&#39;")
    }
    
    /// Unescape HTML special characters
    pub fn unescape_html(text: &str) -> String {
        text.replace("&amp;", "&")
            .replace("&lt;", "<")
            .replace("&gt;", ">")
            .replace("&quot;", "\"")
            .replace("&#39;", "'")
    }
    
    /// Normalize whitespace
    pub fn normalize_whitespace(text: &str) -> String {
        text.split_whitespace().collect::<Vec<&str>>().join(" ")
    }
    
    /// Check if string is a valid identifier
    pub fn is_valid_identifier(ident: &str) -> bool {
        if ident.is_empty() {
            return false;
        }
        
        let first_char = ident.chars().next().unwrap();
        if !first_char.is_alphabetic() && first_char != '_' {
            return false;
        }
        
        ident.chars().all(|c| c.is_alphanumeric() || c == '_')
    }
    
    /// Generate unique identifier
    pub fn generate_id() -> String {
        use uuid::Uuid;
        Uuid::new_v4().to_string()
    }
}