//! CHTL Module Parser
//! 
//! This module handles parsing of CMOD and CJMOD files.

use anyhow::Result;
use std::path::Path;
use crate::chtl::cmod_system::{CmodModule, CmodInfo};
use crate::chtl_js::cjmod_system::CjmodModule;

/// Module parser for CMOD and CJMOD files
pub struct ModuleParser;

impl ModuleParser {
    /// Parse a CMOD module from directory
    pub fn parse_cmod<P: AsRef<Path>>(path: P) -> Result<CmodModule> {
        let path = path.as_ref();
        
        // Read module info
        let info_file = path.join("info").join("module.chtl");
        let info_content = if info_file.exists() {
            std::fs::read_to_string(info_file)?
        } else {
            // Create default info if not found
            String::new()
        };
        
        // Parse module info
        let info = Self::parse_module_info(&info_content, path)?;
        
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
        let exports = Self::parse_exports(&info_content)?;
        
        Ok(CmodModule {
            info,
            exports,
            source_files,
        })
    }
    
    /// Parse a CJMOD module from file
    pub fn parse_cjmod<P: AsRef<Path>>(path: P) -> Result<CjmodModule> {
        let path = path.as_ref();
        let content = std::fs::read_to_string(path)?;
        
        // Extract module name from filename
        let name = path.file_stem()
            .and_then(|s| s.to_str())
            .unwrap_or("unknown")
            .to_string();
        
        Ok(CjmodModule {
            name,
            content,
        })
    }
    
    /// Parse module info from CHTL content
    fn parse_module_info(content: &str, path: &Path) -> Result<CmodInfo> {
        // Simple parsing - in a real implementation, this would use the CHTL parser
        let name = path.file_name()
            .and_then(|s| s.to_str())
            .unwrap_or("unknown")
            .to_string();
        
        Ok(CmodInfo {
            name,
            version: "1.0.0".to_string(),
            description: "CMOD Module".to_string(),
            author: "Unknown".to_string(),
            license: "MIT".to_string(),
            dependencies: Vec::new(),
            category: "general".to_string(),
            min_chtl_version: "1.0.0".to_string(),
            max_chtl_version: "2.0.0".to_string(),
        })
    }
    
    /// Parse exports from CHTL content
    fn parse_exports(content: &str) -> Result<Vec<String>> {
        // Simple parsing - in a real implementation, this would use the CHTL parser
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
}