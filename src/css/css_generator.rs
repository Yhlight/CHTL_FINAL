//! CSS generator
//! 
//! This module provides CSS generation functionality for CHTL,
//! including CSS output generation, formatting, and minification.

use super::*;
use std::collections::HashMap;

/// CSS generator
#[derive(Debug, Clone)]
pub struct CSSGenerator {
    /// Generator options
    options: CSSGeneratorOptions,
    /// CSS rules
    rules: Vec<CSSRule>,
    /// CSS variables
    variables: HashMap<String, String>,
    /// CSS imports
    imports: Vec<String>,
}

/// CSS generator options
#[derive(Debug, Clone)]
pub struct CSSGeneratorOptions {
    /// Minify output
    pub minify: bool,
    /// Include source maps
    pub source_maps: bool,
    /// Autoprefix
    pub autoprefix: bool,
    /// Format output
    pub format: bool,
    /// Indent size
    pub indent_size: usize,
    /// Line ending
    pub line_ending: String,
    /// Include comments
    pub include_comments: bool,
    /// Sort properties
    pub sort_properties: bool,
}

/// CSS generation result
#[derive(Debug, Clone)]
pub struct CSSGenerationResult {
    /// Generated CSS
    pub css: String,
    /// Source map
    pub source_map: Option<String>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// CSS generation error
#[derive(Debug, Clone)]
pub struct CSSGenerationError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl CSSGenerator {
    /// Create a new CSS generator
    pub fn new() -> Self {
        Self {
            options: CSSGeneratorOptions::default(),
            rules: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Create a new CSS generator with options
    pub fn with_options(options: CSSGeneratorOptions) -> Self {
        Self {
            options,
            rules: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Generate CSS from rules
    pub fn generate(&mut self, rules: Vec<CSSRule>) -> Result<CSSGenerationResult, CSSGenerationError> {
        self.rules = rules;
        
        let mut result = CSSGenerationResult {
            css: String::new(),
            source_map: None,
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        // Generate CSS
        result.css = self.generate_css(&mut result)?;
        
        // Generate source map if requested
        if self.options.source_maps {
            result.source_map = Some(self.generate_source_map());
        }
        
        Ok(result)
    }
    
    /// Generate CSS content
    fn generate_css(&self, result: &mut CSSGenerationResult) -> Result<String, CSSGenerationError> {
        let mut css = String::new();
        
        // Add imports
        for import in &self.imports {
            css.push_str(&format!("@import {};\n", import));
        }
        
        if !self.imports.is_empty() {
            css.push('\n');
        }
        
        // Add variables
        if !self.variables.is_empty() {
            css.push_str(":root {\n");
            
            for (name, value) in &self.variables {
                if self.options.minify {
                    css.push_str(&format!("{}:{};", name, value));
                } else {
                    css.push_str(&format!("  {}: {};\n", name, value));
                }
            }
            
            if self.options.minify {
                css.push('}');
            } else {
                css.push_str("}\n\n");
            }
        }
        
        // Add rules
        for rule in &self.rules {
            css.push_str(&self.generate_rule(rule, result)?);
        }
        
        // Format CSS if requested
        if self.options.format && !self.options.minify {
            css = self.format_css(&css);
        }
        
        Ok(css)
    }
    
    /// Generate a single CSS rule
    fn generate_rule(&self, rule: &CSSRule, result: &mut CSSGenerationResult) -> Result<String, CSSGenerationError> {
        let mut css = String::new();
        
        // Add selector
        css.push_str(&rule.selector);
        
        if self.options.minify {
            css.push('{');
        } else {
            css.push_str(" {\n");
        }
        
        // Add properties
        let properties = if self.options.sort_properties {
            self.sort_properties(&rule.properties)
        } else {
            rule.properties.iter().collect()
        };
        
        for (name, value) in properties {
            if self.options.minify {
                css.push_str(name);
                css.push(':');
                css.push_str(value);
                css.push(';');
            } else {
                css.push_str(&format!("  {}: {};\n", name, value));
            }
        }
        
        if self.options.minify {
            css.push('}');
        } else {
            css.push_str("}\n\n");
        }
        
        Ok(css)
    }
    
    /// Sort properties alphabetically
    fn sort_properties(&self, properties: &HashMap<String, String>) -> Vec<(&String, &String)> {
        let mut props: Vec<_> = properties.iter().collect();
        props.sort_by_key(|(name, _)| *name);
        props
    }
    
    /// Format CSS
    fn format_css(&self, css: &str) -> String {
        let mut formatted = String::new();
        let mut indent_level = 0;
        let indent = " ".repeat(self.options.indent_size);
        
        for line in css.lines() {
            let trimmed = line.trim();
            
            if trimmed.is_empty() {
                formatted.push('\n');
                continue;
            }
            
            if trimmed.ends_with('{') {
                formatted.push_str(&indent.repeat(indent_level));
                formatted.push_str(trimmed);
                formatted.push('\n');
                indent_level += 1;
            } else if trimmed.starts_with('}') {
                indent_level = indent_level.saturating_sub(1);
                formatted.push_str(&indent.repeat(indent_level));
                formatted.push_str(trimmed);
                formatted.push('\n');
            } else {
                formatted.push_str(&indent.repeat(indent_level));
                formatted.push_str(trimmed);
                formatted.push('\n');
            }
        }
        
        formatted
    }
    
    /// Generate source map
    fn generate_source_map(&self) -> String {
        // This is a simplified implementation
        // In a real implementation, you would generate a proper source map
        r#"{"version":3,"sources":["source.css"],"names":[],"mappings":"","file":"output.css"}"#.to_string()
    }
    
    /// Add CSS variable
    pub fn add_variable(&mut self, name: String, value: String) {
        self.variables.insert(name, value);
    }
    
    /// Add CSS import
    pub fn add_import(&mut self, import: String) {
        self.imports.push(import);
    }
    
    /// Set generator options
    pub fn set_options(&mut self, options: CSSGeneratorOptions) {
        self.options = options;
    }
    
    /// Get generator options
    pub fn get_options(&self) -> &CSSGeneratorOptions {
        &self.options
    }
    
    /// Get CSS rules
    pub fn get_rules(&self) -> &Vec<CSSRule> {
        &self.rules
    }
    
    /// Get CSS variables
    pub fn get_variables(&self) -> &HashMap<String, String> {
        &self.variables
    }
    
    /// Get CSS imports
    pub fn get_imports(&self) -> &Vec<String> {
        &self.imports
    }
}

impl Default for CSSGenerator {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CSSGeneratorOptions {
    fn default() -> Self {
        Self {
            minify: false,
            source_maps: false,
            autoprefix: false,
            format: true,
            indent_size: 2,
            line_ending: "\n".to_string(),
            include_comments: true,
            sort_properties: false,
        }
    }
}