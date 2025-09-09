//! JavaScript module
//! 
//! This module provides JavaScript compilation and processing functionality for CHTL,
//! including JavaScript parsing, optimization, and generation.

pub mod js_compiler;
pub mod js_parser;
pub mod js_optimizer;
pub mod js_generator;

use std::collections::HashMap;
use std::path::PathBuf;

/// JavaScript compilation context
#[derive(Debug, Clone)]
pub struct JSContext {
    /// Source file path
    pub source_path: PathBuf,
    /// Output file path
    pub output_path: PathBuf,
    /// JavaScript content
    pub content: String,
    /// JavaScript functions
    pub functions: Vec<JSFunction>,
    /// JavaScript variables
    pub variables: HashMap<String, String>,
    /// JavaScript imports
    pub imports: Vec<PathBuf>,
    /// JavaScript options
    pub options: JSOptions,
}

/// JavaScript function
#[derive(Debug, Clone)]
pub struct JSFunction {
    /// Function name
    pub name: String,
    /// Function parameters
    pub parameters: Vec<String>,
    /// Function body
    pub body: String,
    /// Function type
    pub function_type: JSFunctionType,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

/// JavaScript function type
#[derive(Debug, Clone, PartialEq)]
pub enum JSFunctionType {
    Function,
    ArrowFunction,
    Method,
    Constructor,
    Generator,
    Async,
    AsyncGenerator,
}

/// JavaScript options
#[derive(Debug, Clone)]
pub struct JSOptions {
    /// Minify output
    pub minify: bool,
    /// Include source maps
    pub source_maps: bool,
    /// Transpile ES6+
    pub transpile: bool,
    /// Remove unused code
    pub remove_unused: bool,
    /// Optimize code
    pub optimize: bool,
    /// Compress output
    pub compress: bool,
    /// Target version
    pub target_version: String,
}

impl Default for JSOptions {
    fn default() -> Self {
        Self {
            minify: false,
            source_maps: false,
            transpile: false,
            remove_unused: false,
            optimize: false,
            compress: false,
            target_version: "ES5".to_string(),
        }
    }
}

impl JSContext {
    /// Create a new JavaScript context
    pub fn new(source_path: PathBuf, output_path: PathBuf) -> Self {
        Self {
            source_path,
            output_path,
            content: String::new(),
            functions: Vec::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
            options: JSOptions::default(),
        }
    }
    
    /// Add a JavaScript function
    pub fn add_function(&mut self, function: JSFunction) {
        self.functions.push(function);
    }
    
    /// Add a JavaScript variable
    pub fn add_variable(&mut self, name: String, value: String) {
        self.variables.insert(name, value);
    }
    
    /// Add a JavaScript import
    pub fn add_import(&mut self, path: PathBuf) {
        self.imports.push(path);
    }
    
    /// Get JavaScript variable value
    pub fn get_variable(&self, name: &str) -> Option<&String> {
        self.variables.get(name)
    }
    
    /// Set JavaScript options
    pub fn set_options(&mut self, options: JSOptions) {
        self.options = options;
    }
    
    /// Get JavaScript content
    pub fn get_content(&self) -> &String {
        &self.content
    }
    
    /// Set JavaScript content
    pub fn set_content(&mut self, content: String) {
        self.content = content;
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
    pub fn get_imports(&self) -> &Vec<PathBuf> {
        &self.imports
    }
    
    /// Get JavaScript options
    pub fn get_options(&self) -> &JSOptions {
        &self.options
    }
}

impl JSFunction {
    /// Create a new JavaScript function
    pub fn new(name: String, function_type: JSFunctionType, line: usize, column: usize) -> Self {
        Self {
            name,
            parameters: Vec::new(),
            body: String::new(),
            function_type,
            line,
            column,
        }
    }
    
    /// Add a parameter
    pub fn add_parameter(&mut self, parameter: String) {
        self.parameters.push(parameter);
    }
    
    /// Set function body
    pub fn set_body(&mut self, body: String) {
        self.body = body;
    }
    
    /// Get function name
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    /// Get function parameters
    pub fn get_parameters(&self) -> &Vec<String> {
        &self.parameters
    }
    
    /// Get function body
    pub fn get_body(&self) -> &String {
        &self.body
    }
    
    /// Get function type
    pub fn get_function_type(&self) -> &JSFunctionType {
        &self.function_type
    }
    
    /// Get position
    pub fn get_position(&self) -> (usize, usize) {
        (self.line, self.column)
    }
    
    /// Check if function is async
    pub fn is_async(&self) -> bool {
        matches!(self.function_type, JSFunctionType::Async | JSFunctionType::AsyncGenerator)
    }
    
    /// Check if function is generator
    pub fn is_generator(&self) -> bool {
        matches!(self.function_type, JSFunctionType::Generator | JSFunctionType::AsyncGenerator)
    }
    
    /// Check if function is arrow function
    pub fn is_arrow_function(&self) -> bool {
        self.function_type == JSFunctionType::ArrowFunction
    }
    
    /// Check if function is constructor
    pub fn is_constructor(&self) -> bool {
        self.function_type == JSFunctionType::Constructor
    }
}