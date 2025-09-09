//! CSS module
//! 
//! This module provides CSS compilation and processing functionality for CHTL,
//! including CSS parsing, optimization, and generation.

pub mod css_compiler;
pub mod css_parser;
pub mod css_optimizer;
pub mod css_generator;

use std::collections::HashMap;
use std::path::PathBuf;

/// CSS compilation context
#[derive(Debug, Clone)]
pub struct CSSContext {
    /// Source file path
    pub source_path: PathBuf,
    /// Output file path
    pub output_path: PathBuf,
    /// CSS content
    pub content: String,
    /// CSS rules
    pub rules: Vec<CSSRule>,
    /// CSS variables
    pub variables: HashMap<String, String>,
    /// CSS imports
    pub imports: Vec<PathBuf>,
    /// CSS options
    pub options: CSSOptions,
}

/// CSS rule
#[derive(Debug, Clone)]
pub struct CSSRule {
    /// Selector
    pub selector: String,
    /// Properties
    pub properties: HashMap<String, String>,
    /// Media queries
    pub media_queries: Vec<String>,
    /// Pseudo classes
    pub pseudo_classes: Vec<String>,
    /// Pseudo elements
    pub pseudo_elements: Vec<String>,
    /// Specificity
    pub specificity: u32,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

/// CSS options
#[derive(Debug, Clone)]
pub struct CSSOptions {
    /// Minify output
    pub minify: bool,
    /// Include source maps
    pub source_maps: bool,
    /// Autoprefix
    pub autoprefix: bool,
    /// Remove unused rules
    pub remove_unused: bool,
    /// Optimize selectors
    pub optimize_selectors: bool,
    /// Merge rules
    pub merge_rules: bool,
    /// Compress output
    pub compress: bool,
}

impl Default for CSSOptions {
    fn default() -> Self {
        Self {
            minify: false,
            source_maps: false,
            autoprefix: false,
            remove_unused: false,
            optimize_selectors: false,
            merge_rules: false,
            compress: false,
        }
    }
}

impl CSSContext {
    /// Create a new CSS context
    pub fn new(source_path: PathBuf, output_path: PathBuf) -> Self {
        Self {
            source_path,
            output_path,
            content: String::new(),
            rules: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
            options: CSSOptions::default(),
        }
    }
    
    /// Add a CSS rule
    pub fn add_rule(&mut self, rule: CSSRule) {
        self.rules.push(rule);
    }
    
    /// Add a CSS variable
    pub fn add_variable(&mut self, name: String, value: String) {
        self.variables.insert(name, value);
    }
    
    /// Add a CSS import
    pub fn add_import(&mut self, path: PathBuf) {
        self.imports.push(path);
    }
    
    /// Get CSS variable value
    pub fn get_variable(&self, name: &str) -> Option<&String> {
        self.variables.get(name)
    }
    
    /// Set CSS options
    pub fn set_options(&mut self, options: CSSOptions) {
        self.options = options;
    }
    
    /// Get CSS content
    pub fn get_content(&self) -> &String {
        &self.content
    }
    
    /// Set CSS content
    pub fn set_content(&mut self, content: String) {
        self.content = content;
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
    pub fn get_imports(&self) -> &Vec<PathBuf> {
        &self.imports
    }
    
    /// Get CSS options
    pub fn get_options(&self) -> &CSSOptions {
        &self.options
    }
}

impl CSSRule {
    /// Create a new CSS rule
    pub fn new(selector: String, line: usize, column: usize) -> Self {
        Self {
            selector,
            properties: HashMap::new(),
            media_queries: Vec::new(),
            pseudo_classes: Vec::new(),
            pseudo_elements: Vec::new(),
            specificity: 0,
            line,
            column,
        }
    }
    
    /// Add a property
    pub fn add_property(&mut self, name: String, value: String) {
        self.properties.insert(name, value);
    }
    
    /// Add a media query
    pub fn add_media_query(&mut self, query: String) {
        self.media_queries.push(query);
    }
    
    /// Add a pseudo class
    pub fn add_pseudo_class(&mut self, pseudo_class: String) {
        self.pseudo_classes.push(pseudo_class);
    }
    
    /// Add a pseudo element
    pub fn add_pseudo_element(&mut self, pseudo_element: String) {
        self.pseudo_elements.push(pseudo_element);
    }
    
    /// Calculate specificity
    pub fn calculate_specificity(&mut self) {
        let mut specificity = 0;
        
        // Count IDs
        let id_count = self.selector.matches('#').count();
        specificity += id_count * 1000;
        
        // Count classes and attributes
        let class_count = self.selector.matches('.').count();
        let attr_count = self.selector.matches('[').count();
        specificity += (class_count + attr_count) * 100;
        
        // Count elements
        let element_count = self.selector.matches(|c: char| c.is_alphabetic()).count();
        specificity += element_count;
        
        self.specificity = specificity as u32;
    }
    
    /// Get property value
    pub fn get_property(&self, name: &str) -> Option<&String> {
        self.properties.get(name)
    }
    
    /// Check if rule has property
    pub fn has_property(&self, name: &str) -> bool {
        self.properties.contains_key(name)
    }
    
    /// Get all properties
    pub fn get_properties(&self) -> &HashMap<String, String> {
        &self.properties
    }
    
    /// Get media queries
    pub fn get_media_queries(&self) -> &Vec<String> {
        &self.media_queries
    }
    
    /// Get pseudo classes
    pub fn get_pseudo_classes(&self) -> &Vec<String> {
        &self.pseudo_classes
    }
    
    /// Get pseudo elements
    pub fn get_pseudo_elements(&self) -> &Vec<String> {
        &self.pseudo_elements
    }
    
    /// Get specificity
    pub fn get_specificity(&self) -> u32 {
        self.specificity
    }
    
    /// Get position
    pub fn get_position(&self) -> (usize, usize) {
        (self.line, self.column)
    }
}