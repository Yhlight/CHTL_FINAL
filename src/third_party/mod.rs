//! Third-party module
//! 
//! This module provides third-party integration functionality for CHTL,
//! including external library integration, API wrappers, and utility functions.

pub mod external_libs;
pub mod api_wrappers;
pub mod utility_functions;

use std::collections::HashMap;
use std::path::PathBuf;

/// Third-party integration context
#[derive(Debug, Clone)]
pub struct ThirdPartyContext {
    /// External libraries
    pub libraries: HashMap<String, ExternalLibrary>,
    /// API wrappers
    pub api_wrappers: HashMap<String, APIWrapper>,
    /// Utility functions
    pub utility_functions: HashMap<String, UtilityFunction>,
    /// Configuration
    pub config: ThirdPartyConfig,
}

/// External library
#[derive(Debug, Clone)]
pub struct ExternalLibrary {
    /// Library name
    pub name: String,
    /// Library version
    pub version: String,
    /// Library path
    pub path: PathBuf,
    /// Library type
    pub library_type: LibraryType,
    /// Dependencies
    pub dependencies: Vec<String>,
    /// Functions
    pub functions: Vec<String>,
    /// Classes
    pub classes: Vec<String>,
}

/// Library type
#[derive(Debug, Clone, PartialEq)]
pub enum LibraryType {
    Static,
    Dynamic,
    HeaderOnly,
    Source,
    Binary,
}

/// API wrapper
#[derive(Debug, Clone)]
pub struct APIWrapper {
    /// Wrapper name
    pub name: String,
    /// API endpoint
    pub endpoint: String,
    /// Wrapper type
    pub wrapper_type: WrapperType,
    /// Functions
    pub functions: Vec<String>,
    /// Configuration
    pub config: HashMap<String, String>,
}

/// Wrapper type
#[derive(Debug, Clone, PartialEq)]
pub enum WrapperType {
    REST,
    GraphQL,
    WebSocket,
    gRPC,
    Custom,
}

/// Utility function
#[derive(Debug, Clone)]
pub struct UtilityFunction {
    /// Function name
    pub name: String,
    /// Function description
    pub description: String,
    /// Function type
    pub function_type: UtilityFunctionType,
    /// Parameters
    pub parameters: Vec<String>,
    /// Return type
    pub return_type: String,
    /// Implementation
    pub implementation: String,
}

/// Utility function type
#[derive(Debug, Clone, PartialEq)]
pub enum UtilityFunctionType {
    String,
    Number,
    Boolean,
    Array,
    Object,
    Date,
    Math,
    File,
    Network,
    Custom,
}

/// Third-party configuration
#[derive(Debug, Clone)]
pub struct ThirdPartyConfig {
    /// Enable external libraries
    pub enable_libraries: bool,
    /// Enable API wrappers
    pub enable_api_wrappers: bool,
    /// Enable utility functions
    pub enable_utility_functions: bool,
    /// Library search paths
    pub library_search_paths: Vec<PathBuf>,
    /// API base URLs
    pub api_base_urls: HashMap<String, String>,
    /// Timeout settings
    pub timeouts: HashMap<String, u64>,
    /// Retry settings
    pub retry_settings: HashMap<String, u32>,
}

impl ThirdPartyContext {
    /// Create a new third-party context
    pub fn new() -> Self {
        Self {
            libraries: HashMap::new(),
            api_wrappers: HashMap::new(),
            utility_functions: HashMap::new(),
            config: ThirdPartyConfig::default(),
        }
    }
    
    /// Add an external library
    pub fn add_library(&mut self, library: ExternalLibrary) {
        self.libraries.insert(library.name.clone(), library);
    }
    
    /// Add an API wrapper
    pub fn add_api_wrapper(&mut self, wrapper: APIWrapper) {
        self.api_wrappers.insert(wrapper.name.clone(), wrapper);
    }
    
    /// Add a utility function
    pub fn add_utility_function(&mut self, function: UtilityFunction) {
        self.utility_functions.insert(function.name.clone(), function);
    }
    
    /// Get an external library
    pub fn get_library(&self, name: &str) -> Option<&ExternalLibrary> {
        self.libraries.get(name)
    }
    
    /// Get an API wrapper
    pub fn get_api_wrapper(&self, name: &str) -> Option<&APIWrapper> {
        self.api_wrappers.get(name)
    }
    
    /// Get a utility function
    pub fn get_utility_function(&self, name: &str) -> Option<&UtilityFunction> {
        self.utility_functions.get(name)
    }
    
    /// Set configuration
    pub fn set_config(&mut self, config: ThirdPartyConfig) {
        self.config = config;
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &ThirdPartyConfig {
        &self.config
    }
    
    /// Get all libraries
    pub fn get_libraries(&self) -> &HashMap<String, ExternalLibrary> {
        &self.libraries
    }
    
    /// Get all API wrappers
    pub fn get_api_wrappers(&self) -> &HashMap<String, APIWrapper> {
        &self.api_wrappers
    }
    
    /// Get all utility functions
    pub fn get_utility_functions(&self) -> &HashMap<String, UtilityFunction> {
        &self.utility_functions
    }
}

impl ExternalLibrary {
    /// Create a new external library
    pub fn new(name: String, version: String, path: PathBuf, library_type: LibraryType) -> Self {
        Self {
            name,
            version,
            path,
            library_type,
            dependencies: Vec::new(),
            functions: Vec::new(),
            classes: Vec::new(),
        }
    }
    
    /// Add a dependency
    pub fn add_dependency(&mut self, dependency: String) {
        self.dependencies.push(dependency);
    }
    
    /// Add a function
    pub fn add_function(&mut self, function: String) {
        self.functions.push(function);
    }
    
    /// Add a class
    pub fn add_class(&mut self, class: String) {
        self.classes.push(class);
    }
    
    /// Get library name
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    /// Get library version
    pub fn get_version(&self) -> &String {
        &self.version
    }
    
    /// Get library path
    pub fn get_path(&self) -> &PathBuf {
        &self.path
    }
    
    /// Get library type
    pub fn get_library_type(&self) -> &LibraryType {
        &self.library_type
    }
    
    /// Get dependencies
    pub fn get_dependencies(&self) -> &Vec<String> {
        &self.dependencies
    }
    
    /// Get functions
    pub fn get_functions(&self) -> &Vec<String> {
        &self.functions
    }
    
    /// Get classes
    pub fn get_classes(&self) -> &Vec<String> {
        &self.classes
    }
}

impl APIWrapper {
    /// Create a new API wrapper
    pub fn new(name: String, endpoint: String, wrapper_type: WrapperType) -> Self {
        Self {
            name,
            endpoint,
            wrapper_type,
            functions: Vec::new(),
            config: HashMap::new(),
        }
    }
    
    /// Add a function
    pub fn add_function(&mut self, function: String) {
        self.functions.push(function);
    }
    
    /// Set configuration
    pub fn set_config(&mut self, key: String, value: String) {
        self.config.insert(key, value);
    }
    
    /// Get wrapper name
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    /// Get API endpoint
    pub fn get_endpoint(&self) -> &String {
        &self.endpoint
    }
    
    /// Get wrapper type
    pub fn get_wrapper_type(&self) -> &WrapperType {
        &self.wrapper_type
    }
    
    /// Get functions
    pub fn get_functions(&self) -> &Vec<String> {
        &self.functions
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &HashMap<String, String> {
        &self.config
    }
}

impl UtilityFunction {
    /// Create a new utility function
    pub fn new(name: String, description: String, function_type: UtilityFunctionType) -> Self {
        Self {
            name,
            description,
            function_type,
            parameters: Vec::new(),
            return_type: String::new(),
            implementation: String::new(),
        }
    }
    
    /// Add a parameter
    pub fn add_parameter(&mut self, parameter: String) {
        self.parameters.push(parameter);
    }
    
    /// Set return type
    pub fn set_return_type(&mut self, return_type: String) {
        self.return_type = return_type;
    }
    
    /// Set implementation
    pub fn set_implementation(&mut self, implementation: String) {
        self.implementation = implementation;
    }
    
    /// Get function name
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    /// Get function description
    pub fn get_description(&self) -> &String {
        &self.description
    }
    
    /// Get function type
    pub fn get_function_type(&self) -> &UtilityFunctionType {
        &self.function_type
    }
    
    /// Get parameters
    pub fn get_parameters(&self) -> &Vec<String> {
        &self.parameters
    }
    
    /// Get return type
    pub fn get_return_type(&self) -> &String {
        &self.return_type
    }
    
    /// Get implementation
    pub fn get_implementation(&self) -> &String {
        &self.implementation
    }
}

impl Default for ThirdPartyContext {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for ThirdPartyConfig {
    fn default() -> Self {
        Self {
            enable_libraries: true,
            enable_api_wrappers: true,
            enable_utility_functions: true,
            library_search_paths: Vec::new(),
            api_base_urls: HashMap::new(),
            timeouts: HashMap::new(),
            retry_settings: HashMap::new(),
        }
    }
}