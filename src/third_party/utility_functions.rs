//! Utility functions
//! 
//! This module provides utility function functionality for CHTL,
//! including string utilities, number utilities, and custom utilities.

use super::*;
use std::collections::HashMap;

/// Utility function manager
#[derive(Debug, Clone)]
pub struct UtilityFunctionManager {
    /// Registered functions
    functions: HashMap<String, UtilityFunction>,
    /// Function categories
    categories: HashMap<String, Vec<String>>,
    /// Configuration
    config: UtilityFunctionConfig,
}

/// Utility function configuration
#[derive(Debug, Clone)]
pub struct UtilityFunctionConfig {
    /// Enable string utilities
    pub enable_string_utilities: bool,
    /// Enable number utilities
    pub enable_number_utilities: bool,
    /// Enable date utilities
    pub enable_date_utilities: bool,
    /// Enable file utilities
    pub enable_file_utilities: bool,
    /// Enable network utilities
    pub enable_network_utilities: bool,
    /// Enable custom utilities
    pub enable_custom_utilities: bool,
    /// Function timeout
    pub function_timeout: Option<std::time::Duration>,
}

/// Utility function result
#[derive(Debug, Clone)]
pub struct UtilityFunctionResult {
    /// Success status
    pub success: bool,
    /// Result value
    pub value: Option<String>,
    /// Error message
    pub error: Option<String>,
    /// Execution time
    pub execution_time: std::time::Duration,
}

/// Utility function error
#[derive(Debug, Clone)]
pub struct UtilityFunctionError {
    pub message: String,
    pub function_name: String,
    pub suggestion: Option<String>,
}

impl UtilityFunctionManager {
    /// Create a new utility function manager
    pub fn new() -> Self {
        let mut manager = Self {
            functions: HashMap::new(),
            categories: HashMap::new(),
            config: UtilityFunctionConfig::default(),
        };
        
        // Register default utility functions
        manager.register_default_functions();
        
        manager
    }
    
    /// Create a new utility function manager with configuration
    pub fn with_config(config: UtilityFunctionConfig) -> Self {
        let mut manager = Self {
            functions: HashMap::new(),
            categories: HashMap::new(),
            config,
        };
        
        // Register default utility functions
        manager.register_default_functions();
        
        manager
    }
    
    /// Register default utility functions
    fn register_default_functions(&mut self) {
        if self.config.enable_string_utilities {
            self.register_string_utilities();
        }
        
        if self.config.enable_number_utilities {
            self.register_number_utilities();
        }
        
        if self.config.enable_date_utilities {
            self.register_date_utilities();
        }
        
        if self.config.enable_file_utilities {
            self.register_file_utilities();
        }
        
        if self.config.enable_network_utilities {
            self.register_network_utilities();
        }
    }
    
    /// Register string utility functions
    fn register_string_utilities(&mut self) {
        // String length
        let mut func = UtilityFunction::new(
            "str_length".to_string(),
            "Get the length of a string".to_string(),
            UtilityFunctionType::String,
        );
        func.add_parameter("str".to_string());
        func.set_return_type("number".to_string());
        func.set_implementation("str.length".to_string());
        self.register_function(func);
        
        // String uppercase
        let mut func = UtilityFunction::new(
            "str_upper".to_string(),
            "Convert string to uppercase".to_string(),
            UtilityFunctionType::String,
        );
        func.add_parameter("str".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("str.toUpperCase()".to_string());
        self.register_function(func);
        
        // String lowercase
        let mut func = UtilityFunction::new(
            "str_lower".to_string(),
            "Convert string to lowercase".to_string(),
            UtilityFunctionType::String,
        );
        func.add_parameter("str".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("str.toLowerCase()".to_string());
        self.register_function(func);
        
        // String trim
        let mut func = UtilityFunction::new(
            "str_trim".to_string(),
            "Remove whitespace from string".to_string(),
            UtilityFunctionType::String,
        );
        func.add_parameter("str".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("str.trim()".to_string());
        self.register_function(func);
        
        // String replace
        let mut func = UtilityFunction::new(
            "str_replace".to_string(),
            "Replace occurrences in string".to_string(),
            UtilityFunctionType::String,
        );
        func.add_parameter("str".to_string());
        func.add_parameter("search".to_string());
        func.add_parameter("replace".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("str.replace(search, replace)".to_string());
        self.register_function(func);
    }
    
    /// Register number utility functions
    fn register_number_utilities(&mut self) {
        // Number absolute value
        let mut func = UtilityFunction::new(
            "num_abs".to_string(),
            "Get absolute value of number".to_string(),
            UtilityFunctionType::Number,
        );
        func.add_parameter("num".to_string());
        func.set_return_type("number".to_string());
        func.set_implementation("Math.abs(num)".to_string());
        self.register_function(func);
        
        // Number round
        let mut func = UtilityFunction::new(
            "num_round".to_string(),
            "Round number to nearest integer".to_string(),
            UtilityFunctionType::Number,
        );
        func.add_parameter("num".to_string());
        func.set_return_type("number".to_string());
        func.set_implementation("Math.round(num)".to_string());
        self.register_function(func);
        
        // Number floor
        let mut func = UtilityFunction::new(
            "num_floor".to_string(),
            "Round number down to nearest integer".to_string(),
            UtilityFunctionType::Number,
        );
        func.add_parameter("num".to_string());
        func.set_return_type("number".to_string());
        func.set_implementation("Math.floor(num)".to_string());
        self.register_function(func);
        
        // Number ceiling
        let mut func = UtilityFunction::new(
            "num_ceil".to_string(),
            "Round number up to nearest integer".to_string(),
            UtilityFunctionType::Number,
        );
        func.add_parameter("num".to_string());
        func.set_return_type("number".to_string());
        func.set_implementation("Math.ceil(num)".to_string());
        self.register_function(func);
        
        // Number random
        let mut func = UtilityFunction::new(
            "num_random".to_string(),
            "Generate random number between 0 and 1".to_string(),
            UtilityFunctionType::Number,
        );
        func.set_return_type("number".to_string());
        func.set_implementation("Math.random()".to_string());
        self.register_function(func);
    }
    
    /// Register date utility functions
    fn register_date_utilities(&mut self) {
        // Current timestamp
        let mut func = UtilityFunction::new(
            "date_now".to_string(),
            "Get current timestamp".to_string(),
            UtilityFunctionType::Date,
        );
        func.set_return_type("number".to_string());
        func.set_implementation("Date.now()".to_string());
        self.register_function(func);
        
        // Date format
        let mut func = UtilityFunction::new(
            "date_format".to_string(),
            "Format date as string".to_string(),
            UtilityFunctionType::Date,
        );
        func.add_parameter("date".to_string());
        func.add_parameter("format".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("date.toLocaleDateString(format)".to_string());
        self.register_function(func);
        
        // Date parse
        let mut func = UtilityFunction::new(
            "date_parse".to_string(),
            "Parse date from string".to_string(),
            UtilityFunctionType::Date,
        );
        func.add_parameter("date_str".to_string());
        func.set_return_type("Date".to_string());
        func.set_implementation("new Date(date_str)".to_string());
        self.register_function(func);
    }
    
    /// Register file utility functions
    fn register_file_utilities(&mut self) {
        // File exists
        let mut func = UtilityFunction::new(
            "file_exists".to_string(),
            "Check if file exists".to_string(),
            UtilityFunctionType::File,
        );
        func.add_parameter("path".to_string());
        func.set_return_type("boolean".to_string());
        func.set_implementation("fs.existsSync(path)".to_string());
        self.register_function(func);
        
        // File read
        let mut func = UtilityFunction::new(
            "file_read".to_string(),
            "Read file contents".to_string(),
            UtilityFunctionType::File,
        );
        func.add_parameter("path".to_string());
        func.set_return_type("string".to_string());
        func.set_implementation("fs.readFileSync(path, 'utf8')".to_string());
        self.register_function(func);
        
        // File write
        let mut func = UtilityFunction::new(
            "file_write".to_string(),
            "Write content to file".to_string(),
            UtilityFunctionType::File,
        );
        func.add_parameter("path".to_string());
        func.add_parameter("content".to_string());
        func.set_return_type("void".to_string());
        func.set_implementation("fs.writeFileSync(path, content)".to_string());
        self.register_function(func);
    }
    
    /// Register network utility functions
    fn register_network_utilities(&mut self) {
        // HTTP GET
        let mut func = UtilityFunction::new(
            "http_get".to_string(),
            "Make HTTP GET request".to_string(),
            UtilityFunctionType::Network,
        );
        func.add_parameter("url".to_string());
        func.set_return_type("Promise<string>".to_string());
        func.set_implementation("fetch(url).then(r => r.text())".to_string());
        self.register_function(func);
        
        // HTTP POST
        let mut func = UtilityFunction::new(
            "http_post".to_string(),
            "Make HTTP POST request".to_string(),
            UtilityFunctionType::Network,
        );
        func.add_parameter("url".to_string());
        func.add_parameter("data".to_string());
        func.set_return_type("Promise<string>".to_string());
        func.set_implementation("fetch(url, {method: 'POST', body: data}).then(r => r.text())".to_string());
        self.register_function(func);
    }
    
    /// Register a utility function
    pub fn register_function(&mut self, function: UtilityFunction) -> Result<(), UtilityFunctionError> {
        let name = function.name.clone();
        
        // Validate function
        self.validate_function(&function)?;
        
        // Register function
        self.functions.insert(name.clone(), function);
        
        // Add to category
        let category = self.get_function_category(&name);
        self.categories.entry(category).or_insert_with(Vec::new).push(name);
        
        Ok(())
    }
    
    /// Validate utility function
    fn validate_function(&self, function: &UtilityFunction) -> Result<(), UtilityFunctionError> {
        // Check if function name is valid
        if function.name.is_empty() {
            return Err(UtilityFunctionError {
                message: "Function name cannot be empty".to_string(),
                function_name: function.name.clone(),
                suggestion: Some("Provide a valid function name".to_string()),
            });
        }
        
        // Check if description is valid
        if function.description.is_empty() {
            return Err(UtilityFunctionError {
                message: "Function description cannot be empty".to_string(),
                function_name: function.name.clone(),
                suggestion: Some("Provide a valid function description".to_string()),
            });
        }
        
        // Check if implementation is valid
        if function.implementation.is_empty() {
            return Err(UtilityFunctionError {
                message: "Function implementation cannot be empty".to_string(),
                function_name: function.name.clone(),
                suggestion: Some("Provide a valid function implementation".to_string()),
            });
        }
        
        Ok(())
    }
    
    /// Get function category
    fn get_function_category(&self, name: &str) -> String {
        if name.starts_with("str_") {
            "string".to_string()
        } else if name.starts_with("num_") {
            "number".to_string()
        } else if name.starts_with("date_") {
            "date".to_string()
        } else if name.starts_with("file_") {
            "file".to_string()
        } else if name.starts_with("http_") {
            "network".to_string()
        } else {
            "custom".to_string()
        }
    }
    
    /// Execute a utility function
    pub fn execute_function(&self, name: &str, args: Vec<String>) -> Result<UtilityFunctionResult, UtilityFunctionError> {
        let function = self.functions.get(name)
            .ok_or_else(|| UtilityFunctionError {
                message: format!("Function '{}' not found", name),
                function_name: name.to_string(),
                suggestion: Some("Check function name or register the function".to_string()),
            })?;
        
        let start_time = std::time::Instant::now();
        
        // Validate arguments
        if args.len() != function.parameters.len() {
            return Err(UtilityFunctionError {
                message: format!("Expected {} arguments, got {}", function.parameters.len(), args.len()),
                function_name: name.to_string(),
                suggestion: Some("Check function signature".to_string()),
            });
        }
        
        // Execute function
        let result = self.execute_function_implementation(function, args);
        let execution_time = start_time.elapsed();
        
        Ok(UtilityFunctionResult {
            success: result.is_ok(),
            value: result.ok(),
            error: result.err(),
            execution_time,
        })
    }
    
    /// Execute function implementation
    fn execute_function_implementation(&self, function: &UtilityFunction, args: Vec<String>) -> Result<String, String> {
        // This is a simplified implementation
        // In a real implementation, you would use a JavaScript engine or similar
        
        let mut implementation = function.implementation.clone();
        
        // Replace parameters with arguments
        for (i, param) in function.parameters.iter().enumerate() {
            if i < args.len() {
                implementation = implementation.replace(param, &args[i]);
            }
        }
        
        // Execute implementation
        // For now, just return the implementation as a string
        Ok(implementation)
    }
    
    /// Get function
    pub fn get_function(&self, name: &str) -> Option<&UtilityFunction> {
        self.functions.get(name)
    }
    
    /// Get all functions
    pub fn get_functions(&self) -> &HashMap<String, UtilityFunction> {
        &self.functions
    }
    
    /// Get functions by category
    pub fn get_functions_by_category(&self, category: &str) -> Option<&Vec<String>> {
        self.categories.get(category)
    }
    
    /// Get all categories
    pub fn get_categories(&self) -> &HashMap<String, Vec<String>> {
        &self.categories
    }
    
    /// Check if function is registered
    pub fn is_function_registered(&self, name: &str) -> bool {
        self.functions.contains_key(name)
    }
    
    /// Unregister function
    pub fn unregister_function(&mut self, name: &str) -> Result<(), UtilityFunctionError> {
        if let Some(function) = self.functions.remove(name) {
            // Remove from category
            let category = self.get_function_category(name);
            if let Some(functions) = self.categories.get_mut(&category) {
                functions.retain(|f| f != name);
            }
            Ok(())
        } else {
            Err(UtilityFunctionError {
                message: format!("Function '{}' is not registered", name),
                function_name: name.to_string(),
                suggestion: Some("Check function name".to_string()),
            })
        }
    }
    
    /// Clear all functions
    pub fn clear_functions(&mut self) {
        self.functions.clear();
        self.categories.clear();
    }
    
    /// Set configuration
    pub fn set_config(&mut self, config: UtilityFunctionConfig) {
        self.config = config;
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &UtilityFunctionConfig {
        &self.config
    }
}

impl Default for UtilityFunctionManager {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for UtilityFunctionConfig {
    fn default() -> Self {
        Self {
            enable_string_utilities: true,
            enable_number_utilities: true,
            enable_date_utilities: true,
            enable_file_utilities: true,
            enable_network_utilities: true,
            enable_custom_utilities: true,
            function_timeout: None,
        }
    }
}