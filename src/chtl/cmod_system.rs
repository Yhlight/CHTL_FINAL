//! CHTL CMOD system
//! 
//! This module provides CMOD (CHTL Module) functionality,
//! including module creation, packaging, and management.

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::fs;
use serde::{Deserialize, Serialize};
use zip::ZipWriter;
use std::io::Write;

use crate::chtl::context::*;

/// CHTL CMOD system
#[derive(Debug, Clone)]
pub struct CMODSystem {
    /// Module registry
    modules: HashMap<String, CMODModule>,
    /// Module search paths
    search_paths: Vec<PathBuf>,
    /// Module cache
    cache: HashMap<String, CMODModule>,
}

/// CMOD module
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CMODModule {
    /// Module name
    pub name: String,
    /// Module version
    pub version: String,
    /// Module description
    pub description: String,
    /// Module author
    pub author: String,
    /// Module license
    pub license: String,
    /// Module dependencies
    pub dependencies: Vec<String>,
    /// Module category
    pub category: String,
    /// Minimum CHTL version
    pub min_chtl_version: String,
    /// Maximum CHTL version
    pub max_chtl_version: String,
    /// Module source files
    pub source_files: Vec<PathBuf>,
    /// Module info file
    pub info_file: PathBuf,
    /// Module exports
    pub exports: CMODExports,
    /// Module metadata
    pub metadata: HashMap<String, String>,
}

/// CMOD exports
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CMODExports {
    /// Custom styles
    pub custom_styles: Vec<String>,
    /// Custom elements
    pub custom_elements: Vec<String>,
    /// Custom variables
    pub custom_variables: Vec<String>,
    /// Template styles
    pub template_styles: Vec<String>,
    /// Template elements
    pub template_elements: Vec<String>,
    /// Template variables
    pub template_variables: Vec<String>,
    /// Origin HTML
    pub origin_html: Vec<String>,
    /// Origin CSS
    pub origin_css: Vec<String>,
    /// Origin JavaScript
    pub origin_javascript: Vec<String>,
    /// Origin custom types
    pub origin_custom: Vec<String>,
    /// Configuration
    pub configuration: Vec<String>,
}

/// CMOD builder
#[derive(Debug, Clone)]
pub struct CMODBuilder {
    /// Module being built
    module: CMODModule,
    /// Source directory
    source_dir: PathBuf,
    /// Output directory
    output_dir: PathBuf,
}

/// CMOD error
#[derive(Debug, Clone)]
pub struct CMODError {
    pub message: String,
    pub path: Option<PathBuf>,
    pub suggestion: Option<String>,
}

impl CMODSystem {
    /// Create a new CMOD system
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            search_paths: Vec::new(),
            cache: HashMap::new(),
        }
    }
    
    /// Add a search path
    pub fn add_search_path(&mut self, path: PathBuf) {
        self.search_paths.push(path);
    }
    
    /// Load a CMOD module
    pub fn load_module(&mut self, module_name: &str) -> Result<&CMODModule, CMODError> {
        // Check cache first
        if let Some(module) = self.cache.get(module_name) {
            return Ok(module);
        }
        
        // Find module file
        let module_path = self.find_module_file(module_name)?;
        
        // Load module from file
        let module = self.load_module_from_file(&module_path)?;
        
        // Cache module
        self.cache.insert(module_name.to_string(), module.clone());
        self.modules.insert(module_name.to_string(), module);
        
        Ok(self.modules.get(module_name).unwrap())
    }
    
    /// Load a CMOD module from file
    pub fn load_module_from_file(&self, file_path: &Path) -> Result<CMODModule, CMODError> {
        // Read module file
        let content = fs::read_to_string(file_path)
            .map_err(|e| CMODError {
                message: format!("Failed to read module file: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        // Parse module file
        let module: CMODModule = serde_json::from_str(&content)
            .map_err(|e| CMODError {
                message: format!("Failed to parse module file: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: Some("Check module file format".to_string()),
            })?;
        
        Ok(module)
    }
    
    /// Save a CMOD module to file
    pub fn save_module_to_file(&self, module: &CMODModule, file_path: &Path) -> Result<(), CMODError> {
        // Serialize module
        let content = serde_json::to_string_pretty(module)
            .map_err(|e| CMODError {
                message: format!("Failed to serialize module: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: None,
            })?;
        
        // Write to file
        fs::write(file_path, content)
            .map_err(|e| CMODError {
                message: format!("Failed to write module file: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        Ok(())
    }
    
    /// Create a CMOD package
    pub fn create_package(&self, module: &CMODModule, output_path: &Path) -> Result<(), CMODError> {
        // Create ZIP file
        let file = fs::File::create(output_path)
            .map_err(|e| CMODError {
                message: format!("Failed to create package file: {}", e),
                path: Some(output_path.to_path_buf()),
                suggestion: Some("Check output path and permissions".to_string()),
            })?;
        
        let mut zip = ZipWriter::new(file);
        
        // Add module info file
        let info_content = serde_json::to_string_pretty(module)
            .map_err(|e| CMODError {
                message: format!("Failed to serialize module info: {}", e),
                path: Some(output_path.to_path_buf()),
                suggestion: None,
            })?;
        
        zip.start_file("info.json", zip::write::FileOptions::default())
            .map_err(|e| CMODError {
                message: format!("Failed to add info file to package: {}", e),
                path: Some(output_path.to_path_buf()),
                suggestion: None,
            })?;
        
        zip.write_all(info_content.as_bytes())
            .map_err(|e| CMODError {
                message: format!("Failed to write info file to package: {}", e),
                path: Some(output_path.to_path_buf()),
                suggestion: None,
            })?;
        
        // Add source files
        for source_file in &module.source_files {
            if source_file.exists() {
                let content = fs::read_to_string(source_file)
                    .map_err(|e| CMODError {
                        message: format!("Failed to read source file: {}", e),
                        path: Some(source_file.clone()),
                        suggestion: None,
                    })?;
                
                let file_name = source_file.file_name()
                    .and_then(|n| n.to_str())
                    .ok_or_else(|| CMODError {
                        message: "Invalid source file name".to_string(),
                        path: Some(source_file.clone()),
                        suggestion: None,
                    })?;
                
                zip.start_file(format!("src/{}", file_name), zip::write::FileOptions::default())
                    .map_err(|e| CMODError {
                        message: format!("Failed to add source file to package: {}", e),
                        path: Some(output_path.to_path_buf()),
                        suggestion: None,
                    })?;
                
                zip.write_all(content.as_bytes())
                    .map_err(|e| CMODError {
                        message: format!("Failed to write source file to package: {}", e),
                        path: Some(output_path.to_path_buf()),
                        suggestion: None,
                    })?;
            }
        }
        
        // Finish ZIP file
        zip.finish()
            .map_err(|e| CMODError {
                message: format!("Failed to finish package: {}", e),
                path: Some(output_path.to_path_buf()),
                suggestion: None,
            })?;
        
        Ok(())
    }
    
    /// Extract a CMOD package
    pub fn extract_package(&self, package_path: &Path, extract_to: &Path) -> Result<(), CMODError> {
        // Open ZIP file
        let file = fs::File::open(package_path)
            .map_err(|e| CMODError {
                message: format!("Failed to open package file: {}", e),
                path: Some(package_path.to_path_buf()),
                suggestion: Some("Check package file path".to_string()),
            })?;
        
        let mut zip = zip::ZipArchive::new(file)
            .map_err(|e| CMODError {
                message: format!("Failed to read package file: {}", e),
                path: Some(package_path.to_path_buf()),
                suggestion: Some("Check package file format".to_string()),
            })?;
        
        // Extract all files
        for i in 0..zip.len() {
            let mut file = zip.by_index(i)
                .map_err(|e| CMODError {
                    message: format!("Failed to read file from package: {}", e),
                    path: Some(package_path.to_path_buf()),
                    suggestion: None,
                })?;
            
            let file_path = extract_to.join(file.name());
            
            // Create parent directories
            if let Some(parent) = file_path.parent() {
                fs::create_dir_all(parent)
                    .map_err(|e| CMODError {
                        message: format!("Failed to create directory: {}", e),
                        path: Some(parent.to_path_buf()),
                        suggestion: None,
                    })?;
            }
            
            // Extract file
            let mut content = Vec::new();
            file.read_to_end(&mut content)
                .map_err(|e| CMODError {
                    message: format!("Failed to read file content: {}", e),
                    path: Some(package_path.to_path_buf()),
                    suggestion: None,
                })?;
            
            fs::write(&file_path, content)
                .map_err(|e| CMODError {
                    message: format!("Failed to write extracted file: {}", e),
                    path: Some(file_path),
                    suggestion: None,
                })?;
        }
        
        Ok(())
    }
    
    /// Find module file
    fn find_module_file(&self, module_name: &str) -> Result<PathBuf, CMODError> {
        // Try direct path first
        let direct_path = PathBuf::from(module_name);
        if direct_path.exists() {
            return Ok(direct_path);
        }
        
        // Search in search paths
        for search_path in &self.search_paths {
            let module_path = search_path.join(format!("{}.cmod", module_name));
            if module_path.exists() {
                return Ok(module_path);
            }
        }
        
        Err(CMODError {
            message: format!("Module not found: {}", module_name),
            path: None,
            suggestion: Some("Check module name and search paths".to_string()),
        })
    }
    
    /// Get module
    pub fn get_module(&self, name: &str) -> Option<&CMODModule> {
        self.modules.get(name)
    }
    
    /// Get all modules
    pub fn get_modules(&self) -> &HashMap<String, CMODModule> {
        &self.modules
    }
    
    /// Clear cache
    pub fn clear_cache(&mut self) {
        self.cache.clear();
    }
}

impl CMODBuilder {
    /// Create a new CMOD builder
    pub fn new(name: String, source_dir: PathBuf, output_dir: PathBuf) -> Self {
        Self {
            module: CMODModule {
                name,
                version: "1.0.0".to_string(),
                description: String::new(),
                author: String::new(),
                license: "MIT".to_string(),
                dependencies: Vec::new(),
                category: "general".to_string(),
                min_chtl_version: "1.0.0".to_string(),
                max_chtl_version: "2.0.0".to_string(),
                source_files: Vec::new(),
                info_file: PathBuf::new(),
                exports: CMODExports::default(),
                metadata: HashMap::new(),
            },
            source_dir,
            output_dir,
        }
    }
    
    /// Set module version
    pub fn set_version(mut self, version: String) -> Self {
        self.module.version = version;
        self
    }
    
    /// Set module description
    pub fn set_description(mut self, description: String) -> Self {
        self.module.description = description;
        self
    }
    
    /// Set module author
    pub fn set_author(mut self, author: String) -> Self {
        self.module.author = author;
        self
    }
    
    /// Set module license
    pub fn set_license(mut self, license: String) -> Self {
        self.module.license = license;
        self
    }
    
    /// Add dependency
    pub fn add_dependency(mut self, dependency: String) -> Self {
        self.module.dependencies.push(dependency);
        self
    }
    
    /// Set module category
    pub fn set_category(mut self, category: String) -> Self {
        self.module.category = category;
        self
    }
    
    /// Set CHTL version range
    pub fn set_chtl_version_range(mut self, min: String, max: String) -> Self {
        self.module.min_chtl_version = min;
        self.module.max_chtl_version = max;
        self
    }
    
    /// Add source file
    pub fn add_source_file(mut self, file_path: PathBuf) -> Self {
        self.module.source_files.push(file_path);
        self
    }
    
    /// Set info file
    pub fn set_info_file(mut self, info_file: PathBuf) -> Self {
        self.module.info_file = info_file;
        self
    }
    
    /// Set exports
    pub fn set_exports(mut self, exports: CMODExports) -> Self {
        self.module.exports = exports;
        self
    }
    
    /// Add metadata
    pub fn add_metadata(mut self, key: String, value: String) -> Self {
        self.module.metadata.insert(key, value);
        self
    }
    
    /// Build the module
    pub fn build(self) -> Result<CMODModule, CMODError> {
        // Validate module
        self.validate_module()?;
        
        Ok(self.module)
    }
    
    /// Validate module
    fn validate_module(&self) -> Result<(), CMODError> {
        // Check if source directory exists
        if !self.source_dir.exists() {
            return Err(CMODError {
                message: "Source directory does not exist".to_string(),
                path: Some(self.source_dir.clone()),
                suggestion: Some("Create source directory first".to_string()),
            });
        }
        
        // Check if source files exist
        for source_file in &self.module.source_files {
            if !source_file.exists() {
                return Err(CMODError {
                    message: "Source file does not exist".to_string(),
                    path: Some(source_file.clone()),
                    suggestion: Some("Check source file path".to_string()),
                });
            }
        }
        
        // Check if info file exists
        if !self.module.info_file.exists() {
            return Err(CMODError {
                message: "Info file does not exist".to_string(),
                path: Some(self.module.info_file.clone()),
                suggestion: Some("Create info file first".to_string()),
            });
        }
        
        Ok(())
    }
}

impl Default for CMODSystem {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CMODExports {
    fn default() -> Self {
        Self {
            custom_styles: Vec::new(),
            custom_elements: Vec::new(),
            custom_variables: Vec::new(),
            template_styles: Vec::new(),
            template_elements: Vec::new(),
            template_variables: Vec::new(),
            origin_html: Vec::new(),
            origin_css: Vec::new(),
            origin_javascript: Vec::new(),
            origin_custom: Vec::new(),
            configuration: Vec::new(),
        }
    }
}