//! JavaScript optimizer
//! 
//! This module provides JavaScript optimization functionality for CHTL,
//! including JavaScript minification, dead code elimination, and function optimization.

use super::*;
use std::collections::HashMap;

/// JavaScript optimizer
#[derive(Debug, Clone)]
pub struct JSOptimizer {
    /// Optimizer options
    options: JSOptimizerOptions,
    /// JavaScript functions
    functions: Vec<JSFunction>,
    /// JavaScript variables
    variables: HashMap<String, String>,
}

/// JavaScript optimizer options
#[derive(Debug, Clone)]
pub struct JSOptimizerOptions {
    /// Minify output
    pub minify: bool,
    /// Remove unused functions
    pub remove_unused: bool,
    /// Remove unused variables
    pub remove_unused_variables: bool,
    /// Optimize functions
    pub optimize_functions: bool,
    /// Remove duplicates
    pub remove_duplicates: bool,
    /// Compress output
    pub compress: bool,
    /// Remove comments
    pub remove_comments: bool,
    /// Remove empty functions
    pub remove_empty_functions: bool,
    /// Inline small functions
    pub inline_small_functions: bool,
}

/// JavaScript optimization result
#[derive(Debug, Clone)]
pub struct JSOptimizationResult {
    /// Optimized JavaScript
    pub js: String,
    /// Original size
    pub original_size: usize,
    /// Optimized size
    pub optimized_size: usize,
    /// Compression ratio
    pub compression_ratio: f64,
    /// Optimizations applied
    pub optimizations: Vec<String>,
}

/// JavaScript optimization error
#[derive(Debug, Clone)]
pub struct JSOptimizationError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl JSOptimizer {
    /// Create a new JavaScript optimizer
    pub fn new() -> Self {
        Self {
            options: JSOptimizerOptions::default(),
            functions: Vec::new(),
            variables: HashMap::new(),
        }
    }
    
    /// Create a new JavaScript optimizer with options
    pub fn with_options(options: JSOptimizerOptions) -> Self {
        Self {
            options,
            functions: Vec::new(),
            variables: HashMap::new(),
        }
    }
    
    /// Optimize JavaScript functions
    pub fn optimize(&mut self, functions: Vec<JSFunction>) -> Result<JSOptimizationResult, JSOptimizationError> {
        self.functions = functions;
        let original_size = self.calculate_total_size();
        let mut optimizations = Vec::new();
        
        // Remove empty functions
        if self.options.remove_empty_functions {
            self.remove_empty_functions();
            optimizations.push("Removed empty functions".to_string());
        }
        
        // Remove duplicates
        if self.options.remove_duplicates {
            self.remove_duplicates();
            optimizations.push("Removed duplicate functions".to_string());
        }
        
        // Optimize functions
        if self.options.optimize_functions {
            self.optimize_functions();
            optimizations.push("Optimized functions".to_string());
        }
        
        // Inline small functions
        if self.options.inline_small_functions {
            self.inline_small_functions();
            optimizations.push("Inlined small functions".to_string());
        }
        
        // Remove unused functions
        if self.options.remove_unused {
            self.remove_unused_functions();
            optimizations.push("Removed unused functions".to_string());
        }
        
        // Generate optimized JavaScript
        let js = self.generate_js()?;
        let optimized_size = js.len();
        let compression_ratio = if original_size > 0 {
            (original_size - optimized_size) as f64 / original_size as f64
        } else {
            0.0
        };
        
        Ok(JSOptimizationResult {
            js,
            original_size,
            optimized_size,
            compression_ratio,
            optimizations,
        })
    }
    
    /// Remove empty functions
    fn remove_empty_functions(&mut self) {
        self.functions.retain(|func| !func.body.trim().is_empty());
    }
    
    /// Remove duplicate functions
    fn remove_duplicates(&mut self) {
        let mut seen = HashMap::new();
        let mut unique_functions = Vec::new();
        
        for function in self.functions.drain(..) {
            let key = format!("{}:{}:{}", 
                function.name, 
                function.parameters.join(","), 
                function.body
            );
            
            if !seen.contains_key(&key) {
                seen.insert(key, true);
                unique_functions.push(function);
            }
        }
        
        self.functions = unique_functions;
    }
    
    /// Optimize functions
    fn optimize_functions(&mut self) {
        for function in &mut self.functions {
            // Remove unnecessary whitespace
            function.body = function.body
                .lines()
                .map(|line| line.trim())
                .filter(|line| !line.is_empty())
                .collect::<Vec<&str>>()
                .join(" ");
            
            // Remove unnecessary semicolons
            function.body = function.body
                .replace(";;", ";")
                .replace("; ", ";")
                .replace(" ;", ";");
            
            // Remove unnecessary parentheses
            function.body = function.body
                .replace("( ", "(")
                .replace(" )", ")")
                .replace("((", "(")
                .replace("))", ")");
            
            // Remove unnecessary spaces
            function.body = function.body
                .replace("  ", " ")
                .replace(" {", "{")
                .replace("} ", "}")
                .replace(": ", ":")
                .replace(" =", "=")
                .replace("= ", "=");
        }
    }
    
    /// Inline small functions
    fn inline_small_functions(&mut self) {
        let mut inlined_functions = Vec::new();
        
        for function in &self.functions {
            if function.body.len() < 100 && function.parameters.len() <= 2 {
                // Find calls to this function and inline them
                for other_function in &mut self.functions {
                    if other_function.name != function.name {
                        other_function.body = other_function.body
                            .replace(&format!("{}(", function.name), &function.body);
                    }
                }
            } else {
                inlined_functions.push(function.clone());
            }
        }
        
        self.functions = inlined_functions;
    }
    
    /// Remove unused functions
    fn remove_unused_functions(&mut self) {
        // This is a simplified implementation
        // In a real implementation, you would analyze the code to determine which functions are used
        self.functions.retain(|func| {
            !func.name.starts_with("_") || 
            !func.name.contains("unused") ||
            !func.name.contains("deprecated") ||
            func.name == "main" ||
            func.name == "init"
        });
    }
    
    /// Generate optimized JavaScript
    fn generate_js(&self) -> Result<String, JSOptimizationError> {
        let mut js = String::new();
        
        for function in &self.functions {
            js.push_str(&self.generate_function(function)?);
            js.push('\n');
        }
        
        if self.options.minify {
            js = self.minify_js(&js);
        }
        
        Ok(js)
    }
    
    /// Generate a single JavaScript function
    fn generate_function(&self, function: &JSFunction) -> Result<String, JSOptimizationError> {
        let mut js = String::new();
        
        match function.function_type {
            JSFunctionType::Function => {
                js.push_str(&format!("function {}(", function.name));
            }
            JSFunctionType::ArrowFunction => {
                js.push_str(&format!("const {} = (", function.name));
            }
            JSFunctionType::Method => {
                js.push_str(&format!("{}(", function.name));
            }
            JSFunctionType::Constructor => {
                js.push_str(&format!("function {}(", function.name));
            }
            JSFunctionType::Generator => {
                js.push_str(&format!("function* {}(", function.name));
            }
            JSFunctionType::Async => {
                js.push_str(&format!("async function {}(", function.name));
            }
            JSFunctionType::AsyncGenerator => {
                js.push_str(&format!("async function* {}(", function.name));
            }
        }
        
        // Add parameters
        js.push_str(&function.parameters.join(", "));
        js.push_str(") {\n");
        
        // Add body
        js.push_str(&function.body);
        js.push_str("\n}");
        
        Ok(js)
    }
    
    /// Minify JavaScript
    fn minify_js(&self, js: &str) -> String {
        js
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
            .replace(" =", "=")
            .replace("= ", "=")
            .replace("( ", "(")
            .replace(" )", ")")
    }
    
    /// Calculate total size of functions
    fn calculate_total_size(&self) -> usize {
        self.functions.iter().map(|func| {
            func.name.len() + 
            func.parameters.join(",").len() + 
            func.body.len() + 
            10 // Overhead for function syntax
        }).sum()
    }
    
    /// Set optimizer options
    pub fn set_options(&mut self, options: JSOptimizerOptions) {
        self.options = options;
    }
    
    /// Get optimizer options
    pub fn get_options(&self) -> &JSOptimizerOptions {
        &self.options
    }
    
    /// Get JavaScript functions
    pub fn get_functions(&self) -> &Vec<JSFunction> {
        &self.functions
    }
    
    /// Get JavaScript variables
    pub fn get_variables(&self) -> &HashMap<String, String> {
        &self.variables
    }
}

impl Default for JSOptimizer {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for JSOptimizerOptions {
    fn default() -> Self {
        Self {
            minify: false,
            remove_unused: false,
            remove_unused_variables: false,
            optimize_functions: false,
            remove_duplicates: false,
            compress: false,
            remove_comments: false,
            remove_empty_functions: false,
            inline_small_functions: false,
        }
    }
}