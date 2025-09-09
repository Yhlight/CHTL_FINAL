//! Utility functions for CHTL

use crate::error::{ChtlError, Result};
use std::collections::HashMap;
use std::path::{Path, PathBuf};

/// String utilities
pub mod string {
    use super::*;
    
    /// Check if a string is a valid identifier
    pub fn is_valid_identifier(s: &str) -> bool {
        if s.is_empty() {
            return false;
        }
        
        let mut chars = s.chars();
        let first = chars.next().unwrap();
        
        // First character must be alphabetic or underscore
        if !first.is_alphabetic() && first != '_' {
            return false;
        }
        
        // Remaining characters must be alphanumeric, underscore, or hyphen
        chars.all(|c| c.is_alphanumeric() || c == '_' || c == '-')
    }
    
    /// Check if a string is a valid CSS property name
    pub fn is_valid_css_property(s: &str) -> bool {
        if s.is_empty() {
            return false;
        }
        
        // CSS properties can contain letters, numbers, hyphens, and underscores
        s.chars().all(|c| c.is_alphanumeric() || c == '-' || c == '_')
    }
    
    /// Check if a string is a valid CSS value
    pub fn is_valid_css_value(s: &str) -> bool {
        !s.is_empty()
    }
    
    /// Escape HTML special characters
    pub fn escape_html(s: &str) -> String {
        s.chars()
            .map(|c| match c {
                '<' => "&lt;",
                '>' => "&gt;",
                '&' => "&amp;",
                '"' => "&quot;",
                '\'' => "&#39;",
                _ => std::str::from_utf8(&[c as u8]).unwrap_or(""),
            })
            .collect()
    }
    
    /// Unescape HTML entities
    pub fn unescape_html(s: &str) -> String {
        s.replace("&lt;", "<")
            .replace("&gt;", ">")
            .replace("&amp;", "&")
            .replace("&quot;", "\"")
            .replace("&#39;", "'")
    }
    
    /// Check if a string is a valid CSS selector
    pub fn is_valid_css_selector(s: &str) -> bool {
        if s.is_empty() {
            return false;
        }
        
        // Basic validation - can be improved
        !s.contains("  ") && // No double spaces
        !s.starts_with(' ') && // No leading space
        !s.ends_with(' ') // No trailing space
    }
    
    /// Normalize whitespace in a string
    pub fn normalize_whitespace(s: &str) -> String {
        s.split_whitespace().collect::<Vec<&str>>().join(" ")
    }
    
    /// Check if a string is a valid color value
    pub fn is_valid_color(s: &str) -> bool {
        if s.is_empty() {
            return false;
        }
        
        // Check for common color formats
        s.starts_with('#') && s.len() == 7 && s[1..].chars().all(|c| c.is_ascii_hexdigit()) ||
        s.starts_with("rgb(") && s.ends_with(')') ||
        s.starts_with("rgba(") && s.ends_with(')') ||
        s.starts_with("hsl(") && s.ends_with(')') ||
        s.starts_with("hsla(") && s.ends_with(')') ||
        ["red", "green", "blue", "black", "white", "transparent"].contains(&s)
    }
    
    /// Check if a string is a valid CSS unit
    pub fn is_valid_css_unit(s: &str) -> bool {
        ["px", "em", "rem", "%", "vh", "vw", "vmin", "vmax", "pt", "pc", "in", "cm", "mm"].contains(&s)
    }
}

/// File system utilities
pub mod file {
    use super::*;
    use std::fs;
    
    /// Read a file and return its contents
    pub fn read_file(path: &Path) -> Result<String> {
        fs::read_to_string(path)
            .map_err(|e| ChtlError::Io(e))
    }
    
    /// Write contents to a file
    pub fn write_file(path: &Path, contents: &str) -> Result<()> {
        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent)?;
        }
        fs::write(path, contents)
            .map_err(|e| ChtlError::Io(e))
    }
    
    /// Check if a file exists
    pub fn exists(path: &Path) -> bool {
        path.exists()
    }
    
    /// Get file extension
    pub fn get_extension(path: &Path) -> Option<String> {
        path.extension()
            .and_then(|ext| ext.to_str())
            .map(|s| s.to_string())
    }
    
    /// Get file name without extension
    pub fn get_stem(path: &Path) -> Option<String> {
        path.file_stem()
            .and_then(|stem| stem.to_str())
            .map(|s| s.to_string())
    }
    
    /// Resolve a path relative to a base directory
    pub fn resolve_path(base: &Path, relative: &str) -> PathBuf {
        if relative.starts_with('/') || relative.starts_with("\\") {
            // Absolute path
            PathBuf::from(relative)
        } else {
            // Relative path
            base.join(relative)
        }
    }
    
    /// Find files with a specific extension in a directory
    pub fn find_files_with_extension(dir: &Path, extension: &str) -> Result<Vec<PathBuf>> {
        let mut files = Vec::new();
        
        if dir.is_dir() {
            for entry in fs::read_dir(dir)? {
                let entry = entry?;
                let path = entry.path();
                
                if path.is_file() {
                    if let Some(ext) = get_extension(&path) {
                        if ext == extension {
                            files.push(path);
                        }
                    }
                } else if path.is_dir() {
                    let sub_files = find_files_with_extension(&path, extension)?;
                    files.extend(sub_files);
                }
            }
        }
        
        Ok(files)
    }
}

/// Path utilities
pub mod path {
    use super::*;
    
    /// Normalize a path by removing redundant separators and up-level references
    pub fn normalize_path(path: &str) -> String {
        let mut parts = Vec::new();
        
        for part in path.split('/') {
            match part {
                "" | "." => continue,
                ".." => {
                    if !parts.is_empty() {
                        parts.pop();
                    }
                }
                _ => parts.push(part),
            }
        }
        
        if parts.is_empty() {
            ".".to_string()
        } else {
            parts.join("/")
        }
    }
    
    /// Check if a path is absolute
    pub fn is_absolute(path: &str) -> bool {
        path.starts_with('/') || path.starts_with("\\") || 
        (path.len() > 1 && path.chars().nth(1) == Some(':'))
    }
    
    /// Get the directory part of a path
    pub fn dirname(path: &str) -> String {
        if let Some(pos) = path.rfind('/') {
            if pos == 0 {
                "/".to_string()
            } else {
                path[..pos].to_string()
            }
        } else {
            ".".to_string()
        }
    }
    
    /// Get the filename part of a path
    pub fn basename(path: &str) -> String {
        if let Some(pos) = path.rfind('/') {
            path[pos + 1..].to_string()
        } else {
            path.to_string()
        }
    }
    
    /// Join multiple path components
    pub fn join_paths(paths: &[&str]) -> String {
        if paths.is_empty() {
            return ".".to_string();
        }
        
        let mut result = paths[0].to_string();
        
        for path in &paths[1..] {
            if !result.ends_with('/') {
                result.push('/');
            }
            result.push_str(path);
        }
        
        normalize_path(&result)
    }
}

/// CSS utilities
pub mod css {
    use super::*;
    
    /// Parse a CSS property value
    pub fn parse_property_value(value: &str) -> Result<HashMap<String, String>> {
        let mut properties = HashMap::new();
        
        // Split by semicolon
        for rule in value.split(';') {
            let rule = rule.trim();
            if rule.is_empty() {
                continue;
            }
            
            if let Some(colon_pos) = rule.find(':') {
                let property = rule[..colon_pos].trim();
                let value = rule[colon_pos + 1..].trim();
                
                if !property.is_empty() && !value.is_empty() {
                    properties.insert(property.to_string(), value.to_string());
                }
            }
        }
        
        Ok(properties)
    }
    
    /// Validate CSS property and value
    pub fn validate_property(property: &str, value: &str) -> bool {
        string::is_valid_css_property(property) && string::is_valid_css_value(value)
    }
    
    /// Generate CSS selector from element info
    pub fn generate_selector(tag: &str, class: Option<&str>, id: Option<&str>) -> String {
        let mut selector = String::new();
        
        if let Some(id) = id {
            selector.push('#');
            selector.push_str(id);
        }
        
        if let Some(class) = class {
            if !selector.is_empty() {
                selector.push(' ');
            }
            selector.push('.');
            selector.push_str(class);
        }
        
        if !selector.is_empty() {
            selector.push(' ');
        }
        selector.push_str(tag);
        
        selector
    }
    
    /// Check if a CSS value is a valid color
    pub fn is_color_value(value: &str) -> bool {
        string::is_valid_color(value)
    }
    
    /// Check if a CSS value is a valid length
    pub fn is_length_value(value: &str) -> bool {
        if value.is_empty() {
            return false;
        }
        
        // Check if it's a number followed by a unit
        let mut chars = value.chars();
        let mut has_digit = false;
        
        while let Some(c) = chars.next() {
            if c.is_ascii_digit() || c == '.' {
                has_digit = true;
            } else if c.is_alphabetic() {
                // Check if the rest is a valid unit
                let unit: String = chars.collect();
                return has_digit && string::is_valid_css_unit(&unit);
            } else {
                return false;
            }
        }
        
        has_digit
    }
}

/// JavaScript utilities
pub mod javascript {
    use super::*;
    
    /// Check if a string is a valid JavaScript identifier
    pub fn is_valid_identifier(s: &str) -> bool {
        if s.is_empty() {
            return false;
        }
        
        let mut chars = s.chars();
        let first = chars.next().unwrap();
        
        // First character must be letter, underscore, or dollar sign
        if !first.is_alphabetic() && first != '_' && first != '$' {
            return false;
        }
        
        // Remaining characters must be alphanumeric, underscore, or dollar sign
        chars.all(|c| c.is_alphanumeric() || c == '_' || c == '$')
    }
    
    /// Escape JavaScript string
    pub fn escape_string(s: &str) -> String {
        s.chars()
            .map(|c| match c {
                '"' => "\\\"",
                '\\' => "\\\\",
                '\n' => "\\n",
                '\r' => "\\r",
                '\t' => "\\t",
                _ => std::str::from_utf8(&[c as u8]).unwrap_or(""),
            })
            .collect()
    }
    
    /// Check if a string is a valid JavaScript expression
    pub fn is_valid_expression(s: &str) -> bool {
        // Basic validation - can be improved with a proper parser
        !s.is_empty() && 
        s.chars().filter(|&c| c == '(').count() == s.chars().filter(|&c| c == ')').count() &&
        s.chars().filter(|&c| c == '[').count() == s.chars().filter(|&c| c == ']').count() &&
        s.chars().filter(|&c| c == '{').count() == s.chars().filter(|&c| c == '}').count()
    }
}

/// Module utilities
pub mod module {
    use super::*;
    
    /// Parse module path
    pub fn parse_module_path(path: &str) -> Result<(String, Option<String>)> {
        if let Some(at_pos) = path.find('@') {
            let module_name = path[..at_pos].to_string();
            let version = if at_pos + 1 < path.len() {
                Some(path[at_pos + 1..].to_string())
            } else {
                None
            };
            Ok((module_name, version))
        } else {
            Ok((path.to_string(), None))
        }
    }
    
    /// Validate module name
    pub fn is_valid_module_name(name: &str) -> bool {
        !name.is_empty() && 
        name.chars().all(|c| c.is_alphanumeric() || c == '-' || c == '_' || c == '.')
    }
    
    /// Generate module import statement
    pub fn generate_import_statement(module: &str, alias: Option<&str>) -> String {
        if let Some(alias) = alias {
            format!("import {} from '{}';", alias, module)
        } else {
            format!("import '{}';", module)
        }
    }
}

/// Configuration utilities
pub mod config {
    use super::*;
    
    /// Parse configuration value
    pub fn parse_config_value(value: &str) -> Result<serde_json::Value> {
        // Try to parse as JSON first
        if let Ok(json_value) = serde_json::from_str(value) {
            return Ok(json_value);
        }
        
        // If not JSON, treat as string
        Ok(serde_json::Value::String(value.to_string()))
    }
    
    /// Validate configuration key
    pub fn is_valid_config_key(key: &str) -> bool {
        !key.is_empty() && 
        key.chars().all(|c| c.is_alphanumeric() || c == '_')
    }
    
    /// Get default configuration
    pub fn get_default_config() -> HashMap<String, String> {
        let mut config = HashMap::new();
        config.insert("HTML5_DOCTYPE".to_string(), "true".to_string());
        config.insert("DEBUG_MODE".to_string(), "false".to_string());
        config.insert("INDEX_INITIAL_COUNT".to_string(), "0".to_string());
        config
    }
}