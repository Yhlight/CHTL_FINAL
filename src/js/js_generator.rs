//! JavaScript generator
//! 
//! This module provides JavaScript generation functionality for CHTL,
//! including JavaScript output generation, formatting, and minification.

use super::*;
use std::collections::HashMap;

/// JavaScript generator
#[derive(Debug, Clone)]
pub struct JSGenerator {
    /// Generator options
    options: JSGeneratorOptions,
    /// JavaScript functions
    functions: Vec<JSFunction>,
    /// JavaScript variables
    variables: HashMap<String, String>,
    /// JavaScript imports
    imports: Vec<String>,
}

/// JavaScript generator options
#[derive(Debug, Clone)]
pub struct JSGeneratorOptions {
    /// Minify output
    pub minify: bool,
    /// Include source maps
    pub source_maps: bool,
    /// Format output
    pub format: bool,
    /// Indent size
    pub indent_size: usize,
    /// Line ending
    pub line_ending: String,
    /// Include comments
    pub include_comments: bool,
    /// Sort functions
    pub sort_functions: bool,
    /// Include semicolons
    pub include_semicolons: bool,
}

/// JavaScript generation result
#[derive(Debug, Clone)]
pub struct JSGenerationResult {
    /// Generated JavaScript
    pub js: String,
    /// Source map
    pub source_map: Option<String>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// JavaScript generation error
#[derive(Debug, Clone)]
pub struct JSGenerationError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl JSGenerator {
    /// Create a new JavaScript generator
    pub fn new() -> Self {
        Self {
            options: JSGeneratorOptions::default(),
            functions: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Create a new JavaScript generator with options
    pub fn with_options(options: JSGeneratorOptions) -> Self {
        Self {
            options,
            functions: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Generate JavaScript from functions
    pub fn generate(&mut self, functions: Vec<JSFunction>) -> Result<JSGenerationResult, JSGenerationError> {
        self.functions = functions;
        
        let mut result = JSGenerationResult {
            js: String::new(),
            source_map: None,
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        // Generate JavaScript
        result.js = self.generate_js(&mut result)?;
        
        // Generate source map if requested
        if self.options.source_maps {
            result.source_map = Some(self.generate_source_map());
        }
        
        Ok(result)
    }
    
    /// Generate JavaScript content
    fn generate_js(&self, result: &mut JSGenerationResult) -> Result<String, JSGenerationError> {
        let mut js = String::new();
        
        // Add imports
        for import in &self.imports {
            js.push_str(&format!("import {};\n", import));
        }
        
        if !self.imports.is_empty() {
            js.push('\n');
        }
        
        // Add variables
        if !self.variables.is_empty() {
            for (name, value) in &self.variables {
                if self.options.minify {
                    js.push_str(&format!("const {}={};", name, value));
                } else {
                    js.push_str(&format!("const {} = {};\n", name, value));
                }
            }
            
            if !self.options.minify {
                js.push('\n');
            }
        }
        
        // Add functions
        let functions = if self.options.sort_functions {
            self.sort_functions(&self.functions)
        } else {
            self.functions.clone()
        };
        
        for function in &functions {
            js.push_str(&self.generate_function(function, result)?);
            js.push('\n');
        }
        
        // Format JavaScript if requested
        if self.options.format && !self.options.minify {
            js = self.format_js(&js);
        }
        
        Ok(js)
    }
    
    /// Generate a single JavaScript function
    fn generate_function(&self, function: &JSFunction, result: &mut JSGenerationResult) -> Result<String, JSGenerationError> {
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
        
        // Add semicolon if requested
        if self.options.include_semicolons {
            js.push(';');
        }
        
        Ok(js)
    }
    
    /// Sort functions alphabetically
    fn sort_functions(&self, functions: &[JSFunction]) -> Vec<JSFunction> {
        let mut funcs: Vec<_> = functions.to_vec();
        funcs.sort_by_key(|func| func.name.clone());
        funcs
    }
    
    /// Format JavaScript
    fn format_js(&self, js: &str) -> String {
        let mut formatted = String::new();
        let mut indent_level = 0;
        let indent = " ".repeat(self.options.indent_size);
        
        for line in js.lines() {
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
        r#"{"version":3,"sources":["source.js"],"names":[],"mappings":"","file":"output.js"}"#.to_string()
    }
    
    /// Add JavaScript variable
    pub fn add_variable(&mut self, name: String, value: String) {
        self.variables.insert(name, value);
    }
    
    /// Add JavaScript import
    pub fn add_import(&mut self, import: String) {
        self.imports.push(import);
    }
    
    /// Set generator options
    pub fn set_options(&mut self, options: JSGeneratorOptions) {
        self.options = options;
    }
    
    /// Get generator options
    pub fn get_options(&self) -> &JSGeneratorOptions {
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
    
    /// Get JavaScript imports
    pub fn get_imports(&self) -> &Vec<String> {
        &self.imports
    }
}

impl Default for JSGenerator {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for JSGeneratorOptions {
    fn default() -> Self {
        Self {
            minify: false,
            source_maps: false,
            format: true,
            indent_size: 2,
            line_ending: "\n".to_string(),
            include_comments: true,
            sort_functions: false,
            include_semicolons: true,
        }
    }
}