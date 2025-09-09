//! CHTL Context
//! 
//! This module manages the compilation context and state.

use std::collections::HashMap;

/// CHTL compilation context
pub struct ChtlContext {
    /// Global variables and constants
    pub globals: HashMap<String, String>,
    /// Imported modules
    pub imports: HashMap<String, String>,
    /// Current namespace
    pub current_namespace: Option<String>,
    /// Configuration settings
    pub config: HashMap<String, String>,
}

impl ChtlContext {
    /// Create a new CHTL context
    pub fn new() -> Self {
        Self {
            globals: HashMap::new(),
            imports: HashMap::new(),
            current_namespace: None,
            config: HashMap::new(),
        }
    }
    
    /// Set a global variable
    pub fn set_global(&mut self, key: String, value: String) {
        self.globals.insert(key, value);
    }
    
    /// Get a global variable
    pub fn get_global(&self, key: &str) -> Option<&String> {
        self.globals.get(key)
    }
    
    /// Set current namespace
    pub fn set_namespace(&mut self, namespace: String) {
        self.current_namespace = Some(namespace);
    }
    
    /// Get current namespace
    pub fn get_namespace(&self) -> Option<&String> {
        self.current_namespace.as_ref()
    }
    
    /// Set configuration
    pub fn set_config(&mut self, key: String, value: String) {
        self.config.insert(key, value);
    }
    
    /// Get configuration
    pub fn get_config(&self, key: &str) -> Option<&String> {
        self.config.get(key)
    }
}