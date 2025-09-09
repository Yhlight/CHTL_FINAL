//! Module loader
//! 
//! This module provides module loading functionality for CHTL,
//! including module discovery, loading, and initialization.

use super::*;
use std::collections::HashMap;
use std::path::PathBuf;

/// Module loader
#[derive(Debug, Clone)]
pub struct ModuleLoader {
    /// Loaded modules
    modules: HashMap<String, Module>,
    /// Module search paths
    search_paths: Vec<PathBuf>,
    /// Module cache
    cache: HashMap<String, ModuleCache>,
    /// Configuration
    config: ModuleLoaderConfig,
}

/// Module loader configuration
#[derive(Debug, Clone)]
pub struct ModuleLoaderConfig {
    /// Enable module caching
    pub enable_caching: bool,
    /// Cache timeout
    pub cache_timeout: Option<std::time::Duration>,
    /// Enable module validation
    pub enable_validation: bool,
    /// Enable module discovery
    pub enable_discovery: bool,
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
    /// Loaded module
    pub module: Option<Module>,
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

impl ModuleLoader {
    /// Create a new module loader
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            search_paths: Vec::new(),
            cache: HashMap::new(),
            config: ModuleLoaderConfig::default(),
        }
    }
    
    /// Create a new module loader with configuration
    pub fn with_config(config: ModuleLoaderConfig) -> Self {
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
    
    /// Discover modules
    pub fn discover_modules(&mut self) -> Result<Vec<String>, ModuleLoadingError> {
        let mut discovered_modules = Vec::new();
        
        for search_path in &self.search_paths {
            if let Ok(entries) = std::fs::read_dir(search_path) {
                for entry in entries {
                    if let Ok(entry) = entry {
                        let path = entry.path();
                        
                        if path.is_file() {
                            if let Some(extension) = path.extension() {
                                if extension == "cmod" || extension == "cjmod" {
                                    if let Some(name) = path.file_stem().and_then(|s| s.to_str()) {
                                        discovered_modules.push(name.to_string());
                                    }
                                }
                            }
                        } else if path.is_dir() {
                            // Check for hybrid modules
                            let cmod_path = path.join("module.cmod");
                            let cjmod_path = path.join("module.cjmod");
                            
                            if cmod_path.exists() || cjmod_path.exists() {
                                if let Some(name) = path.file_name().and_then(|s| s.to_str()) {
                                    discovered_modules.push(name.to_string());
                                }
                            }
                        }
                    }
                }
            }
        }
        
        Ok(discovered_modules)
    }
    
    /// Load a module
    pub fn load_module(&mut self, name: &str) -> Result<ModuleLoadingResult, ModuleLoadingError> {
        let mut result = ModuleLoadingResult {
            success: false,
            module: None,
            dependencies: Vec::new(),
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        // Check if module is already loaded
        if let Some(module) = self.modules.get(name) {
            result.success = true;
            result.module = Some(module.clone());
            result.dependencies = module.dependencies.clone();
            return Ok(result);
        }
        
        // Check cache
        if self.config.enable_caching {
            if let Some(cached_module) = self.get_cached_module(name) {
                result.success = true;
                result.module = Some(cached_module);
                return Ok(result);
            }
        }
        
        // Find module
        let module_path = self.find_module(name)?;
        
        // Load module
        let module = self.load_module_from_path(&module_path)?;
        
        // Validate module
        if self.config.enable_validation {
            self.validate_module(&module, &mut result)?;
        }
        
        // Cache module
        if self.config.enable_caching {
            self.cache_module(&module);
        }
        
        // Add module
        self.modules.insert(name.to_string(), module.clone());
        result.module = Some(module);
        result.success = true;
        
        Ok(result)
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
        
        let mut module = Module::new(name, version, path.clone(), module_type);
        
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
    
    /// Validate module
    fn validate_module(&self, module: &Module, result: &mut ModuleLoadingResult) -> Result<(), ModuleLoadingError> {
        // Check if module name is valid
        if module.name.is_empty() {
            result.errors.push("Module name cannot be empty".to_string());
            return Err(ModuleLoadingError {
                message: "Module name cannot be empty".to_string(),
                module_name: module.name.clone(),
                suggestion: Some("Provide a valid module name".to_string()),
            });
        }
        
        // Check if module version is valid
        if module.version.is_empty() {
            result.warnings.push("Module version is empty".to_string());
        }
        
        // Check if module has exports
        if module.exports.is_empty() {
            result.warnings.push("Module has no exports".to_string());
        }
        
        // Check if module type is allowed
        if !self.config.allowed_types.contains(&module.module_type) {
            result.errors.push(format!("Module type {:?} is not allowed", module.module_type));
            return Err(ModuleLoadingError {
                message: format!("Module type {:?} is not allowed", module.module_type),
                module_name: module.name.clone(),
                suggestion: Some("Check module type configuration".to_string()),
            });
        }
        
        Ok(())
    }
    
    /// Get cached module
    fn get_cached_module(&self, name: &str) -> Option<Module> {
        if let Some(cache) = self.cache.get(name) {
            // Check if cache is still valid
            if let Ok(metadata) = std::fs::metadata(&cache.path) {
                if let Ok(modified) = metadata.modified() {
                    if modified <= cache.last_modified {
                        return Some(Module::new(
                            name.to_string(),
                            "1.0.0".to_string(),
                            cache.path.clone(),
                            ModuleType::CMOD,
                        ));
                    }
                }
            }
        }
        None
    }
    
    /// Cache module
    fn cache_module(&mut self, module: &Module) {
        self.cache.insert(module.name.clone(), ModuleCache {
            path: module.path.clone(),
            last_modified: std::time::SystemTime::now(),
            content: module.content.clone(),
            exports: module.exports.clone(),
        });
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
            self.cache.remove(name);
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
    pub fn set_config(&mut self, config: ModuleLoaderConfig) {
        self.config = config;
    }
    
    /// Get configuration
    pub fn get_config(&self) -> &ModuleLoaderConfig {
        &self.config
    }
}

impl Default for ModuleLoader {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for ModuleLoaderConfig {
    fn default() -> Self {
        Self {
            enable_caching: true,
            cache_timeout: Some(std::time::Duration::from_secs(3600)),
            enable_validation: true,
            enable_discovery: true,
            search_paths: Vec::new(),
            allowed_types: vec![ModuleType::CMOD, ModuleType::CJMOD, ModuleType::Hybrid],
        }
    }
}