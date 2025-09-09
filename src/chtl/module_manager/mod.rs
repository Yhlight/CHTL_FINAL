//! CHTL Module Manager
//! 
//! This module handles module loading, resolution, and management.

use anyhow::Result;
use std::collections::HashMap;
use std::path::{Path, PathBuf};
use crate::chtl::cmod_system::{CmodSystem, CmodModule};
use crate::chtl_js::cjmod_system::CjmodSystem;
use crate::chtl::module_parser::ModuleParser;

/// Module manager for handling imports and module resolution
pub struct ModuleManager {
    cmod_system: CmodSystem,
    cjmod_system: CjmodSystem,
    search_paths: Vec<PathBuf>,
    loaded_modules: HashMap<String, LoadedModule>,
}

/// Represents a loaded module
#[derive(Debug, Clone)]
pub enum LoadedModule {
    Cmod(CmodModule),
    Cjmod(String), // Content as string
}

impl ModuleManager {
    /// Create a new module manager
    pub fn new() -> Self {
        Self {
            cmod_system: CmodSystem::new(),
            cjmod_system: CjmodSystem::new(),
            search_paths: vec![
                PathBuf::from("./module"),
                PathBuf::from("./src/module"),
                PathBuf::from("./lib"),
            ],
            loaded_modules: HashMap::new(),
        }
    }
    
    /// Add a search path for modules
    pub fn add_search_path<P: AsRef<Path>>(&mut self, path: P) {
        self.search_paths.push(path.as_ref().to_path_buf());
    }
    
    /// Load a module by name
    pub fn load_module(&mut self, name: &str) -> Result<()> {
        // Check if already loaded
        if self.loaded_modules.contains_key(name) {
            return Ok(());
        }
        
        // Search for module in search paths
        for search_path in &self.search_paths {
            let module_path = search_path.join(name);
            
            // Try to load as CMOD
            if module_path.is_dir() {
                if let Ok(cmod) = ModuleParser::parse_cmod(&module_path) {
                    // Note: In a real implementation, we would need to add a method to CmodSystem
                    // to insert modules, as the modules field is private
                    self.loaded_modules.insert(name.to_string(), LoadedModule::Cmod(cmod));
                    return Ok(());
                }
            }
            
            // Try to load as CJMOD
            let cjmod_path = module_path.with_extension("cjjs");
            if cjmod_path.exists() {
                if let Ok(cjmod) = ModuleParser::parse_cjmod(&cjmod_path) {
                    // Note: In a real implementation, we would need to add a method to CjmodSystem
                    // to insert modules, as the modules field is private
                    self.loaded_modules.insert(name.to_string(), LoadedModule::Cjmod(cjmod.content));
                    return Ok(());
                }
            }
        }
        
        Err(anyhow::anyhow!("Module '{}' not found in search paths", name))
    }
    
    /// Get a loaded module
    pub fn get_module(&self, name: &str) -> Option<&LoadedModule> {
        self.loaded_modules.get(name)
    }
    
    /// Resolve module path
    pub fn resolve_module_path(&self, name: &str) -> Option<PathBuf> {
        for search_path in &self.search_paths {
            let module_path = search_path.join(name);
            
            // Check for CMOD directory
            if module_path.is_dir() {
                return Some(module_path);
            }
            
            // Check for CJMOD file
            let cjmod_path = module_path.with_extension("cjjs");
            if cjmod_path.exists() {
                return Some(cjmod_path);
            }
        }
        
        None
    }
    
    /// List all loaded modules
    pub fn list_loaded_modules(&self) -> Vec<String> {
        self.loaded_modules.keys().cloned().collect()
    }
    
    /// Get module content
    pub fn get_module_content(&self, name: &str) -> Option<String> {
        match self.loaded_modules.get(name)? {
            LoadedModule::Cmod(module) => {
                // Read and concatenate all source files
                let mut content = String::new();
                for source_file in &module.source_files {
                    if let Ok(file_content) = std::fs::read_to_string(source_file) {
                        content.push_str(&file_content);
                        content.push('\n');
                    }
                }
                Some(content)
            }
            LoadedModule::Cjmod(content) => Some(content.clone()),
        }
    }
}