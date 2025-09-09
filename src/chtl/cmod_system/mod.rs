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
        let info_file = path.join("info").join("module.chtl");
        let info_content = if info_file.exists() {
            std::fs::read_to_string(info_file)?
        } else {
            String::new()
        };
        
        // Parse module info
        let info = self.parse_module_info(&info_content, path)?;
        
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
        
        // Parse exports
        let exports = self.parse_exports(&info_content)?;
        
        let module = CmodModule {
            info,
            exports,
            source_files,
        };
        
        self.modules.insert(module.info.name.clone(), module);
        
        Ok(())
    }
    
    /// Parse module info from CHTL content
    fn parse_module_info(&self, content: &str, path: &Path) -> Result<CmodInfo> {
        // Simple parsing - in a real implementation, this would use the CHTL parser
        let name = path.file_name()
            .and_then(|s| s.to_str())
            .unwrap_or("unknown")
            .to_string();
        
        // Parse version from content
        let version = self.extract_setting(content, "version").unwrap_or_else(|| "1.0.0".to_string());
        let description = self.extract_setting(content, "description").unwrap_or_else(|| "CMOD Module".to_string());
        let author = self.extract_setting(content, "author").unwrap_or_else(|| "Unknown".to_string());
        let license = self.extract_setting(content, "license").unwrap_or_else(|| "MIT".to_string());
        let category = self.extract_setting(content, "category").unwrap_or_else(|| "general".to_string());
        let min_chtl_version = self.extract_setting(content, "min_chtl_version").unwrap_or_else(|| "1.0.0".to_string());
        let max_chtl_version = self.extract_setting(content, "max_chtl_version").unwrap_or_else(|| "2.0.0".to_string());
        
        // Parse dependencies
        let dependencies = self.extract_dependencies(content);
        
        Ok(CmodInfo {
            name,
            version,
            description,
            author,
            license,
            dependencies,
            category,
            min_chtl_version,
            max_chtl_version,
        })
    }
    
    /// Extract a setting value from module info content
    fn extract_setting(&self, content: &str, key: &str) -> Option<String> {
        for line in content.lines() {
            if line.trim().starts_with(&format!("{}:", key)) {
                if let Some(value) = line.split(':').nth(1) {
                    return Some(value.trim().to_string());
                }
            }
        }
        None
    }
    
    /// Extract dependencies from module info content
    fn extract_dependencies(&self, content: &str) -> Vec<String> {
        let mut dependencies = Vec::new();
        
        for line in content.lines() {
            if line.trim().starts_with("dependencies:") {
                if let Some(deps_str) = line.split(':').nth(1) {
                    for dep in deps_str.split(',') {
                        dependencies.push(dep.trim().to_string());
                    }
                }
            }
        }
        
        dependencies
    }
    
    /// Parse exports from CHTL content
    fn parse_exports(&self, content: &str) -> Result<Vec<String>> {
        let mut exports = Vec::new();
        
        for line in content.lines() {
            if line.trim().starts_with("[Export]") {
                // Extract export name
                if let Some(export_name) = line.split_whitespace().nth(1) {
                    exports.push(export_name.to_string());
                }
            }
        }
        
        Ok(exports)
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