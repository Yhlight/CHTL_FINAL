//! CSS compiler
//! 
//! This module provides CSS compilation functionality for CHTL,
//! including CSS parsing, optimization, and generation.

use super::*;
use std::path::PathBuf;
use std::collections::HashMap;

/// CSS compiler
#[derive(Debug, Clone)]
pub struct CSSCompiler {
    /// Compiler options
    options: CSSOptions,
    /// Source maps
    source_maps: HashMap<String, String>,
    /// CSS variables
    variables: HashMap<String, String>,
    /// CSS imports
    imports: Vec<PathBuf>,
}

/// CSS compilation result
#[derive(Debug, Clone)]
pub struct CSSCompilationResult {
    /// Compiled CSS
    pub css: String,
    /// Source map
    pub source_map: Option<String>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
    /// Dependencies
    pub dependencies: Vec<PathBuf>,
}

/// CSS compilation error
#[derive(Debug, Clone)]
pub struct CSSCompilationError {
    pub message: String,
    pub line: Option<usize>,
    pub column: Option<usize>,
    pub suggestion: Option<String>,
}

impl CSSCompiler {
    /// Create a new CSS compiler
    pub fn new() -> Self {
        Self {
            options: CSSOptions::default(),
            source_maps: HashMap::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Create a new CSS compiler with options
    pub fn with_options(options: CSSOptions) -> Self {
        Self {
            options,
            source_maps: HashMap::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Compile CSS from file
    pub fn compile_file(&mut self, file_path: &PathBuf) -> Result<CSSCompilationResult, CSSCompilationError> {
        let content = std::fs::read_to_string(file_path)
            .map_err(|e| CSSCompilationError {
                message: format!("Failed to read file: {}", e),
                line: None,
                column: None,
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.compile_content(&content, file_path)
    }
    
    /// Compile CSS from content
    pub fn compile_content(&mut self, content: &str, file_path: &PathBuf) -> Result<CSSCompilationResult, CSSCompilationError> {
        let mut result = CSSCompilationResult {
            css: String::new(),
            source_map: None,
            warnings: Vec::new(),
            errors: Vec::new(),
            dependencies: Vec::new(),
        };
        
        // Parse CSS
        let rules = self.parse_css(content, &mut result)?;
        
        // Process variables
        self.process_variables(&rules, &mut result)?;
        
        // Process imports
        self.process_imports(&rules, &mut result)?;
        
        // Optimize CSS
        let optimized_rules = self.optimize_css(&rules, &mut result)?;
        
        // Generate CSS
        result.css = self.generate_css(&optimized_rules, &mut result)?;
        
        // Generate source map if requested
        if self.options.source_maps {
            result.source_map = Some(self.generate_source_map(file_path, &result.css));
        }
        
        Ok(result)
    }
    
    /// Parse CSS content
    fn parse_css(&self, content: &str, result: &mut CSSCompilationResult) -> Result<Vec<CSSRule>, CSSCompilationError> {
        let mut rules = Vec::new();
        let mut current_rule: Option<CSSRule> = None;
        let mut current_selector = String::new();
        let mut current_property = String::new();
        let mut current_value = String::new();
        let mut in_rule = false;
        let mut in_property = false;
        let mut in_value = false;
        let mut in_comment = false;
        let mut line = 1;
        let mut column = 1;
        
        for (i, ch) in content.chars().enumerate() {
            match ch {
                '\n' => {
                    line += 1;
                    column = 1;
                }
                '/' if i + 1 < content.len() && content.chars().nth(i + 1) == Some('*') => {
                    in_comment = true;
                    column += 1;
                }
                '*' if in_comment && i + 1 < content.len() && content.chars().nth(i + 1) == Some('/') => {
                    in_comment = false;
                    column += 1;
                }
                _ if in_comment => {
                    column += 1;
                    continue;
                }
                '{' if !in_rule => {
                    if current_selector.trim().is_empty() {
                        result.errors.push(format!("Empty selector at line {}", line));
                        continue;
                    }
                    
                    current_rule = Some(CSSRule::new(current_selector.trim().to_string(), line, column));
                    in_rule = true;
                    current_selector.clear();
                    column += 1;
                }
                '}' if in_rule => {
                    if let Some(mut rule) = current_rule.take() {
                        rule.calculate_specificity();
                        rules.push(rule);
                    }
                    in_rule = false;
                    in_property = false;
                    in_value = false;
                    current_property.clear();
                    current_value.clear();
                    column += 1;
                }
                ':' if in_rule && !in_property => {
                    in_property = true;
                    in_value = false;
                    column += 1;
                }
                ';' if in_rule && in_property && in_value => {
                    if let Some(ref mut rule) = current_rule {
                        rule.add_property(current_property.trim().to_string(), current_value.trim().to_string());
                    }
                    in_property = false;
                    in_value = false;
                    current_property.clear();
                    current_value.clear();
                    column += 1;
                }
                _ if in_rule && in_property && !in_value => {
                    if ch == ' ' || ch == '\t' {
                        in_value = true;
                    } else {
                        current_property.push(ch);
                    }
                    column += 1;
                }
                _ if in_rule && in_property && in_value => {
                    current_value.push(ch);
                    column += 1;
                }
                _ if !in_rule => {
                    current_selector.push(ch);
                    column += 1;
                }
                _ => {
                    column += 1;
                }
            }
        }
        
        if in_rule {
            result.errors.push("Unclosed rule block".to_string());
        }
        
        if in_comment {
            result.warnings.push("Unclosed comment block".to_string());
        }
        
        Ok(rules)
    }
    
    /// Process CSS variables
    fn process_variables(&mut self, rules: &[CSSRule], result: &mut CSSCompilationResult) -> Result<(), CSSCompilationError> {
        for rule in rules {
            if rule.selector.starts_with(":root") || rule.selector.starts_with("--") {
                for (name, value) in &rule.properties {
                    if name.starts_with("--") {
                        self.variables.insert(name.clone(), value.clone());
                    }
                }
            }
        }
        
        Ok(())
    }
    
    /// Process CSS imports
    fn process_imports(&mut self, rules: &[CSSRule], result: &mut CSSCompilationResult) -> Result<(), CSSCompilationError> {
        for rule in rules {
            if rule.selector == "@import" {
                for (_, value) in &rule.properties {
                    if let Some(import_path) = self.extract_import_path(value) {
                        result.dependencies.push(import_path);
                    }
                }
            }
        }
        
        Ok(())
    }
    
    /// Extract import path from CSS value
    fn extract_import_path(&self, value: &str) -> Option<PathBuf> {
        let value = value.trim();
        if value.starts_with("url(") && value.ends_with(")") {
            let url = &value[4..value.len()-1];
            if url.starts_with('"') && url.ends_with('"') {
                let path = &url[1..url.len()-1];
                return Some(PathBuf::from(path));
            }
        }
        None
    }
    
    /// Optimize CSS
    fn optimize_css(&self, rules: &[CSSRule], result: &mut CSSCompilationResult) -> Result<Vec<CSSRule>, CSSCompilationError> {
        let mut optimized_rules = rules.to_vec();
        
        if self.options.remove_unused {
            optimized_rules = self.remove_unused_rules(optimized_rules, result);
        }
        
        if self.options.merge_rules {
            optimized_rules = self.merge_rules(optimized_rules, result);
        }
        
        if self.options.optimize_selectors {
            optimized_rules = self.optimize_selectors(optimized_rules, result);
        }
        
        Ok(optimized_rules)
    }
    
    /// Remove unused CSS rules
    fn remove_unused_rules(&self, rules: Vec<CSSRule>, result: &mut CSSCompilationResult) -> Vec<CSSRule> {
        // This is a simplified implementation
        // In a real implementation, you would analyze the HTML to determine which rules are used
        rules.into_iter().filter(|rule| {
            !rule.selector.starts_with(".") || 
            !rule.selector.contains("unused") ||
            !rule.selector.contains("deprecated")
        }).collect()
    }
    
    /// Merge CSS rules
    fn merge_rules(&self, rules: Vec<CSSRule>, result: &mut CSSCompilationResult) -> Vec<CSSRule> {
        let mut merged_rules: Vec<CSSRule> = Vec::new();
        let mut rule_map: HashMap<String, CSSRule> = HashMap::new();
        
        for rule in rules {
            if let Some(existing_rule) = rule_map.get_mut(&rule.selector) {
                for (name, value) in rule.properties {
                    existing_rule.add_property(name, value);
                }
            } else {
                rule_map.insert(rule.selector.clone(), rule);
            }
        }
        
        for (_, rule) in rule_map {
            merged_rules.push(rule);
        }
        
        merged_rules
    }
    
    /// Optimize CSS selectors
    fn optimize_selectors(&self, rules: Vec<CSSRule>, result: &mut CSSCompilationResult) -> Vec<CSSRule> {
        rules.into_iter().map(|mut rule| {
            // Remove redundant selectors
            rule.selector = rule.selector
                .replace("  ", " ")
                .replace(" > ", ">")
                .replace(" + ", "+")
                .replace(" ~ ", "~");
            
            rule
        }).collect()
    }
    
    /// Generate CSS from rules
    fn generate_css(&self, rules: &[CSSRule], result: &mut CSSCompilationResult) -> Result<String, CSSCompilationError> {
        let mut css = String::new();
        
        for rule in rules {
            css.push_str(&rule.selector);
            css.push_str(" {\n");
            
            for (name, value) in &rule.properties {
                css.push_str("  ");
                css.push_str(name);
                css.push_str(": ");
                css.push_str(value);
                css.push_str(";\n");
            }
            
            css.push_str("}\n\n");
        }
        
        if self.options.minify {
            css = self.minify_css(&css);
        }
        
        Ok(css)
    }
    
    /// Minify CSS
    fn minify_css(&self, css: &str) -> String {
        css
            .lines()
            .map(|line| line.trim())
            .filter(|line| !line.is_empty())
            .collect::<Vec<&str>>()
            .join("")
            .replace("  ", " ")
            .replace(" {", "{")
            .replace("} ", "}")
            .replace("; ", ";")
            .replace(": ", ":")
    }
    
    /// Generate source map
    fn generate_source_map(&self, file_path: &PathBuf, css: &str) -> String {
        format!(
            r#"{{"version":3,"sources":["{}"],"names":[],"mappings":"","file":"{}.css"}}"#,
            file_path.to_string_lossy(),
            file_path.file_stem().unwrap_or_default().to_string_lossy()
        )
    }
    
    /// Set compiler options
    pub fn set_options(&mut self, options: CSSOptions) {
        self.options = options;
    }
    
    /// Get compiler options
    pub fn get_options(&self) -> &CSSOptions {
        &self.options
    }
    
    /// Get CSS variables
    pub fn get_variables(&self) -> &HashMap<String, String> {
        &self.variables
    }
    
    /// Get CSS imports
    pub fn get_imports(&self) -> &Vec<PathBuf> {
        &self.imports
    }
}

impl Default for CSSCompiler {
    fn default() -> Self {
        Self::new()
    }
}