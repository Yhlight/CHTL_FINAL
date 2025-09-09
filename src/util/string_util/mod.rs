//! String utilities
//! 
//! This module provides string manipulation functions for CHTL.

use regex::Regex;

/// String utilities
pub struct StringUtil;

impl StringUtil {
    /// Trim whitespace from string
    pub fn trim(s: &str) -> String {
        s.trim().to_string()
    }
    
    /// Check if string starts with whitespace
    pub fn starts_with_whitespace(s: &str) -> bool {
        s.chars().next().map_or(false, |c| c.is_whitespace())
    }
    
    /// Check if string ends with whitespace
    pub fn ends_with_whitespace(s: &str) -> bool {
        s.chars().last().map_or(false, |c| c.is_whitespace())
    }
    
    /// Remove leading whitespace
    pub fn trim_start(s: &str) -> String {
        s.trim_start().to_string()
    }
    
    /// Remove trailing whitespace
    pub fn trim_end(s: &str) -> String {
        s.trim_end().to_string()
    }
    
    /// Split string by whitespace
    pub fn split_whitespace(s: &str) -> Vec<String> {
        s.split_whitespace().map(|s| s.to_string()).collect()
    }
    
    /// Join strings with separator
    pub fn join(strings: &[String], separator: &str) -> String {
        strings.join(separator)
    }
    
    /// Check if string contains only whitespace
    pub fn is_whitespace_only(s: &str) -> bool {
        s.chars().all(|c| c.is_whitespace())
    }
    
    /// Normalize line endings
    pub fn normalize_line_endings(s: &str) -> String {
        s.replace("\r\n", "\n").replace('\r', "\n")
    }
    
    /// Escape regex special characters
    pub fn escape_regex(s: &str) -> String {
        regex::escape(s)
    }
    
    /// Check if string matches pattern
    pub fn matches_pattern(s: &str, pattern: &str) -> bool {
        if let Ok(regex) = Regex::new(pattern) {
            regex.is_match(s)
        } else {
            false
        }
    }
    
    /// Replace all occurrences
    pub fn replace_all(s: &str, from: &str, to: &str) -> String {
        s.replace(from, to)
    }
    
    /// Convert to snake_case
    pub fn to_snake_case(s: &str) -> String {
        let mut result = String::new();
        let mut chars = s.chars().peekable();
        
        while let Some(ch) = chars.next() {
            if ch.is_uppercase() {
                if !result.is_empty() {
                    result.push('_');
                }
                result.push(ch.to_lowercase().next().unwrap());
            } else {
                result.push(ch);
            }
        }
        
        result
    }
    
    /// Convert to camelCase
    pub fn to_camel_case(s: &str) -> String {
        let mut result = String::new();
        let mut chars = s.chars().peekable();
        let mut capitalize_next = false;
        
        while let Some(ch) = chars.next() {
            if ch == '_' {
                capitalize_next = true;
            } else if capitalize_next {
                result.push(ch.to_uppercase().next().unwrap());
                capitalize_next = false;
            } else {
                result.push(ch);
            }
        }
        
        result
    }
}