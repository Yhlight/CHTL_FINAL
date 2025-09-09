//! JavaScript compiler
//! 
//! This module provides JavaScript compilation functionality for CHTL,
//! including JavaScript parsing, optimization, and generation.

use super::*;
use std::path::PathBuf;
use std::collections::HashMap;

/// JavaScript compiler
#[derive(Debug, Clone)]
pub struct JSCompiler {
    /// Compiler options
    options: JSOptions,
    /// Source maps
    source_maps: HashMap<String, String>,
    /// JavaScript variables
    variables: HashMap<String, String>,
    /// JavaScript imports
    imports: Vec<PathBuf>,
}

/// JavaScript compilation result
#[derive(Debug, Clone)]
pub struct JSCompilationResult {
    /// Compiled JavaScript
    pub js: String,
    /// Source map
    pub source_map: Option<String>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
    /// Dependencies
    pub dependencies: Vec<PathBuf>,
}

/// JavaScript compilation error
#[derive(Debug, Clone)]
pub struct JSCompilationError {
    pub message: String,
    pub line: Option<usize>,
    pub column: Option<usize>,
    pub suggestion: Option<String>,
}

impl JSCompiler {
    /// Create a new JavaScript compiler
    pub fn new() -> Self {
        Self {
            options: JSOptions::default(),
            source_maps: HashMap::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Create a new JavaScript compiler with options
    pub fn with_options(options: JSOptions) -> Self {
        Self {
            options,
            source_maps: HashMap::new(),
            variables: HashMap::new(),
            imports: Vec::new(),
        }
    }
    
    /// Compile JavaScript from file
    pub fn compile_file(&mut self, file_path: &PathBuf) -> Result<JSCompilationResult, JSCompilationError> {
        let content = std::fs::read_to_string(file_path)
            .map_err(|e| JSCompilationError {
                message: format!("Failed to read file: {}", e),
                line: None,
                column: None,
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.compile_content(&content, file_path)
    }
    
    /// Compile JavaScript from content
    pub fn compile_content(&mut self, content: &str, file_path: &PathBuf) -> Result<JSCompilationResult, JSCompilationError> {
        let mut result = JSCompilationResult {
            js: String::new(),
            source_map: None,
            warnings: Vec::new(),
            errors: Vec::new(),
            dependencies: Vec::new(),
        };
        
        // Parse JavaScript
        let functions = self.parse_js(content, &mut result)?;
        
        // Process variables
        self.process_variables(&functions, &mut result)?;
        
        // Process imports
        self.process_imports(&functions, &mut result)?;
        
        // Optimize JavaScript
        let optimized_functions = self.optimize_js(&functions, &mut result)?;
        
        // Generate JavaScript
        result.js = self.generate_js(&optimized_functions, &mut result)?;
        
        // Generate source map if requested
        if self.options.source_maps {
            result.source_map = Some(self.generate_source_map(file_path, &result.js));
        }
        
        Ok(result)
    }
    
    /// Parse JavaScript content
    fn parse_js(&self, content: &str, result: &mut JSCompilationResult) -> Result<Vec<JSFunction>, JSCompilationError> {
        let mut functions = Vec::new();
        let mut current_function: Option<JSFunction> = None;
        let mut current_name = String::new();
        let mut current_parameters = Vec::new();
        let mut current_body = String::new();
        let mut in_function = false;
        let mut in_parameters = false;
        let mut in_body = false;
        let mut brace_count = 0;
        let mut line = 1;
        let mut column = 1;
        
        for (i, ch) in content.chars().enumerate() {
            match ch {
                '\n' => {
                    line += 1;
                    column = 1;
                }
                'f' if i + 8 < content.len() && &content[i..i+8] == "function" => {
                    if let Some(func) = current_function.take() {
                        functions.push(func);
                    }
                    
                    current_function = Some(JSFunction::new(
                        String::new(),
                        JSFunctionType::Function,
                        line,
                        column
                    ));
                    in_function = true;
                    column += 8;
                }
                '=' if i + 1 < content.len() && content.chars().nth(i + 1) == Some('>') => {
                    if let Some(func) = current_function.take() {
                        functions.push(func);
                    }
                    
                    current_function = Some(JSFunction::new(
                        String::new(),
                        JSFunctionType::ArrowFunction,
                        line,
                        column
                    ));
                    in_function = true;
                    column += 1;
                }
                '(' if in_function && !in_parameters => {
                    in_parameters = true;
                    column += 1;
                }
                ')' if in_function && in_parameters => {
                    in_parameters = false;
                    in_body = true;
                    column += 1;
                }
                '{' if in_function && in_body => {
                    brace_count += 1;
                    current_body.push(ch);
                    column += 1;
                }
                '}' if in_function && in_body => {
                    brace_count -= 1;
                    current_body.push(ch);
                    
                    if brace_count == 0 {
                        if let Some(ref mut func) = current_function {
                            func.set_body(current_body.clone());
                            functions.push(func.clone());
                        }
                        current_function = None;
                        in_function = false;
                        in_body = false;
                        current_body.clear();
                    }
                    column += 1;
                }
                _ if in_function && in_parameters => {
                    if ch.is_alphabetic() || ch == '_' || ch == '$' {
                        current_name.push(ch);
                    } else if ch == ',' || ch == ' ' {
                        if !current_name.is_empty() {
                            if let Some(ref mut func) = current_function {
                                func.add_parameter(current_name.clone());
                            }
                            current_name.clear();
                        }
                    }
                    column += 1;
                }
                _ if in_function && in_body => {
                    current_body.push(ch);
                    column += 1;
                }
                _ => {
                    column += 1;
                }
            }
        }
        
        if let Some(func) = current_function {
            functions.push(func);
        }
        
        Ok(functions)
    }
    
    /// Process JavaScript variables
    fn process_variables(&mut self, functions: &[JSFunction], result: &mut JSCompilationResult) -> Result<(), JSCompilationError> {
        for function in functions {
            if function.name.starts_with("var_") || function.name.starts_with("let_") || function.name.starts_with("const_") {
                let var_name = function.name.trim_start_matches("var_").trim_start_matches("let_").trim_start_matches("const_");
                self.variables.insert(var_name.to_string(), function.body.clone());
            }
        }
        
        Ok(())
    }
    
    /// Process JavaScript imports
    fn process_imports(&mut self, functions: &[JSFunction], result: &mut JSCompilationResult) -> Result<(), JSCompilationError> {
        for function in functions {
            if function.name == "import" {
                if let Some(import_path) = self.extract_import_path(&function.body) {
                    result.dependencies.push(import_path);
                }
            }
        }
        
        Ok(())
    }
    
    /// Extract import path from JavaScript function body
    fn extract_import_path(&self, body: &str) -> Option<PathBuf> {
        let body = body.trim();
        if body.starts_with("require(") && body.ends_with(")") {
            let path = &body[8..body.len()-1];
            if path.starts_with('"') && path.ends_with('"') {
                let path = &path[1..path.len()-1];
                return Some(PathBuf::from(path));
            }
        }
        None
    }
    
    /// Optimize JavaScript
    fn optimize_js(&self, functions: &[JSFunction], result: &mut JSCompilationResult) -> Result<Vec<JSFunction>, JSCompilationError> {
        let mut optimized_functions = functions.to_vec();
        
        if self.options.remove_unused {
            optimized_functions = self.remove_unused_functions(optimized_functions, result);
        }
        
        if self.options.optimize {
            optimized_functions = self.optimize_functions(optimized_functions, result);
        }
        
        Ok(optimized_functions)
    }
    
    /// Remove unused JavaScript functions
    fn remove_unused_functions(&self, functions: Vec<JSFunction>, result: &mut JSCompilationResult) -> Vec<JSFunction> {
        // This is a simplified implementation
        // In a real implementation, you would analyze the code to determine which functions are used
        functions.into_iter().filter(|func| {
            !func.name.starts_with("_") || 
            !func.name.contains("unused") ||
            !func.name.contains("deprecated")
        }).collect()
    }
    
    /// Optimize JavaScript functions
    fn optimize_functions(&self, functions: Vec<JSFunction>, result: &mut JSCompilationResult) -> Vec<JSFunction> {
        functions.into_iter().map(|mut func| {
            // Remove unnecessary whitespace
            func.body = func.body
                .lines()
                .map(|line| line.trim())
                .filter(|line| !line.is_empty())
                .collect::<Vec<&str>>()
                .join(" ");
            
            // Remove unnecessary semicolons
            func.body = func.body
                .replace(";;", ";")
                .replace("; ", ";")
                .replace(" ;", ";");
            
            func
        }).collect()
    }
    
    /// Generate JavaScript from functions
    fn generate_js(&self, functions: &[JSFunction], result: &mut JSCompilationResult) -> Result<String, JSCompilationError> {
        let mut js = String::new();
        
        for function in functions {
            js.push_str(&self.generate_function(function, result)?);
            js.push('\n');
        }
        
        if self.options.minify {
            js = self.minify_js(&js);
        }
        
        Ok(js)
    }
    
    /// Generate a single JavaScript function
    fn generate_function(&self, function: &JSFunction, result: &mut JSCompilationResult) -> Result<String, JSCompilationError> {
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
    }
    
    /// Generate source map
    fn generate_source_map(&self, file_path: &PathBuf, js: &str) -> String {
        format!(
            r#"{{"version":3,"sources":["{}"],"names":[],"mappings":"","file":"{}.js"}}"#,
            file_path.to_string_lossy(),
            file_path.file_stem().unwrap_or_default().to_string_lossy()
        )
    }
    
    /// Set compiler options
    pub fn set_options(&mut self, options: JSOptions) {
        self.options = options;
    }
    
    /// Get compiler options
    pub fn get_options(&self) -> &JSOptions {
        &self.options
    }
    
    /// Get JavaScript variables
    pub fn get_variables(&self) -> &HashMap<String, String> {
        &self.variables
    }
    
    /// Get JavaScript imports
    pub fn get_imports(&self) -> &Vec<PathBuf> {
        &self.imports
    }
}

impl Default for JSCompiler {
    fn default() -> Self {
        Self::new()
    }
}