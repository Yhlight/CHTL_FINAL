//! Zip utilities
//! 
//! This module provides zip utility functionality for CHTL,
//! including creating, extracting, and managing zip archives.

use std::path::{Path, PathBuf};
use std::fs::File;
use std::io::{Read, Write};
use zip::ZipWriter;
use zip::write::FileOptions;

/// Zip utilities
#[derive(Debug, Clone)]
pub struct ZipUtils {
    /// Compression level
    compression_level: u32,
    /// File options
    file_options: FileOptions,
}

/// Zip utility error
#[derive(Debug, Clone)]
pub struct ZipUtilsError {
    pub message: String,
    pub path: Option<PathBuf>,
    pub suggestion: Option<String>,
}

impl ZipUtils {
    /// Create a new zip utils instance
    pub fn new() -> Self {
        Self {
            compression_level: 6,
            file_options: FileOptions::default().compression_method(zip::CompressionMethod::Deflated),
        }
    }
    
    /// Create a new zip utils instance with compression level
    pub fn with_compression_level(compression_level: u32) -> Self {
        Self {
            compression_level,
            file_options: FileOptions::default().compression_method(zip::CompressionMethod::Deflated),
        }
    }
    
    /// Create a zip archive
    pub fn create_archive(&self, archive_path: &Path, files: &[PathBuf]) -> Result<(), ZipUtilsError> {
        let file = File::create(archive_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to create archive file: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        let mut zip = ZipWriter::new(file);
        
        for file_path in files {
            self.add_file_to_archive(&mut zip, file_path)?;
        }
        
        zip.finish()
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to finish archive: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: None,
            })?;
        
        Ok(())
    }
    
    /// Add file to archive
    fn add_file_to_archive(&self, zip: &mut ZipWriter<File>, file_path: &Path) -> Result<(), ZipUtilsError> {
        let file_name = file_path.file_name()
            .and_then(|n| n.to_str())
            .ok_or_else(|| ZipUtilsError {
                message: "Invalid file name".to_string(),
                path: Some(file_path.to_path_buf()),
                suggestion: None,
            })?;
        
        zip.start_file(file_name, self.file_options)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to start file in archive: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: None,
            })?;
        
        let mut file = File::open(file_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to open file: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        let mut buffer = Vec::new();
        file.read_to_end(&mut buffer)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to read file: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: None,
            })?;
        
        zip.write_all(&buffer)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to write file to archive: {}", e),
                path: Some(file_path.to_path_buf()),
                suggestion: None,
            })?;
        
        Ok(())
    }
    
    /// Extract zip archive
    pub fn extract_archive(&self, archive_path: &Path, extract_to: &Path) -> Result<(), ZipUtilsError> {
        let file = File::open(archive_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to open archive file: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        let mut zip = zip::ZipArchive::new(file)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to read archive: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check archive format".to_string()),
            })?;
        
        for i in 0..zip.len() {
            let mut file = zip.by_index(i)
                .map_err(|e| ZipUtilsError {
                    message: format!("Failed to read file from archive: {}", e),
                    path: Some(archive_path.to_path_buf()),
                    suggestion: None,
                })?;
            
            let file_path = extract_to.join(file.name());
            
            // Create parent directories if they don't exist
            if let Some(parent) = file_path.parent() {
                std::fs::create_dir_all(parent)
                    .map_err(|e| ZipUtilsError {
                        message: format!("Failed to create directory: {}", e),
                        path: Some(parent.to_path_buf()),
                        suggestion: Some("Check directory permissions".to_string()),
                    })?;
            }
            
            // Extract file
            let mut extracted_file = File::create(&file_path)
                .map_err(|e| ZipUtilsError {
                    message: format!("Failed to create extracted file: {}", e),
                    path: Some(file_path),
                    suggestion: Some("Check file path and permissions".to_string()),
                })?;
            
            std::io::copy(&mut file, &mut extracted_file)
                .map_err(|e| ZipUtilsError {
                    message: format!("Failed to extract file: {}", e),
                    path: Some(file_path),
                    suggestion: None,
                })?;
        }
        
        Ok(())
    }
    
    /// List archive contents
    pub fn list_archive_contents(&self, archive_path: &Path) -> Result<Vec<String>, ZipUtilsError> {
        let file = File::open(archive_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to open archive file: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        let zip = zip::ZipArchive::new(file)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to read archive: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check archive format".to_string()),
            })?;
        
        let mut contents = Vec::new();
        
        for i in 0..zip.len() {
            let file = zip.by_index(i)
                .map_err(|e| ZipUtilsError {
                    message: format!("Failed to read file from archive: {}", e),
                    path: Some(archive_path.to_path_buf()),
                    suggestion: None,
                })?;
            
            contents.push(file.name().to_string());
        }
        
        Ok(contents)
    }
    
    /// Check if file is a valid zip archive
    pub fn is_valid_archive(&self, archive_path: &Path) -> bool {
        if let Ok(file) = File::open(archive_path) {
            zip::ZipArchive::new(file).is_ok()
        } else {
            false
        }
    }
    
    /// Get archive size
    pub fn get_archive_size(&self, archive_path: &Path) -> Result<u64, ZipUtilsError> {
        let metadata = std::fs::metadata(archive_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to get archive metadata: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        Ok(metadata.len())
    }
    
    /// Get file count in archive
    pub fn get_file_count(&self, archive_path: &Path) -> Result<usize, ZipUtilsError> {
        let file = File::open(archive_path)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to open archive file: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        let zip = zip::ZipArchive::new(file)
            .map_err(|e| ZipUtilsError {
                message: format!("Failed to read archive: {}", e),
                path: Some(archive_path.to_path_buf()),
                suggestion: Some("Check archive format".to_string()),
            })?;
        
        Ok(zip.len())
    }
    
    /// Get compression level
    pub fn get_compression_level(&self) -> u32 {
        self.compression_level
    }
    
    /// Set compression level
    pub fn set_compression_level(&mut self, level: u32) {
        self.compression_level = level;
    }
    
    /// Get file options
    pub fn get_file_options(&self) -> &FileOptions {
        &self.file_options
    }
    
    /// Set file options
    pub fn set_file_options(&mut self, options: FileOptions) {
        self.file_options = options;
    }
}

impl Default for ZipUtils {
    fn default() -> Self {
        Self::new()
    }
}