//! Module system
//! 
//! This module provides module system functionality for CHTL,
//! including CMOD modules, CJMOD modules, and module management.

pub mod cmod_system;
pub mod cjmod_system;
pub mod module_manager;
pub mod module_loader;

use std::collections::HashMap;
use std::path::PathBuf;

/// Module system context
#[derive(Debug, Clone)]
pub struct ModuleContext {
    /// Loaded modules
    modules: HashMap<String, Module>,
    /// Module search paths
    search_paths: Vec<PathBuf>,
    /// Module cache
    cache: HashMap<String, ModuleCache>,
    /// Configuration
    config: ModuleConfig,
}

/// Module
#[derive(Debug, Clone)]
pub struct Module {
    /// Module name
    pub name: String,
    /// Module version
    pub version: String,
    /// Module path
    pub path: PathBuf,
    /// Module type
    pub module_type: ModuleType,
    /// Dependencies
    pub dependencies: Vec<String>,
    /// Exports
    pub exports: Vec<String>,
    /// Module content
    pub content: String,
    /// Module metadata
    pub metadata: HashMap<String, String>,
}

/// Module type
#[derive(Debug, Clone, PartialEq)]
pub enum ModuleType {
    CMOD,
    CJMOD,
    Hybrid,
    External,
}

/// Module cache
#[derive(Debug, Clone)]
pub struct ModuleCache {
    /// Module path
    pub path: PathBuf,
    /// Last modified time
    pub last_modified: std::time::SystemTime,
    /// Cached content
    pub content: String,
    /// Cached exports
    pub exports: Vec<String>,
}

/// Module configuration
#[derive(Debug, Clone)]
pub struct ModuleConfig {
    /// Enable module caching
    pub enable_caching: bool,
    /// Cache timeout
    pub cache_timeout: Option<std::time::Duration>,
    /// Enable module validation
    pub enable_validation: bool,
    /// Enable dependency resolution
    pub enable_dependency_resolution: bool,
    /// Module search paths
    pub search_paths: Vec<PathBuf>,
    /// Allowed module types
    pub allowed_types: Vec<ModuleType>,
}

/// Module loading result
#[derive(Debug, Clone)]
pub struct ModuleLoadingResult {
    /// Success status
    pub success: bool,
    /// Module name
    pub name: String,
    /// Loaded exports
    pub exports: Vec<String>,
    /// Dependencies
    pub dependencies: Vec<String>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// Module loading error
#[derive(Debug, Clone)]
pub struct ModuleLoadingError {
    pub message: String,
    pub module_name: String,
    pub suggestion: Option<String>,
}

impl ModuleContext {
    /// Create a new module context
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            search_paths: Vec::new(),
            cache: HashMap::new(),
            config: ModuleConfig::default(),
        }
    }
    
    /// Create a new module context with configuration
    pub fn with_config(config: ModuleConfig) -> Self {
        Self {
            modules: HashMap::new(),
            search_paths: Vec::new(),
            cache: HashMap::new(),
            config,
        }
    }
    
    /// Add a module search path
    pub fn add_search_path(&mut self, path: PathBuf) {
        self.search_paths.push(path);
    }
    
    /// Load a module
    pub fn load_module(&mut self, name: &str) -> Result<ModuleLoadingResult, ModuleLoadingError> {
        // Check if module is already loaded
        if let Some(module) = self.modules.get(name) {
            return Ok(ModuleLoadingResult {
                success: true,
                name: name.to_string(),
                exports: module.exports.clone(),
                dependencies: module.dependencies.clone(),
                warnings: Vec::new(),
                errors: Vec::new(),
            });
        }
        
        // Search for module
        let module_path = self.find_module(name)?;
        
        // Load module
        let module = self.load_module_from_path(&module_path)?;
        
        // Cache module
        self.modules.insert(name.to_string(), module.clone());
        
        Ok(ModuleLoadingResult {
            success: true,
            name: name.to_string(),
            exports: module.exports,
            dependencies: module.dependencies,
            warnings: Vec::new(),
            errors: Vec::new(),
        })
    }
    
    /// Find module in search paths
    fn find_module(&self, name: &str) -> Result<PathBuf, ModuleLoadingError> {
        for search_path in &self.search_paths {
            let module_path = search_path.join(format!("{}.cmod", name));
            if module_path.exists() {
                return Ok(module_path);
            }
            
            let module_path = search_path.join(format!("{}.cjmod", name));
            if module_path.exists() {
                return Ok(module_path);
            }
            
            let module_path = search_path.join(format!("{}/module.cmod", name));
            if module_path.exists() {
                return Ok(module_path);
            }
        }
        
        Err(ModuleLoadingError {
            message: format!("Module '{}' not found in search paths", name),
            module_name: name.to_string(),
            suggestion: Some("Add module path or check module name".to_string()),
        })
    }
    
    /// Load module from path
    fn load_module_from_path(&self, path: &PathBuf) -> Result<Module, ModuleLoadingError> {
        let name = path.file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("unknown")
            .to_string();
        
        let version = self.extract_version(path)?;
        let module_type = self.detect_module_type(path)?;
        
        let mut module = Module {
            name,
            version,
            path: path.clone(),
            module_type,
            dependencies: Vec::new(),
            exports: Vec::new(),
            content: String::new(),
            metadata: HashMap::new(),
        };
        
        // Load module content
        module.content = std::fs::read_to_string(path)
            .map_err(|e| ModuleLoadingError {
                message: format!("Failed to read module file: {}", e),
                module_name: module.name.clone(),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        // Parse module
        self.parse_module(&mut module)?;
        
        Ok(module)
    }
    
    /// Extract module version
    fn extract_version(&self, path: &PathBuf) -> Result<String, ModuleLoadingError> {
        // This is a simplified implementation
        // In a real implementation, you would extract version from module metadata
        Ok("1.0.0".to_string())
    }
    
    /// Detect module type
    fn detect_module_type(&self, path: &PathBuf) -> Result<ModuleType, ModuleLoadingError> {
        let extension = path.extension()
            .and_then(|s| s.to_str())
            .unwrap_or("");
        
        match extension {
            "cmod" => Ok(ModuleType::CMOD),
            "cjmod" => Ok(ModuleType::CJMOD),
            _ => {
                // Check if it's a hybrid module
                if path.join("module.cmod").exists() && path.join("module.cjmod").exists() {
                    Ok(ModuleType::Hybrid)
                } else {
                    Ok(ModuleType::External)
                }
            }
        }
    }
    
    /// Parse module
    fn parse_module(&self, module: &mut Module) -> Result<(), ModuleLoadingError> {
        // This is a simplified implementation
        // In a real implementation, you would parse the module file
        
        match module.module_type {
            ModuleType::CMOD => {
                self.parse_cmod_module(module)?;
            }
            ModuleType::CJMOD => {
                self.parse_cjmod_module(module)?;
            }
            ModuleType::Hybrid => {
                self.parse_hybrid_module(module)?;
            }
            ModuleType::External => {
                self.parse_external_module(module)?;
            }
        }
        
        Ok(())
    }
    
    /// Parse CMOD module
    fn parse_cmod_module(&self, module: &mut Module) -> Result<(), ModuleLoadingError> {
        // Parse CMOD module content
        for line in module.content.lines() {
            let line = line.trim();
            
            if line.starts_with("export ") {
                if let Some(export) = self.extract_export(line) {
                    module.exports.push(export);
                }
            } else if line.starts_with("import ") {
                if let Some(dependency) = self.extract_dependency(line) {
                    module.dependencies.push(dependency);
                }
            }
        }
        
        Ok(())
    }
    
    /// Parse CJMOD module
    fn parse_cjmod_module(&self, module: &mut Module) -> Result<(), ModuleLoadingError> {
        // Parse CJMOD module content
        for line in module.content.lines() {
            let line = line.trim();
            
            if line.starts_with("export ") {
                if let Some(export) = self.extract_export(line) {
                    module.exports.push(export);
                }
            } else if line.starts_with("import ") {
                if let Some(dependency) = self.extract_dependency(line) {
                    module.dependencies.push(dependency);
                }
            }
        }
        
        Ok(())
    }
    
    /// Parse hybrid module
    fn parse_hybrid_module(&self, module: &mut Module) -> Result<(), ModuleLoadingError> {
        // Parse both CMOD and CJMOD parts
        self.parse_cmod_module(module)?;
        self.parse_cjmod_module(module)?;
        
        Ok(())
    }
    
    /// Parse external module
    fn parse_external_module(&self, module: &mut Module) -> Result<(), ModuleLoadingError> {
        // Parse external module content
        for line in module.content.lines() {
            let line = line.trim();
            
            if line.starts_with("export ") {
                if let Some(export) = self.extract_export(line) {
                    module.exports.push(export);
                }
            } else if line.starts_with("import ") {
                if let Some(dependency) = self.extract_dependency(line) {
                    module.dependencies.push(dependency);
                }
            }
        }
        
        Ok(())
    }
    
    /// Extract export from line
    fn extract_export(&self, line: &str) -> Option<String> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 {
            return Some(parts[1].to_string());
        }
        None
    }
    
    /// Extract dependency from line
    fn extract_dependency(&self, line: &str) -> Option<String> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 {
            return Some(parts[1].to_string());
        }
        None
    }
    
    /// Get module
    pub fn get_module(&self, name: &str) -> Option<&Module> {
        self.modules.get(name)
    }
    
    /// Get all modules
    pub fn get_modules(&self) -> &HashMap<String, Module> {
        &self.modules
    }
    
    /// Check if module is loaded
    pub fn is_module_loaded(&self, name: &str) -> bool {
        self.modules.contains_key(name)
    }
    
    /// Unload module
    pub fn unload_module(&mut self, name: &str) -> Result<(), ModuleLoadingError> {
        if self.modules.remove(name).is_some() {
            Ok(())
        } else {
            Err(ModuleLoadingError {
                message: format!("Module '{}' is not loaded", name),
                module_name: name.to_string(),
                suggestion: Some("Check module name".to_string()),
            })
        }
    }
    
    /// Clear all modules
    pub fn clear_modules(&mut self) {
        self.modules.clear();
        self.cache.clear();
    }
    
    /// Set configuration
    pub fn set_config(&mut self, config: ModuleConfig) {
        self.config = config;
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &ModuleConfig {
        &self.config
    }
}

impl Module {
    /// Create a new module
    pub fn new(name: String, version: String, path: PathBuf, module_type: ModuleType) -> Self {
        Self {
            name,
            version,
            path,
            module_type,
            dependencies: Vec::new(),
            exports: Vec::new(),
            content: String::new(),
            metadata: HashMap::new(),
        }
    }
    
    /// Add a dependency
    pub fn add_dependency(&mut self, dependency: String) {
        self.dependencies.push(dependency);
    }
    
    /// Add an export
    pub fn add_export(&mut self, export: String) {
        self.exports.push(export);
    }
    
    /// Set module content
    pub fn set_content(&mut self, content: String) {
        self.content = content;
    }
    
    /// Set metadata
    pub fn set_metadata(&mut self, key: String, value: String) {
        self.metadata.insert(key, value);
    }
    
    /// Get module name
    pub fn get_name(&self) -> &String {
        &self.name
    }
    
    /// Get module version
    pub fn get_version(&self) -> &String {
        &self.version
    }
    
    /// Get module path
    pub fn get_path(&self) -> &PathBuf {
        &self.path
    }
    
    /// Get module type
    pub fn get_module_type(&self) -> &ModuleType {
        &self.module_type
    }
    
    /// Get dependencies
    pub fn get_dependencies(&self) -> &Vec<String> {
        &self.dependencies
    }
    
    /// Get exports
    pub fn get_exports(&self) -> &Vec<String> {
        &self.exports
    }
    
    /// Get module content
    pub fn get_content(&self) -> &String {
        &self.content
    }
    
    /// Get metadata
    pub fn get_metadata(&self) -> &HashMap<String, String> {
        &self.metadata
    }
}

impl Default for ModuleContext {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for ModuleConfig {
    fn default() -> Self {
        Self {
            enable_caching: true,
            cache_timeout: Some(std::time::Duration::from_secs(3600)),
            enable_validation: true,
            enable_dependency_resolution: true,
            search_paths: Vec::new(),
            allowed_types: vec![ModuleType::CMOD, ModuleType::CJMOD, ModuleType::Hybrid],
        }
    }
}