//! CHTL JS Parser State
//! 
//! This module manages the parser state during CHTL JS compilation.

use std::collections::HashMap;

/// CHTL JS Parser state
pub struct ChtlJSParserState {
    pub variables: HashMap<String, String>,
}

impl ChtlJSParserState {
    /// Create a new CHTL JS parser state
    pub fn new() -> Self {
        Self {
            variables: HashMap::new(),
        }
    }
}