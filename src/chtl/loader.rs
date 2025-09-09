//! CHTL file loader
//! 
//! This module provides file loading functionality for CHTL,
//! including source file loading, module resolution, and dependency management.

use std::path::{Path, PathBuf};
use std::fs;
use std::collections::HashMap;
use walkdir::WalkDir;
use glob::Pattern;

use crate::chtl::context::*;

/// CHTL file loader
#[derive(Debug, Clone)]
pub struct CHTLLoader {
    /// Base directory for file resolution
    base_dir: PathBuf,
    /// Module search paths
    search_paths: Vec<PathBuf>,
    /// Loaded files cache
    loaded_files: HashMap<PathBuf, LoadedFile>,
    /// Module registry
    module_registry: HashMap<String, ModuleInfo>,
    /// Dependency graph
    dependency_graph: HashMap<PathBuf, Vec<PathBuf>>,
}

/// Loaded file information
#[derive(Debug, Clone)]
pub struct LoadedFile {
    /// File path
    pub path: PathBuf,
    /// File content
    pub content: String,
    /// File type
    pub file_type: FileType,
    /// Dependencies
    pub dependencies: Vec<PathBuf>,
    /// Last modified time
    pub last_modified: std::time::SystemTime,
    /// File size
    pub size: u64,
}

/// File type enumeration
#[derive(Debug, Clone, PartialEq)]
pub enum FileType {
    CHTL,
    HTML,
    CSS,
    JavaScript,
    CMOD,
    CJMOD,
    Unknown,
}

/// Module information
#[derive(Debug, Clone)]
pub struct ModuleInfo {
    /// Module name
    pub name: String,
    /// Module path
    pub path: PathBuf,
    /// Module type
    pub module_type: ModuleType,
    /// Module exports
    pub exports: Vec<String>,
    /// Module dependencies
    pub dependencies: Vec<String>,
    /// Module metadata
    pub metadata: HashMap<String, String>,
}

/// Loader error
#[derive(Debug, Clone)]
pub struct LoaderError {
    pub message: String,
    pub path: Option<PathBuf>,
    pub suggestion: Option<String>,
}

impl CHTLLoader {
    /// Create a new CHTL loader
    pub fn new(base_dir: PathBuf) -> Self {
        Self {
            base_dir,
            search_paths: Vec::new(),
            loaded_files: HashMap::new(),
            module_registry: HashMap::new(),
            dependency_graph: HashMap::new(),
        }
    }
    
    /// Add a search path
    pub fn add_search_path(&mut self, path: PathBuf) {
        self.search_paths.push(path);
    }
    
    /// Load a file by path
    pub fn load_file(&mut self, path: &Path) -> Result<&LoadedFile, LoaderError> {
        let path = self.resolve_path(path)?;
        
        // Check if file is already loaded
        if let Some(file) = self.loaded_files.get(&path) {
            return Ok(file);
        }
        
        // Load file from filesystem
        let content = fs::read_to_string(&path)
            .map_err(|e| LoaderError {
                message: format!("Failed to read file: {}", e),
                path: Some(path.clone()),
                suggestion: Some("Check if file exists and is readable".to_string()),
            })?;
        
        let file_type = self.detect_file_type(&path);
        let last_modified = fs::metadata(&path)
            .map_err(|e| LoaderError {
                message: format!("Failed to get file metadata: {}", e),
                path: Some(path.clone()),
                suggestion: None,
            })?
            .modified()
            .map_err(|e| LoaderError {
                message: format!("Failed to get file modification time: {}", e),
                path: Some(path.clone()),
                suggestion: None,
            })?;
        
        let size = fs::metadata(&path)
            .map_err(|e| LoaderError {
                message: format!("Failed to get file metadata: {}", e),
                path: Some(path.clone()),
                suggestion: None,
            })?
            .len();
        
        let dependencies = self.extract_dependencies(&content, &file_type)?;
        
        let loaded_file = LoadedFile {
            path: path.clone(),
            content,
            file_type,
            dependencies,
            last_modified,
            size,
        };
        
        self.loaded_files.insert(path.clone(), loaded_file);
        
        Ok(self.loaded_files.get(&path).unwrap())
    }
    
    /// Load a file by name (with search paths)
    pub fn load_file_by_name(&mut self, name: &str) -> Result<&LoadedFile, LoaderError> {
        // Try direct path first
        if let Ok(path) = self.resolve_path(Path::new(name)) {
            return self.load_file(&path);
        }
        
        // Search in search paths
        for search_path in &self.search_paths {
            let full_path = search_path.join(name);
            if let Ok(path) = self.resolve_path(&full_path) {
                return self.load_file(&path);
            }
        }
        
        Err(LoaderError {
            message: format!("File not found: {}", name),
            path: None,
            suggestion: Some("Check file name and search paths".to_string()),
        })
    }
    
    /// Load a module
    pub fn load_module(&mut self, module_name: &str) -> Result<&ModuleInfo, LoaderError> {
        // Check if module is already loaded
        if let Some(module) = self.module_registry.get(module_name) {
            return Ok(module);
        }
        
        // Find module file
        let module_path = self.find_module(module_name)?;
        
        // Load module file
        let loaded_file = self.load_file(&module_path)?;
        
        // Parse module information
        let module_info = self.parse_module_info(loaded_file)?;
        
        self.module_registry.insert(module_name.to_string(), module_info);
        
        Ok(self.module_registry.get(module_name).unwrap())
    }
    
    /// Load all files in a directory
    pub fn load_directory(&mut self, dir_path: &Path) -> Result<Vec<&LoadedFile>, LoaderError> {
        let mut loaded_files = Vec::new();
        
        for entry in WalkDir::new(dir_path) {
            let entry = entry.map_err(|e| LoaderError {
                message: format!("Failed to read directory entry: {}", e),
                path: Some(dir_path.to_path_buf()),
                suggestion: None,
            })?;
            
            if entry.file_type().is_file() {
                let file_type = self.detect_file_type(entry.path());
                if matches!(file_type, FileType::CHTL | FileType::HTML | FileType::CSS | FileType::JavaScript) {
                    let loaded_file = self.load_file(entry.path())?;
                    loaded_files.push(loaded_file);
                }
            }
        }
        
        Ok(loaded_files)
    }
    
    /// Load files matching a pattern
    pub fn load_files_by_pattern(&mut self, pattern: &str) -> Result<Vec<&LoadedFile>, LoaderError> {
        let mut loaded_files = Vec::new();
        
        for search_path in &self.search_paths {
            for entry in WalkDir::new(search_path) {
                let entry = entry.map_err(|e| LoaderError {
                    message: format!("Failed to read directory entry: {}", e),
                    path: Some(search_path.clone()),
                    suggestion: None,
                })?;
                
                if entry.file_type().is_file() {
                    let path_str = entry.path().to_string_lossy();
                    if Pattern::new(pattern)
                        .map_err(|e| LoaderError {
                            message: format!("Invalid pattern: {}", e),
                            path: None,
                            suggestion: Some("Check pattern syntax".to_string()),
                        })?
                        .matches(&path_str)
                    {
                        let loaded_file = self.load_file(entry.path())?;
                        loaded_files.push(loaded_file);
                    }
                }
            }
        }
        
        Ok(loaded_files)
    }
    
    /// Resolve a file path
    fn resolve_path(&self, path: &Path) -> Result<PathBuf, LoaderError> {
        if path.is_absolute() {
            Ok(path.to_path_buf())
        } else {
            let resolved = self.base_dir.join(path);
            if resolved.exists() {
                Ok(resolved.canonicalize().map_err(|e| LoaderError {
                    message: format!("Failed to canonicalize path: {}", e),
                    path: Some(resolved),
                    suggestion: None,
                })?)
            } else {
                Err(LoaderError {
                    message: format!("File not found: {}", path.display()),
                    path: Some(path.to_path_buf()),
                    suggestion: Some("Check file path".to_string()),
                })
            }
        }
    }
    
    /// Detect file type from extension
    fn detect_file_type(&self, path: &Path) -> FileType {
        match path.extension().and_then(|s| s.to_str()) {
            Some("chtl") => FileType::CHTL,
            Some("html") | Some("htm") => FileType::HTML,
            Some("css") => FileType::CSS,
            Some("js") | Some("mjs") => FileType::JavaScript,
            Some("cmod") => FileType::CMOD,
            Some("cjmod") => FileType::CJMOD,
            _ => FileType::Unknown,
        }
    }
    
    /// Extract dependencies from file content
    fn extract_dependencies(&self, content: &str, file_type: &FileType) -> Result<Vec<PathBuf>, LoaderError> {
        let mut dependencies = Vec::new();
        
        match file_type {
            FileType::CHTL => {
                // Extract [Import] statements
                let import_pattern = regex::Regex::new(r"\[Import\]\s+@\w+\s+from\s+([^\s;]+)")
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile import regex: {}", e),
                        path: None,
                        suggestion: None,
                    })?;
                
                for cap in import_pattern.captures_iter(content) {
                    if let Some(path_str) = cap.get(1) {
                        let path = PathBuf::from(path_str.as_str());
                        dependencies.push(path);
                    }
                }
            }
            FileType::HTML => {
                // Extract <link> and <script> src attributes
                let link_pattern = regex::Regex::new(r#"<(?:link|script)\s+[^>]*src=["']([^"']+)["']"#)
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile link regex: {}", e),
                        path: None,
                        suggestion: None,
                    })?;
                
                for cap in link_pattern.captures_iter(content) {
                    if let Some(path_str) = cap.get(1) {
                        let path = PathBuf::from(path_str.as_str());
                        dependencies.push(path);
                    }
                }
            }
            FileType::CSS => {
                // Extract @import statements
                let import_pattern = regex::Regex::new(r#"@import\s+['"]([^'"]+)['"]"#)
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile CSS import regex: {}", e),
                        path: None,
                        suggestion: None,
                    })?;
                
                for cap in import_pattern.captures_iter(content) {
                    if let Some(path_str) = cap.get(1) {
                        let path = PathBuf::from(path_str.as_str());
                        dependencies.push(path);
                    }
                }
            }
            FileType::JavaScript => {
                // Extract import/require statements
                let import_pattern = regex::Regex::new(r#"(?:import\s+.*\s+from\s+|require\s*\(\s*)['"]([^'"]+)['"]"#)
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile JS import regex: {}", e),
                        path: None,
                        suggestion: None,
                    })?;
                
                for cap in import_pattern.captures_iter(content) {
                    if let Some(path_str) = cap.get(1) {
                        let path = PathBuf::from(path_str.as_str());
                        dependencies.push(path);
                    }
                }
            }
            _ => {}
        }
        
        Ok(dependencies)
    }
    
    /// Find a module by name
    fn find_module(&self, module_name: &str) -> Result<PathBuf, LoaderError> {
        // Try direct path first
        let direct_path = PathBuf::from(module_name);
        if direct_path.exists() {
            return Ok(direct_path);
        }
        
        // Search in search paths
        for search_path in &self.search_paths {
            let module_path = search_path.join(module_name);
            if module_path.exists() {
                return Ok(module_path);
            }
            
            // Try with different extensions
            for ext in &["chtl", "cmod", "cjmod"] {
                let module_path = search_path.join(format!("{}.{}", module_name, ext));
                if module_path.exists() {
                    return Ok(module_path);
                }
            }
        }
        
        Err(LoaderError {
            message: format!("Module not found: {}", module_name),
            path: None,
            suggestion: Some("Check module name and search paths".to_string()),
        })
    }
    
    /// Parse module information
    fn parse_module_info(&self, loaded_file: &LoadedFile) -> Result<ModuleInfo, LoaderError> {
        let mut exports = Vec::new();
        let mut dependencies = Vec::new();
        let mut metadata = HashMap::new();
        
        match loaded_file.file_type {
            FileType::CHTL => {
                // Parse CHTL-specific module information
                // This would involve parsing the AST to extract exports
                // For now, we'll do a simple text-based extraction
                
                // Extract [Template] definitions
                let template_pattern = regex::Regex::new(r"\[Template\]\s+@\w+\s+(\w+)")
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile template regex: {}", e),
                        path: Some(loaded_file.path.clone()),
                        suggestion: None,
                    })?;
                
                for cap in template_pattern.captures_iter(&loaded_file.content) {
                    if let Some(name) = cap.get(1) {
                        exports.push(name.as_str().to_string());
                    }
                }
                
                // Extract [Custom] definitions
                let custom_pattern = regex::Regex::new(r"\[Custom\]\s+@\w+\s+(\w+)")
                    .map_err(|e| LoaderError {
                        message: format!("Failed to compile custom regex: {}", e),
                        path: Some(loaded_file.path.clone()),
                        suggestion: None,
                    })?;
                
                for cap in custom_pattern.captures_iter(&loaded_file.content) {
                    if let Some(name) = cap.get(1) {
                        exports.push(name.as_str().to_string());
                    }
                }
            }
            _ => {
                // For non-CHTL files, use filename as export
                if let Some(name) = loaded_file.path.file_stem().and_then(|s| s.to_str()) {
                    exports.push(name.to_string());
                }
            }
        }
        
        // Convert dependencies to strings
        for dep in &loaded_file.dependencies {
            if let Some(dep_str) = dep.to_str() {
                dependencies.push(dep_str.to_string());
            }
        }
        
        // Extract metadata from file
        metadata.insert("size".to_string(), loaded_file.size.to_string());
        metadata.insert("last_modified".to_string(), 
            loaded_file.last_modified.duration_since(std::time::UNIX_EPOCH)
                .unwrap_or_default()
                .as_secs()
                .to_string());
        
        Ok(ModuleInfo {
            name: loaded_file.path.file_stem()
                .and_then(|s| s.to_str())
                .unwrap_or("unknown")
                .to_string(),
            path: loaded_file.path.clone(),
            module_type: self.file_type_to_module_type(&loaded_file.file_type),
            exports,
            dependencies,
            metadata,
        })
    }
    
    /// Convert file type to module type
    fn file_type_to_module_type(&self, file_type: &FileType) -> ModuleType {
        match file_type {
            FileType::CHTL => ModuleType::CHTL,
            FileType::HTML => ModuleType::HTML,
            FileType::CSS => ModuleType::CSS,
            FileType::JavaScript => ModuleType::JavaScript,
            FileType::CMOD => ModuleType::CMOD,
            FileType::CJMOD => ModuleType::CJMOD,
            _ => ModuleType::CHTL, // Default
        }
    }
    
    /// Get loaded file
    pub fn get_loaded_file(&self, path: &Path) -> Option<&LoadedFile> {
        self.loaded_files.get(path)
    }
    
    /// Get module info
    pub fn get_module_info(&self, name: &str) -> Option<&ModuleInfo> {
        self.module_registry.get(name)
    }
    
    /// Get all loaded files
    pub fn get_loaded_files(&self) -> &HashMap<PathBuf, LoadedFile> {
        &self.loaded_files
    }
    
    /// Get all modules
    pub fn get_modules(&self) -> &HashMap<String, ModuleInfo> {
        &self.module_registry
    }
    
    /// Clear loaded files cache
    pub fn clear_cache(&mut self) {
        self.loaded_files.clear();
        self.module_registry.clear();
        self.dependency_graph.clear();
    }
    
    /// Check if file is loaded
    pub fn is_loaded(&self, path: &Path) -> bool {
        self.loaded_files.contains_key(path)
    }
    
    /// Get file dependencies
    pub fn get_dependencies(&self, path: &Path) -> Option<&Vec<PathBuf>> {
        self.dependency_graph.get(path)
    }
}

impl Default for CHTLLoader {
    fn default() -> Self {
        Self::new(PathBuf::from("."))
    }
}