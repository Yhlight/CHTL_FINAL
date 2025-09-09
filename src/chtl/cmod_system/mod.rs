//! CHTL CMOD System
//! 
//! This module handles CMOD (CHTL Module) operations.

use anyhow::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::path::Path;

/// CMOD module information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CmodInfo {
    pub name: String,
    pub version: String,
    pub description: String,
    pub author: String,
    pub license: String,
    pub dependencies: Vec<String>,
    pub category: String,
    pub min_chtl_version: String,
    pub max_chtl_version: String,
}

/// CMOD module
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CmodModule {
    pub info: CmodInfo,
    pub exports: Vec<String>,
    pub source_files: Vec<String>,
}

/// CMOD system manager
pub struct CmodSystem {
    /// Loaded modules
    modules: HashMap<String, CmodModule>,
    /// Module search paths
    search_paths: Vec<String>,
}

impl CmodSystem {
    /// Create a new CMOD system
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            search_paths: vec![
                "./module".to_string(),
                "./src/module".to_string(),
            ],
        }
    }
    
    /// Load a CMOD module
    pub fn load_module<P: AsRef<Path>>(&mut self, path: P) -> Result<()> {
        let path = path.as_ref();
        
        // Read module info
        let info_file = path.join("info").join(format!("{}.chtl", path.file_name().unwrap().to_string_lossy()));
        let info_content = std::fs::read_to_string(info_file)?;
        
        // Parse module info (simplified)
        let info = CmodInfo {
            name: path.file_name().unwrap().to_string_lossy().to_string(),
            version: "1.0.0".to_string(),
            description: "CMOD Module".to_string(),
            author: "Unknown".to_string(),
            license: "MIT".to_string(),
            dependencies: Vec::new(),
            category: "general".to_string(),
            min_chtl_version: "1.0.0".to_string(),
            max_chtl_version: "2.0.0".to_string(),
        };
        
        // Find source files
        let src_dir = path.join("src");
        let mut source_files = Vec::new();
        
        if src_dir.exists() {
            for entry in std::fs::read_dir(&src_dir)? {
                let entry = entry?;
                if entry.path().extension().and_then(|s| s.to_str()) == Some("chtl") {
                    source_files.push(entry.path().to_string_lossy().to_string());
                }
            }
        }
        
        let module = CmodModule {
            info,
            exports: Vec::new(),
            source_files,
        };
        
        self.modules.insert(module.info.name.clone(), module);
        
        Ok(())
    }
    
    /// Get a loaded module
    pub fn get_module(&self, name: &str) -> Option<&CmodModule> {
        self.modules.get(name)
    }
    
    /// List all loaded modules
    pub fn list_modules(&self) -> Vec<&CmodModule> {
        self.modules.values().collect()
    }
    
    /// Add search path
    pub fn add_search_path(&mut self, path: String) {
        self.search_paths.push(path);
    }
}