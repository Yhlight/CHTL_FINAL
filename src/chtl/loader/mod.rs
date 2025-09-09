//! CHTL File Loader
//! 
//! This module handles loading and managing CHTL files and modules.

use anyhow::Result;
use std::path::Path;
use std::collections::HashMap;

/// File loader for CHTL
pub struct ChtlLoader {
    /// Loaded files cache
    loaded_files: HashMap<String, String>,
    /// Module search paths
    search_paths: Vec<String>,
}

impl ChtlLoader {
    /// Create a new CHTL loader
    pub fn new() -> Self {
        Self {
            loaded_files: HashMap::new(),
            search_paths: vec![
                ".".to_string(),
                "./module".to_string(),
                "./src".to_string(),
            ],
        }
    }
    
    /// Load a CHTL file
    pub fn load_file<P: AsRef<Path>>(&mut self, path: P) -> Result<String> {
        let path_str = path.as_ref().to_string_lossy().to_string();
        
        // Check cache first
        if let Some(content) = self.loaded_files.get(&path_str) {
            return Ok(content.clone());
        }
        
        // Load file from filesystem
        let content = std::fs::read_to_string(&path)?;
        
        // Cache the content
        self.loaded_files.insert(path_str, content.clone());
        
        Ok(content)
    }
    
    /// Load a module by name
    pub fn load_module(&mut self, module_name: &str) -> Result<String> {
        // Search in module paths
        for search_path in &self.search_paths {
            let module_path = format!("{}/{}.chtl", search_path, module_name);
            if Path::new(&module_path).exists() {
                return self.load_file(&module_path);
            }
            
            // Also try .cmod files
            let cmod_path = format!("{}/{}.cmod", search_path, module_name);
            if Path::new(&cmod_path).exists() {
                return self.load_file(&cmod_path);
            }
        }
        
        Err(anyhow::anyhow!("Module '{}' not found", module_name))
    }
    
    /// Add search path
    pub fn add_search_path(&mut self, path: String) {
        self.search_paths.push(path);
    }
    
    /// Clear cache
    pub fn clear_cache(&mut self) {
        self.loaded_files.clear();
    }
}