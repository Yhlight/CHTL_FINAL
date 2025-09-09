//! Placeholder manager
//! 
//! This module provides placeholder management functionality for CHTL,
//! including placeholder creation, resolution, and replacement.

use std::collections::HashMap;
use regex::Regex;
use lazy_static::lazy_static;

/// Placeholder manager
#[derive(Debug, Clone)]
pub struct PlaceholderManager {
    /// Placeholder counter
    counter: usize,
    /// Placeholder mappings
    mappings: HashMap<String, String>,
    /// Reverse mappings
    reverse_mappings: HashMap<String, String>,
    /// Placeholder patterns
    patterns: Vec<PlaceholderPattern>,
}

/// Placeholder pattern
#[derive(Debug, Clone)]
pub struct PlaceholderPattern {
    /// Pattern name
    pub name: String,
    /// Pattern regex
    pub regex: Regex,
    /// Pattern type
    pub pattern_type: PlaceholderPatternType,
    /// Pattern metadata
    pub metadata: HashMap<String, String>,
}

/// Placeholder pattern type
#[derive(Debug, Clone, PartialEq)]
pub enum PlaceholderPatternType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Comment,
    String,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
}

/// Placeholder error
#[derive(Debug, Clone)]
pub struct PlaceholderError {
    pub message: String,
    pub position: usize,
    pub suggestion: Option<String>,
}

impl PlaceholderManager {
    /// Create a new placeholder manager
    pub fn new() -> Self {
        let mut manager = Self {
            counter: 0,
            mappings: HashMap::new(),
            reverse_mappings: HashMap::new(),
            patterns: Vec::new(),
        };
        
        // Initialize default patterns
        manager.initialize_patterns();
        
        manager
    }
    
    /// Initialize default patterns
    fn initialize_patterns(&mut self) {
        // CHTL patterns
        self.add_pattern(PlaceholderPattern {
            name: "chtl_template".to_string(),
            regex: Regex::new(r"\[Template\]\s+@\w+\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_custom".to_string(),
            regex: Regex::new(r"\[Custom\]\s+@\w+\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_origin".to_string(),
            regex: Regex::new(r"\[Origin\]\s+@\w+\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_import".to_string(),
            regex: Regex::new(r"\[Import\]\s+@\w+\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_namespace".to_string(),
            regex: Regex::new(r"\[Namespace\]\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_configuration".to_string(),
            regex: Regex::new(r"\[Configuration\]\s+@\w+\s+\w+").unwrap(),
            pattern_type: PlaceholderPatternType::CHTL,
            metadata: HashMap::new(),
        });
        
        // CHTL JS patterns
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_selector".to_string(),
            regex: Regex::new(r"\{\{[^}]+\}\}").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_virtual".to_string(),
            regex: Regex::new(r"vir\s+\w+\s*=").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_listen".to_string(),
            regex: Regex::new(r"listen\s*\{").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_delegate".to_string(),
            regex: Regex::new(r"delegate\s*\{").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_animate".to_string(),
            regex: Regex::new(r"animate\s*\{").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_router".to_string(),
            regex: Regex::new(r"router\s*\{").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "chtl_js_fileloader".to_string(),
            regex: Regex::new(r"fileloader\s*\{").unwrap(),
            pattern_type: PlaceholderPatternType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        // CSS patterns
        self.add_pattern(PlaceholderPattern {
            name: "css_style".to_string(),
            regex: Regex::new(r"<style[^>]*>").unwrap(),
            pattern_type: PlaceholderPatternType::CSS,
            metadata: HashMap::new(),
        });
        
        // JavaScript patterns
        self.add_pattern(PlaceholderPattern {
            name: "javascript_script".to_string(),
            regex: Regex::new(r"<script[^>]*>").unwrap(),
            pattern_type: PlaceholderPatternType::JavaScript,
            metadata: HashMap::new(),
        });
        
        // HTML patterns
        self.add_pattern(PlaceholderPattern {
            name: "html_tag".to_string(),
            regex: Regex::new(r"<[^>]+>").unwrap(),
            pattern_type: PlaceholderPatternType::HTML,
            metadata: HashMap::new(),
        });
        
        // Comment patterns
        self.add_pattern(PlaceholderPattern {
            name: "comment_single".to_string(),
            regex: Regex::new(r"//.*").unwrap(),
            pattern_type: PlaceholderPatternType::Comment,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "comment_multi".to_string(),
            regex: Regex::new(r"/\*.*?\*/").unwrap(),
            pattern_type: PlaceholderPatternType::Comment,
            metadata: HashMap::new(),
        });
        
        // String patterns
        self.add_pattern(PlaceholderPattern {
            name: "string_double".to_string(),
            regex: Regex::new(r#""[^"]*""#).unwrap(),
            pattern_type: PlaceholderPatternType::String,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(PlaceholderPattern {
            name: "string_single".to_string(),
            regex: Regex::new(r"'[^']*'").unwrap(),
            pattern_type: PlaceholderPatternType::String,
            metadata: HashMap::new(),
        });
    }
    
    /// Add a pattern
    pub fn add_pattern(&mut self, pattern: PlaceholderPattern) {
        self.patterns.push(pattern);
    }
    
    /// Create a placeholder for content
    pub fn create_placeholder(&mut self, content: &str) -> String {
        let placeholder = format!("_PLACEHOLDER_{}_", self.counter);
        self.counter += 1;
        
        self.mappings.insert(placeholder.clone(), content.to_string());
        self.reverse_mappings.insert(content.to_string(), placeholder.clone());
        
        placeholder
    }
    
    /// Create a placeholder with type
    pub fn create_placeholder_with_type(&mut self, content: &str, pattern_type: PlaceholderPatternType) -> String {
        let placeholder = format!("_PLACEHOLDER_{}_{:?}_", self.counter, pattern_type);
        self.counter += 1;
        
        self.mappings.insert(placeholder.clone(), content.to_string());
        self.reverse_mappings.insert(content.to_string(), placeholder.clone());
        
        placeholder
    }
    
    /// Resolve a placeholder
    pub fn resolve_placeholder(&self, placeholder: &str) -> Option<&String> {
        self.mappings.get(placeholder)
    }
    
    /// Get placeholder for content
    pub fn get_placeholder(&self, content: &str) -> Option<&String> {
        self.reverse_mappings.get(content)
    }
    
    /// Replace placeholders in content
    pub fn replace_placeholders(&self, content: &str) -> String {
        let mut result = content.to_string();
        
        for (placeholder, original) in &self.mappings {
            result = result.replace(placeholder, original);
        }
        
        result
    }
    
    /// Replace content with placeholders
    pub fn replace_with_placeholders(&mut self, content: &str) -> Result<String, PlaceholderError> {
        let mut result = content.to_string();
        
        for pattern in &self.patterns {
            let matches: Vec<_> = pattern.regex.find_iter(&result).collect();
            
            for mat in matches.iter().rev() {
                let matched_content = &result[mat.start()..mat.end()];
                
                // Check if already has placeholder
                if let Some(placeholder) = self.get_placeholder(matched_content) {
                    result.replace_range(mat.start()..mat.end(), placeholder);
                } else {
                    let placeholder = self.create_placeholder_with_type(matched_content, pattern.pattern_type.clone());
                    result.replace_range(mat.start()..mat.end(), &placeholder);
                }
            }
        }
        
        Ok(result)
    }
    
    /// Replace placeholders with content
    pub fn replace_placeholders_with_content(&self, content: &str) -> String {
        let mut result = content.to_string();
        
        for (placeholder, original) in &self.mappings {
            result = result.replace(placeholder, original);
        }
        
        result
    }
    
    /// Get placeholder mappings
    pub fn get_mappings(&self) -> &HashMap<String, String> {
        &self.mappings
    }
    
    /// Get reverse mappings
    pub fn get_reverse_mappings(&self) -> &HashMap<String, String> {
        &self.reverse_mappings
    }
    
    /// Get patterns
    pub fn get_patterns(&self) -> &[PlaceholderPattern] {
        &self.patterns
    }
    
    /// Clear all placeholders
    pub fn clear(&mut self) {
        self.counter = 0;
        self.mappings.clear();
        self.reverse_mappings.clear();
    }
    
    /// Clear patterns
    pub fn clear_patterns(&mut self) {
        self.patterns.clear();
    }
    
    /// Get placeholder count
    pub fn get_placeholder_count(&self) -> usize {
        self.mappings.len()
    }
    
    /// Get pattern count
    pub fn get_pattern_count(&self) -> usize {
        self.patterns.len()
    }
    
    /// Validate placeholder
    pub fn validate_placeholder(&self, placeholder: &str) -> bool {
        self.mappings.contains_key(placeholder)
    }
    
    /// Validate content
    pub fn validate_content(&self, content: &str) -> bool {
        self.reverse_mappings.contains_key(content)
    }
    
    /// Get placeholder statistics
    pub fn get_statistics(&self) -> PlaceholderStatistics {
        let mut type_counts = HashMap::new();
        
        for pattern in &self.patterns {
            let count = type_counts.get(&pattern.pattern_type).unwrap_or(&0) + 1;
            type_counts.insert(pattern.pattern_type.clone(), count);
        }
        
        PlaceholderStatistics {
            total_placeholders: self.mappings.len(),
            total_patterns: self.patterns.len(),
            type_counts,
        }
    }
}

/// Placeholder statistics
#[derive(Debug, Clone)]
pub struct PlaceholderStatistics {
    pub total_placeholders: usize,
    pub total_patterns: usize,
    pub type_counts: HashMap<PlaceholderPatternType, usize>,
}

impl Default for PlaceholderManager {
    fn default() -> Self {
        Self::new()
    }
}