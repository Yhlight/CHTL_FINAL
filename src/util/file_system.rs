//! File system utilities
//! 
//! This module provides file system utility functionality for CHTL,
//! including file operations, directory management, and path utilities.

use std::path::{Path, PathBuf};
use std::fs;
use std::io;
use walkdir::WalkDir;

/// File system utilities
#[derive(Debug, Clone)]
pub struct FileSystemUtils {
    /// Base directory
    base_dir: PathBuf,
    /// Current working directory
    current_dir: PathBuf,
}

/// File system error
#[derive(Debug, Clone)]
pub struct FileSystemError {
    pub message: String,
    pub path: Option<PathBuf>,
    pub suggestion: Option<String>,
}

impl FileSystemUtils {
    /// Create a new file system utils instance
    pub fn new() -> Self {
        Self {
            base_dir: PathBuf::from("."),
            current_dir: std::env::current_dir().unwrap_or_else(|_| PathBuf::from(".")),
        }
    }
    
    /// Create a new file system utils instance with base directory
    pub fn with_base_dir(base_dir: PathBuf) -> Self {
        Self {
            base_dir,
            current_dir: std::env::current_dir().unwrap_or_else(|_| PathBuf::from(".")),
        }
    }
    
    /// Get base directory
    pub fn get_base_dir(&self) -> &PathBuf {
        &self.base_dir
    }
    
    /// Set base directory
    pub fn set_base_dir(&mut self, base_dir: PathBuf) {
        self.base_dir = base_dir;
    }
    
    /// Get current working directory
    pub fn get_current_dir(&self) -> &PathBuf {
        &self.current_dir
    }
    
    /// Set current working directory
    pub fn set_current_dir(&mut self, current_dir: PathBuf) {
        self.current_dir = current_dir;
    }
    
    /// Check if a path exists
    pub fn exists(&self, path: &Path) -> bool {
        path.exists()
    }
    
    /// Check if a path is a file
    pub fn is_file(&self, path: &Path) -> bool {
        path.is_file()
    }
    
    /// Check if a path is a directory
    pub fn is_directory(&self, path: &Path) -> bool {
        path.is_dir()
    }
    
    /// Get file size
    pub fn get_file_size(&self, path: &Path) -> Result<u64, FileSystemError> {
        let metadata = fs::metadata(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to get file metadata: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        Ok(metadata.len())
    }
    
    /// Get file extension
    pub fn get_file_extension(&self, path: &Path) -> Option<String> {
        path.extension()
            .and_then(|ext| ext.to_str())
            .map(|ext| ext.to_string())
    }
    
    /// Get file name without extension
    pub fn get_file_stem(&self, path: &Path) -> Option<String> {
        path.file_stem()
            .and_then(|stem| stem.to_str())
            .map(|stem| stem.to_string())
    }
    
    /// Get file name
    pub fn get_file_name(&self, path: &Path) -> Option<String> {
        path.file_name()
            .and_then(|name| name.to_str())
            .map(|name| name.to_string())
    }
    
    /// Get parent directory
    pub fn get_parent_dir(&self, path: &Path) -> Option<PathBuf> {
        path.parent().map(|p| p.to_path_buf())
    }
    
    /// Read file contents
    pub fn read_file(&self, path: &Path) -> Result<String, FileSystemError> {
        fs::read_to_string(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to read file: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })
    }
    
    /// Write file contents
    pub fn write_file(&self, path: &Path, contents: &str) -> Result<(), FileSystemError> {
        // Create parent directories if they don't exist
        if let Some(parent) = path.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| FileSystemError {
                    message: format!("Failed to create parent directories: {}", e),
                    path: Some(parent.to_path_buf()),
                    suggestion: Some("Check directory permissions".to_string()),
                })?;
        }
        
        fs::write(path, contents)
            .map_err(|e| FileSystemError {
                message: format!("Failed to write file: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })
    }
    
    /// Copy file
    pub fn copy_file(&self, from: &Path, to: &Path) -> Result<(), FileSystemError> {
        // Create parent directories if they don't exist
        if let Some(parent) = to.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| FileSystemError {
                    message: format!("Failed to create parent directories: {}", e),
                    path: Some(parent.to_path_buf()),
                    suggestion: Some("Check directory permissions".to_string()),
                })?;
        }
        
        fs::copy(from, to)
            .map_err(|e| FileSystemError {
                message: format!("Failed to copy file: {}", e),
                path: Some(from.to_path_buf()),
                suggestion: Some("Check file paths and permissions".to_string()),
            })?;
        
        Ok(())
    }
    
    /// Move file
    pub fn move_file(&self, from: &Path, to: &Path) -> Result<(), FileSystemError> {
        // Create parent directories if they don't exist
        if let Some(parent) = to.parent() {
            fs::create_dir_all(parent)
                .map_err(|e| FileSystemError {
                    message: format!("Failed to create parent directories: {}", e),
                    path: Some(parent.to_path_buf()),
                    suggestion: Some("Check directory permissions".to_string()),
                })?;
        }
        
        fs::rename(from, to)
            .map_err(|e| FileSystemError {
                message: format!("Failed to move file: {}", e),
                path: Some(from.to_path_buf()),
                suggestion: Some("Check file paths and permissions".to_string()),
            })
    }
    
    /// Delete file
    pub fn delete_file(&self, path: &Path) -> Result<(), FileSystemError> {
        fs::remove_file(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to delete file: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })
    }
    
    /// Create directory
    pub fn create_directory(&self, path: &Path) -> Result<(), FileSystemError> {
        fs::create_dir_all(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to create directory: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check directory path and permissions".to_string()),
            })
    }
    
    /// Delete directory
    pub fn delete_directory(&self, path: &Path) -> Result<(), FileSystemError> {
        fs::remove_dir_all(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to delete directory: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check directory path and permissions".to_string()),
            })
    }
    
    /// List directory contents
    pub fn list_directory(&self, path: &Path) -> Result<Vec<PathBuf>, FileSystemError> {
        let mut entries = Vec::new();
        
        for entry in fs::read_dir(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to read directory: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check directory path and permissions".to_string()),
            })?
        {
            let entry = entry
                .map_err(|e| FileSystemError {
                    message: format!("Failed to read directory entry: {}", e),
                    path: Some(path.to_path_buf()),
                    suggestion: None,
                })?;
            
            entries.push(entry.path());
        }
        
        Ok(entries)
    }
    
    /// Walk directory recursively
    pub fn walk_directory(&self, path: &Path) -> Result<Vec<PathBuf>, FileSystemError> {
        let mut entries = Vec::new();
        
        for entry in WalkDir::new(path) {
            let entry = entry
                .map_err(|e| FileSystemError {
                    message: format!("Failed to walk directory: {}", e),
                    path: Some(path.to_path_buf()),
                    suggestion: None,
                })?;
            
            entries.push(entry.path().to_path_buf());
        }
        
        Ok(entries)
    }
    
    /// Find files by pattern
    pub fn find_files(&self, path: &Path, pattern: &str) -> Result<Vec<PathBuf>, FileSystemError> {
        let mut files = Vec::new();
        
        for entry in WalkDir::new(path) {
            let entry = entry
                .map_err(|e| FileSystemError {
                    message: format!("Failed to walk directory: {}", e),
                    path: Some(path.to_path_buf()),
                    suggestion: None,
                })?;
            
            if entry.file_type().is_file() {
                if let Some(file_name) = entry.file_name().to_str() {
                    if file_name.contains(pattern) {
                        files.push(entry.path().to_path_buf());
                    }
                }
            }
        }
        
        Ok(files)
    }
    
    /// Get relative path
    pub fn get_relative_path(&self, path: &Path, base: &Path) -> Result<PathBuf, FileSystemError> {
        path.strip_prefix(base)
            .map(|p| p.to_path_buf())
            .map_err(|_| FileSystemError {
                message: "Path is not relative to base".to_string(),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check path relationships".to_string()),
            })
    }
    
    /// Get absolute path
    pub fn get_absolute_path(&self, path: &Path) -> Result<PathBuf, FileSystemError> {
        if path.is_absolute() {
            Ok(path.to_path_buf())
        } else {
            Ok(self.current_dir.join(path))
        }
    }
    
    /// Normalize path
    pub fn normalize_path(&self, path: &Path) -> PathBuf {
        let mut components = Vec::new();
        
        for component in path.components() {
            match component {
                std::path::Component::ParentDir => {
                    if !components.is_empty() {
                        components.pop();
                    }
                }
                std::path::Component::CurDir => {
                    // Skip current directory
                }
                _ => {
                    components.push(component);
                }
            }
        }
        
        components.iter().collect()
    }
    
    /// Get file modification time
    pub fn get_file_modification_time(&self, path: &Path) -> Result<std::time::SystemTime, FileSystemError> {
        let metadata = fs::metadata(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to get file metadata: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        metadata.modified()
            .map_err(|e| FileSystemError {
                message: format!("Failed to get file modification time: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: None,
            })
    }
    
    /// Check if file is newer than another
    pub fn is_file_newer(&self, file1: &Path, file2: &Path) -> Result<bool, FileSystemError> {
        let time1 = self.get_file_modification_time(file1)?;
        let time2 = self.get_file_modification_time(file2)?;
        
        Ok(time1 > time2)
    }
    
    /// Get file permissions
    pub fn get_file_permissions(&self, path: &Path) -> Result<fs::Permissions, FileSystemError> {
        let metadata = fs::metadata(path)
            .map_err(|e| FileSystemError {
                message: format!("Failed to get file metadata: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        Ok(metadata.permissions())
    }
    
    /// Set file permissions
    pub fn set_file_permissions(&self, path: &Path, permissions: fs::Permissions) -> Result<(), FileSystemError> {
        fs::set_permissions(path, permissions)
            .map_err(|e| FileSystemError {
                message: format!("Failed to set file permissions: {}", e),
                path: Some(path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })
    }
}

impl Default for FileSystemUtils {
    fn default() -> Self {
        Self::new()
    }
}