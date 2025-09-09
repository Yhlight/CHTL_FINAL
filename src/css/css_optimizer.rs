//! CSS optimizer
//! 
//! This module provides CSS optimization functionality for CHTL,
//! including CSS minification, rule merging, and selector optimization.

use super::*;
use std::collections::HashMap;

/// CSS optimizer
#[derive(Debug, Clone)]
pub struct CSSOptimizer {
    /// Optimizer options
    options: CSSOptimizerOptions,
    /// CSS rules
    rules: Vec<CSSRule>,
    /// CSS variables
    variables: HashMap<String, String>,
}

/// CSS optimizer options
#[derive(Debug, Clone)]
pub struct CSSOptimizerOptions {
    /// Minify output
    pub minify: bool,
    /// Remove unused rules
    pub remove_unused: bool,
    /// Merge rules
    pub merge_rules: bool,
    /// Optimize selectors
    pub optimize_selectors: bool,
    /// Remove duplicates
    pub remove_duplicates: bool,
    /// Compress output
    pub compress: bool,
    /// Remove comments
    pub remove_comments: bool,
    /// Remove empty rules
    pub remove_empty_rules: bool,
}

/// CSS optimization result
#[derive(Debug, Clone)]
pub struct CSSOptimizationResult {
    /// Optimized CSS
    pub css: String,
    /// Original size
    pub original_size: usize,
    /// Optimized size: usize,
    pub optimized_size: usize,
    /// Compression ratio
    pub compression_ratio: f64,
    /// Optimizations applied
    pub optimizations: Vec<String>,
}

impl CSSOptimizer {
    /// Create a new CSS optimizer
    pub fn new() -> Self {
        Self {
            options: CSSOptimizerOptions::default(),
            rules: Vec::new(),
            variables: HashMap::new(),
        }
    }
    
    /// Create a new CSS optimizer with options
    pub fn with_options(options: CSSOptimizerOptions) -> Self {
        Self {
            options,
            rules: Vec::new(),
            variables: HashMap::new(),
        }
    }
    
    /// Optimize CSS rules
    pub fn optimize(&mut self, rules: Vec<CSSRule>) -> Result<CSSOptimizationResult, CSSOptimizationError> {
        self.rules = rules;
        let original_size = self.calculate_total_size();
        let mut optimizations = Vec::new();
        
        // Remove empty rules
        if self.options.remove_empty_rules {
            self.remove_empty_rules();
            optimizations.push("Removed empty rules".to_string());
        }
        
        // Remove duplicates
        if self.options.remove_duplicates {
            self.remove_duplicates();
            optimizations.push("Removed duplicate rules".to_string());
        }
        
        // Merge rules
        if self.options.merge_rules {
            self.merge_rules();
            optimizations.push("Merged rules with same selectors".to_string());
        }
        
        // Optimize selectors
        if self.options.optimize_selectors {
            self.optimize_selectors();
            optimizations.push("Optimized selectors".to_string());
        }
        
        // Remove unused rules
        if self.options.remove_unused {
            self.remove_unused_rules();
            optimizations.push("Removed unused rules".to_string());
        }
        
        // Generate optimized CSS
        let css = self.generate_css()?;
        let optimized_size = css.len();
        let compression_ratio = if original_size > 0 {
            (original_size - optimized_size) as f64 / original_size as f64
        } else {
            0.0
        };
        
        Ok(CSSOptimizationResult {
            css,
            original_size,
            optimized_size,
            compression_ratio,
            optimizations,
        })
    }
    
    /// Remove empty rules
    fn remove_empty_rules(&mut self) {
        self.rules.retain(|rule| !rule.properties.is_empty());
    }
    
    /// Remove duplicate rules
    fn remove_duplicates(&mut self) {
        let mut seen = HashMap::new();
        let mut unique_rules = Vec::new();
        
        for rule in self.rules.drain(..) {
            let key = format!("{}:{}", rule.selector, self.serialize_properties(&rule.properties));
            
            if !seen.contains_key(&key) {
                seen.insert(key, true);
                unique_rules.push(rule);
            }
        }
        
        self.rules = unique_rules;
    }
    
    /// Merge rules with same selectors
    fn merge_rules(&mut self) {
        let mut rule_map: HashMap<String, CSSRule> = HashMap::new();
        
        for rule in self.rules.drain(..) {
            if let Some(existing_rule) = rule_map.get_mut(&rule.selector) {
                // Merge properties
                for (name, value) in rule.properties {
                    existing_rule.add_property(name, value);
                }
                
                // Merge media queries
                for media_query in rule.media_queries {
                    if !existing_rule.media_queries.contains(&media_query) {
                        existing_rule.media_queries.push(media_query);
                    }
                }
                
                // Merge pseudo classes
                for pseudo_class in rule.pseudo_classes {
                    if !existing_rule.pseudo_classes.contains(&pseudo_class) {
                        existing_rule.pseudo_classes.push(pseudo_class);
                    }
                }
                
                // Merge pseudo elements
                for pseudo_element in rule.pseudo_elements {
                    if !existing_rule.pseudo_elements.contains(&pseudo_element) {
                        existing_rule.pseudo_elements.push(pseudo_element);
                    }
                }
            } else {
                rule_map.insert(rule.selector.clone(), rule);
            }
        }
        
        self.rules = rule_map.into_values().collect();
    }
    
    /// Optimize selectors
    fn optimize_selectors(&mut self) {
        for rule in &mut self.rules {
            // Remove redundant spaces
            rule.selector = rule.selector
                .replace("  ", " ")
                .replace(" > ", ">")
                .replace(" + ", "+")
                .replace(" ~ ", "~")
                .trim().to_string();
            
            // Remove redundant universal selectors
            rule.selector = rule.selector
                .replace("* ", "")
                .replace(" *", "")
                .replace("*>", ">")
                .replace(">*", ">");
            
            // Optimize attribute selectors
            rule.selector = rule.selector
                .replace("[class=", ".")
                .replace("[id=", "#");
        }
    }
    
    /// Remove unused rules
    fn remove_unused_rules(&mut self) {
        // This is a simplified implementation
        // In a real implementation, you would analyze the HTML to determine which rules are used
        self.rules.retain(|rule| {
            !rule.selector.starts_with(".") || 
            !rule.selector.contains("unused") ||
            !rule.selector.contains("deprecated") ||
            !rule.selector.contains("old")
        });
    }
    
    /// Generate optimized CSS
    fn generate_css(&self) -> Result<String, CSSOptimizationError> {
        let mut css = String::new();
        
        for rule in &self.rules {
            // Add selector
            css.push_str(&rule.selector);
            
            if self.options.minify {
                css.push('{');
            } else {
                css.push_str(" {\n");
            }
            
            // Add properties
            for (name, value) in &rule.properties {
                if self.options.minify {
                    css.push_str(name);
                    css.push(':');
                    css.push_str(value);
                    css.push(';');
                } else {
                    css.push_str("  ");
                    css.push_str(name);
                    css.push_str(": ");
                    css.push_str(value);
                    css.push_str(";\n");
                }
            }
            
            if self.options.minify {
                css.push('}');
            } else {
                css.push_str("}\n\n");
            }
        }
        
        if self.options.compress {
            css = self.compress_css(&css);
        }
        
        Ok(css)
    }
    
    /// Compress CSS
    fn compress_css(&self, css: &str) -> String {
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
    
    /// Serialize properties for comparison
    fn serialize_properties(&self, properties: &HashMap<String, String>) -> String {
        let mut props: Vec<_> = properties.iter().collect();
        props.sort_by_key(|(name, _)| *name);
        
        props
            .iter()
            .map(|(name, value)| format!("{}:{}", name, value))
            .collect::<Vec<_>>()
            .join(";")
    }
    
    /// Calculate total size of rules
    fn calculate_total_size(&self) -> usize {
        self.rules.iter().map(|rule| {
            rule.selector.len() + 
            rule.properties.iter().map(|(name, value)| name.len() + value.len() + 3).sum::<usize>() +
            rule.media_queries.iter().map(|mq| mq.len()).sum::<usize>() +
            rule.pseudo_classes.iter().map(|pc| pc.len()).sum::<usize>() +
            rule.pseudo_elements.iter().map(|pe| pe.len()).sum::<usize>()
        }).sum()
    }
    
    /// Set optimizer options
    pub fn set_options(&mut self, options: CSSOptimizerOptions) {
        self.options = options;
    }
    
    /// Get optimizer options
    pub fn get_options(&self) -> &CSSOptimizerOptions {
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
}

/// CSS optimization error
#[derive(Debug, Clone)]
pub struct CSSOptimizationError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl Default for CSSOptimizer {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CSSOptimizerOptions {
    fn default() -> Self {
        Self {
            minify: false,
            remove_unused: false,
            merge_rules: false,
            optimize_selectors: false,
            remove_duplicates: false,
            compress: false,
            remove_comments: false,
            remove_empty_rules: false,
        }
    }
}