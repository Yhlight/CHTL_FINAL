//! String utilities
//! 
//! This module provides string utility functionality for CHTL,
//! including string manipulation, formatting, and parsing utilities.

use std::collections::HashMap;
use regex::Regex;
use lazy_static::lazy_static;

/// String utilities
#[derive(Debug, Clone)]
pub struct StringUtils {
    /// String patterns
    patterns: HashMap<String, Regex>,
}

/// String utility error
#[derive(Debug, Clone)]
pub struct StringUtilsError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl StringUtils {
    /// Create a new string utils instance
    pub fn new() -> Self {
        let mut utils = Self {
            patterns: HashMap::new(),
        };
        
        // Initialize default patterns
        utils.initialize_patterns();
        
        utils
    }
    
    /// Initialize default patterns
    fn initialize_patterns(&mut self) {
        // HTML patterns
        self.add_pattern("html_tag", r"<[^>]+>");
        self.add_pattern("html_comment", r"<!--.*?-->");
        self.add_pattern("html_doctype", r"<!DOCTYPE[^>]*>");
        
        // CSS patterns
        self.add_pattern("css_rule", r"[^{]+\{[^}]*\}");
        self.add_pattern("css_comment", r"/\*.*?\*/");
        self.add_pattern("css_import", r"@import[^;]+;");
        self.add_pattern("css_media", r"@media[^{]+\{[^}]*\}");
        
        // JavaScript patterns
        self.add_pattern("js_function", r"function\s+\w+\s*\([^)]*\)\s*\{[^}]*\}");
        self.add_pattern("js_arrow_function", r"\w+\s*=>\s*\{[^}]*\}");
        self.add_pattern("js_comment_single", r"//.*");
        self.add_pattern("js_comment_multi", r"/\*.*?\*/");
        self.add_pattern("js_string_double", r#""[^"]*""#);
        self.add_pattern("js_string_single", r"'[^']*'");
        
        // CHTL patterns
        self.add_pattern("chtl_template", r"\[Template\]\s+@\w+\s+\w+");
        self.add_pattern("chtl_custom", r"\[Custom\]\s+@\w+\s+\w+");
        self.add_pattern("chtl_origin", r"\[Origin\]\s+@\w+\s+\w+");
        self.add_pattern("chtl_import", r"\[Import\]\s+@\w+\s+\w+");
        self.add_pattern("chtl_namespace", r"\[Namespace\]\s+\w+");
        self.add_pattern("chtl_configuration", r"\[Configuration\]\s+@\w+\s+\w+");
        
        // CHTL JS patterns
        self.add_pattern("chtl_js_selector", r"\{\{[^}]+\}\}");
        self.add_pattern("chtl_js_virtual", r"vir\s+\w+\s*=");
        self.add_pattern("chtl_js_listen", r"listen\s*\{");
        self.add_pattern("chtl_js_delegate", r"delegate\s*\{");
        self.add_pattern("chtl_js_animate", r"animate\s*\{");
        self.add_pattern("chtl_js_router", r"router\s*\{");
        self.add_pattern("chtl_js_fileloader", r"fileloader\s*\{");
    }
    
    /// Add a pattern
    pub fn add_pattern(&mut self, name: &str, pattern: &str) {
        if let Ok(regex) = Regex::new(pattern) {
            self.patterns.insert(name.to_string(), regex);
        }
    }
    
    /// Get a pattern
    pub fn get_pattern(&self, name: &str) -> Option<&Regex> {
        self.patterns.get(name)
    }
    
    /// Check if string matches pattern
    pub fn matches_pattern(&self, text: &str, pattern_name: &str) -> bool {
        if let Some(pattern) = self.get_pattern(pattern_name) {
            pattern.is_match(text)
        } else {
            false
        }
    }
    
    /// Find all matches of a pattern
    pub fn find_matches(&self, text: &str, pattern_name: &str) -> Vec<String> {
        if let Some(pattern) = self.get_pattern(pattern_name) {
            pattern.find_iter(text)
                .map(|m| m.as_str().to_string())
                .collect()
        } else {
            Vec::new()
        }
    }
    
    /// Replace all matches of a pattern
    pub fn replace_matches(&self, text: &str, pattern_name: &str, replacement: &str) -> String {
        if let Some(pattern) = self.get_pattern(pattern_name) {
            pattern.replace_all(text, replacement).to_string()
        } else {
            text.to_string()
        }
    }
    
    /// Split string by pattern
    pub fn split_by_pattern(&self, text: &str, pattern_name: &str) -> Vec<String> {
        if let Some(pattern) = self.get_pattern(pattern_name) {
            pattern.split(text)
                .map(|s| s.to_string())
                .collect()
        } else {
            vec![text.to_string()]
        }
    }
    
    /// Escape HTML special characters
    pub fn escape_html(&self, text: &str) -> String {
        text.replace('&', "&amp;")
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&#x27;")
    }
    
    /// Unescape HTML special characters
    pub fn unescape_html(&self, text: &str) -> String {
        text.replace("&amp;", "&")
            .replace("&lt;", "<")
            .replace("&gt;", ">")
            .replace("&quot;", "\"")
            .replace("&#x27;", "'")
    }
    
    /// Escape CSS special characters
    pub fn escape_css(&self, text: &str) -> String {
        text.replace('\\', "\\\\")
            .replace('"', "\\\"")
            .replace('\'', "\\'")
            .replace('\n', "\\n")
            .replace('\r', "\\r")
            .replace('\t', "\\t")
    }
    
    /// Unescape CSS special characters
    pub fn unescape_css(&self, text: &str) -> String {
        text.replace("\\\\", "\\")
            .replace("\\\"", "\"")
            .replace("\\'", "'")
            .replace("\\n", "\n")
            .replace("\\r", "\r")
            .replace("\\t", "\t")
    }
    
    /// Escape JavaScript special characters
    pub fn escape_javascript(&self, text: &str) -> String {
        text.replace('\\', "\\\\")
            .replace('"', "\\\"")
            .replace('\'', "\\'")
            .replace('\n', "\\n")
            .replace('\r', "\\r")
            .replace('\t', "\\t")
            .replace('\0', "\\0")
    }
    
    /// Unescape JavaScript special characters
    pub fn unescape_javascript(&self, text: &str) -> String {
        text.replace("\\\\", "\\")
            .replace("\\\"", "\"")
            .replace("\\'", "'")
            .replace("\\n", "\n")
            .replace("\\r", "\r")
            .replace("\\t", "\t")
            .replace("\\0", "\0")
    }
    
    /// Trim whitespace
    pub fn trim_whitespace(&self, text: &str) -> String {
        text.trim().to_string()
    }
    
    /// Trim all whitespace
    pub fn trim_all_whitespace(&self, text: &str) -> String {
        text.chars()
            .filter(|c| !c.is_whitespace())
            .collect()
    }
    
    /// Normalize whitespace
    pub fn normalize_whitespace(&self, text: &str) -> String {
        text.split_whitespace()
            .collect::<Vec<&str>>()
            .join(" ")
    }
    
    /// Convert to lowercase
    pub fn to_lowercase(&self, text: &str) -> String {
        text.to_lowercase()
    }
    
    /// Convert to uppercase
    pub fn to_uppercase(&self, text: &str) -> String {
        text.to_uppercase()
    }
    
    /// Convert to title case
    pub fn to_title_case(&self, text: &str) -> String {
        text.split_whitespace()
            .map(|word| {
                let mut chars = word.chars();
                match chars.next() {
                    None => String::new(),
                    Some(first) => first.to_uppercase().collect::<String>() + &chars.as_str().to_lowercase(),
                }
            })
            .collect::<Vec<String>>()
            .join(" ")
    }
    
    /// Convert to camel case
    pub fn to_camel_case(&self, text: &str) -> String {
        let words: Vec<&str> = text.split_whitespace().collect();
        if words.is_empty() {
            return String::new();
        }
        
        let first_word = words[0].to_lowercase();
        let rest_words: String = words[1..]
            .iter()
            .map(|word| {
                let mut chars = word.chars();
                match chars.next() {
                    None => String::new(),
                    Some(first) => first.to_uppercase().collect::<String>() + &chars.as_str().to_lowercase(),
                }
            })
            .collect();
        
        first_word + &rest_words
    }
    
    /// Convert to snake case
    pub fn to_snake_case(&self, text: &str) -> String {
        text.split_whitespace()
            .map(|word| word.to_lowercase())
            .collect::<Vec<String>>()
            .join("_")
    }
    
    /// Convert to kebab case
    pub fn to_kebab_case(&self, text: &str) -> String {
        text.split_whitespace()
            .map(|word| word.to_lowercase())
            .collect::<Vec<String>>()
            .join("-")
    }
    
    /// Check if string is empty or whitespace
    pub fn is_empty_or_whitespace(&self, text: &str) -> bool {
        text.trim().is_empty()
    }
    
    /// Check if string is numeric
    pub fn is_numeric(&self, text: &str) -> bool {
        text.parse::<f64>().is_ok()
    }
    
    /// Check if string is integer
    pub fn is_integer(&self, text: &str) -> bool {
        text.parse::<i64>().is_ok()
    }
    
    /// Check if string is boolean
    pub fn is_boolean(&self, text: &str) -> bool {
        matches!(text.to_lowercase().as_str(), "true" | "false")
    }
    
    /// Check if string is URL
    pub fn is_url(&self, text: &str) -> bool {
        text.starts_with("http://") || text.starts_with("https://") || text.starts_with("ftp://")
    }
    
    /// Check if string is email
    pub fn is_email(&self, text: &str) -> bool {
        self.matches_pattern(text, "email")
    }
    
    /// Check if string is valid identifier
    pub fn is_valid_identifier(&self, text: &str) -> bool {
        if text.is_empty() {
            return false;
        }
        
        let first_char = text.chars().next().unwrap();
        if !first_char.is_ascii_alphabetic() && first_char != '_' {
            return false;
        }
        
        text.chars().all(|c| c.is_ascii_alphanumeric() || c == '_')
    }
    
    /// Generate random string
    pub fn generate_random_string(&self, length: usize) -> String {
        use std::collections::hash_map::DefaultHasher;
        use std::hash::{Hash, Hasher};
        use std::time::{SystemTime, UNIX_EPOCH};
        
        let mut hasher = DefaultHasher::new();
        SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_nanos().hash(&mut hasher);
        let hash = hasher.finish();
        
        let chars: Vec<char> = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789".chars().collect();
        let mut result = String::new();
        
        for i in 0..length {
            let index = (hash >> (i * 6)) as usize % chars.len();
            result.push(chars[index]);
        }
        
        result
    }
    
    /// Format string with placeholders
    pub fn format_string(&self, template: &str, values: &HashMap<String, String>) -> String {
        let mut result = template.to_string();
        
        for (key, value) in values {
            let placeholder = format!("{{{}}}", key);
            result = result.replace(&placeholder, value);
        }
        
        result
    }
    
    /// Parse key-value pairs
    pub fn parse_key_value_pairs(&self, text: &str, separator: &str) -> HashMap<String, String> {
        let mut pairs = HashMap::new();
        
        for line in text.lines() {
            if let Some(separator_pos) = line.find(separator) {
                let key = line[..separator_pos].trim().to_string();
                let value = line[separator_pos + separator.len()..].trim().to_string();
                pairs.insert(key, value);
            }
        }
        
        pairs
    }
    
    /// Join strings with separator
    pub fn join_strings(&self, strings: &[String], separator: &str) -> String {
        strings.join(separator)
    }
    
    /// Split string by separator
    pub fn split_string(&self, text: &str, separator: &str) -> Vec<String> {
        text.split(separator)
            .map(|s| s.to_string())
            .collect()
    }
    
    /// Remove duplicates from string vector
    pub fn remove_duplicates(&self, strings: &[String]) -> Vec<String> {
        let mut seen = std::collections::HashSet::new();
        strings.iter()
            .filter(|s| seen.insert(s))
            .cloned()
            .collect()
    }
    
    /// Sort strings
    pub fn sort_strings(&self, strings: &mut [String]) {
        strings.sort();
    }
    
    /// Reverse string
    pub fn reverse_string(&self, text: &str) -> String {
        text.chars().rev().collect()
    }
    
    /// Count occurrences of substring
    pub fn count_occurrences(&self, text: &str, substring: &str) -> usize {
        text.matches(substring).count()
    }
    
    /// Check if string starts with prefix
    pub fn starts_with(&self, text: &str, prefix: &str) -> bool {
        text.starts_with(prefix)
    }
    
    /// Check if string ends with suffix
    pub fn ends_with(&self, text: &str, suffix: &str) -> bool {
        text.ends_with(suffix)
    }
    
    /// Check if string contains substring
    pub fn contains(&self, text: &str, substring: &str) -> bool {
        text.contains(substring)
    }
    
    /// Get string length
    pub fn get_length(&self, text: &str) -> usize {
        text.len()
    }
    
    /// Get character count
    pub fn get_char_count(&self, text: &str) -> usize {
        text.chars().count()
    }
    
    /// Get word count
    pub fn get_word_count(&self, text: &str) -> usize {
        text.split_whitespace().count()
    }
    
    /// Get line count
    pub fn get_line_count(&self, text: &str) -> usize {
        text.lines().count()
    }
}

impl Default for StringUtils {
    fn default() -> Self {
        Self::new()
    }
}