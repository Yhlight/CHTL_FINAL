//! File system utilities
//! 
//! This module provides file system operations for CHTL.

use anyhow::Result;
use std::path::{Path, PathBuf};
use walkdir::WalkDir;

/// File system utilities
pub struct FileSystemUtil;

impl FileSystemUtil {
    /// Find all CHTL files in a directory
    pub fn find_chtl_files<P: AsRef<Path>>(dir: P) -> Result<Vec<PathBuf>> {
        let mut files = Vec::new();
        
        for entry in WalkDir::new(dir) {
            let entry = entry?;
            if entry.file_type().is_file() {
                if let Some(ext) = entry.path().extension() {
                    if ext == "chtl" {
                        files.push(entry.path().to_path_buf());
                    }
                }
            }
        }
        
        Ok(files)
    }
    
    /// Find all CMOD files in a directory
    pub fn find_cmod_files<P: AsRef<Path>>(dir: P) -> Result<Vec<PathBuf>> {
        let mut files = Vec::new();
        
        for entry in WalkDir::new(dir) {
            let entry = entry?;
            if entry.file_type().is_file() {
                if let Some(ext) = entry.path().extension() {
                    if ext == "cmod" {
                        files.push(entry.path().to_path_buf());
                    }
                }
            }
        }
        
        Ok(files)
    }
    
    /// Create directory if it doesn't exist
    pub fn ensure_dir<P: AsRef<Path>>(path: P) -> Result<()> {
        if !path.as_ref().exists() {
            std::fs::create_dir_all(path)?;
        }
        Ok(())
    }
    
    /// Copy file
    pub fn copy_file<P: AsRef<Path>, Q: AsRef<Path>>(from: P, to: Q) -> Result<()> {
        std::fs::copy(from, to)?;
        Ok(())
    }
    
    /// Read file to string
    pub fn read_file<P: AsRef<Path>>(path: P) -> Result<String> {
        Ok(std::fs::read_to_string(path)?)
    }
    
    /// Write string to file
    pub fn write_file<P: AsRef<Path>>(path: P, content: &str) -> Result<()> {
        std::fs::write(path, content)?;
        Ok(())
    }
}