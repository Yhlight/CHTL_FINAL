//! CHTL JS CJMOD System
//! 
//! This module handles CJMOD (CHTL JS Module) operations.

use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// CJMOD module
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CjmodModule {
    pub name: String,
    pub content: String,
}

/// CJMOD system manager
pub struct CjmodSystem {
    /// Loaded modules
    modules: HashMap<String, CjmodModule>,
}

impl CjmodSystem {
    /// Create a new CJMOD system
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
        }
    }
    
    /// Load a CJMOD module
    pub fn load_module(&mut self, name: String, content: String) -> Result<()> {
        let module = CjmodModule {
            name: name.clone(),
            content,
        };
        
        self.modules.insert(name, module);
        Ok(())
    }
    
    /// Get a loaded module
    pub fn get_module(&self, name: &str) -> Option<&CjmodModule> {
        self.modules.get(name)
    }
    
    /// List all loaded modules
    pub fn list_modules(&self) -> Vec<&CjmodModule> {
        self.modules.values().collect()
    }
    
    /// Get module content
    pub fn get_module_content(&self, name: &str) -> Option<&String> {
        self.modules.get(name).map(|m| &m.content)
    }
}