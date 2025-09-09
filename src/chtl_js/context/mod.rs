//! CHTL JS Context
//! 
//! This module manages the CHTL JS compilation context and state.

use std::collections::HashMap;

/// CHTL JS compilation context
pub struct ChtlJSContext {
    /// Global variables and constants
    pub globals: HashMap<String, String>,
    /// Imported modules
    pub imports: HashMap<String, String>,
    /// Current namespace
    pub current_namespace: Option<String>,
    /// Configuration settings
    pub config: HashMap<String, String>,
}

impl ChtlJSContext {
    /// Create a new CHTL JS context
    pub fn new() -> Self {
        Self {
            globals: HashMap::new(),
            imports: HashMap::new(),
            current_namespace: None,
            config: HashMap::new(),
        }
    }
}