//! Code merger
//! 
//! This module provides the main code merging functionality for CHTL,
//! including merging different types of code and managing code integration.

use std::collections::HashMap;
use crate::chtl::context::*;
use crate::scanner::*;

/// Code merger
#[derive(Debug, Clone)]
pub struct CodeMerger {
    /// Merged code
    merged_code: HashMap<CodeType, String>,
    /// Code fragments
    fragments: Vec<CodeFragment>,
    /// Merger options
    options: MergerOptions,
}

/// Code type
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum CodeType {
    HTML,
    CSS,
    JavaScript,
    CHTL,
    CHTLJS,
}

/// Code fragment
#[derive(Debug, Clone)]
pub struct CodeFragment {
    /// Fragment type
    pub fragment_type: FragmentType,
    /// Fragment content
    pub content: String,
    /// Fragment start position
    pub start_position: usize,
    /// Fragment end position
    pub end_position: usize,
    /// Fragment line
    pub line: usize,
    /// Fragment column
    pub column: usize,
    /// Fragment metadata
    pub metadata: HashMap<String, String>,
}

/// Fragment type
#[derive(Debug, Clone, PartialEq)]
pub enum FragmentType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Comment,
    Whitespace,
    Unknown,
}

/// Merger options
#[derive(Debug, Clone)]
pub struct MergerOptions {
    /// Minify output
    pub minify: bool,
    /// Include source maps
    pub source_maps: bool,
    /// Preserve comments
    pub preserve_comments: bool,
    /// Merge order
    pub merge_order: Vec<CodeType>,
    /// Custom merge functions
    pub custom_merge_functions: HashMap<CodeType, Box<dyn Fn(&str, &str) -> String>>,
}

/// Merger error
#[derive(Debug, Clone)]
pub struct MergerError {
    pub message: String,
    pub fragment_type: FragmentType,
    pub position: usize,
    pub suggestion: Option<String>,
}

impl CodeMerger {
    /// Create a new code merger
    pub fn new() -> Self {
        Self {
            merged_code: HashMap::new(),
            fragments: Vec::new(),
            options: MergerOptions::default(),
        }
    }
    
    /// Create a new code merger with options
    pub fn with_options(options: MergerOptions) -> Self {
        Self {
            merged_code: HashMap::new(),
            fragments: Vec::new(),
            options,
        }
    }
    
    /// Merge code fragments
    pub fn merge(&mut self, fragments: Vec<CodeFragment>) -> Result<HashMap<CodeType, String>, MergerError> {
        self.fragments = fragments;
        self.merged_code.clear();
        
        // Group fragments by type
        let mut grouped_fragments: HashMap<CodeType, Vec<&CodeFragment>> = HashMap::new();
        
        for fragment in &self.fragments {
            let code_type = self.fragment_type_to_code_type(&fragment.fragment_type);
            grouped_fragments.entry(code_type).or_insert_with(Vec::new).push(fragment);
        }
        
        // Merge each type
        for code_type in &self.options.merge_order {
            if let Some(fragments) = grouped_fragments.get(code_type) {
                let merged = self.merge_fragments_of_type(fragments, code_type)?;
                self.merged_code.insert(code_type.clone(), merged);
            }
        }
        
        // Merge remaining types
        for (code_type, fragments) in &grouped_fragments {
            if !self.merged_code.contains_key(code_type) {
                let merged = self.merge_fragments_of_type(fragments, code_type)?;
                self.merged_code.insert(code_type.clone(), merged);
            }
        }
        
        Ok(self.merged_code.clone())
    }
    
    /// Merge fragments of a specific type
    fn merge_fragments_of_type(&self, fragments: &[&CodeFragment], code_type: &CodeType) -> Result<String, MergerError> {
        let mut merged = String::new();
        
        for fragment in fragments {
            let content = &fragment.content;
            
            // Apply custom merge function if available
            if let Some(merge_function) = self.options.custom_merge_functions.get(code_type) {
                merged = merge_function(&merged, content);
            } else {
                // Default merge behavior
                merged = self.default_merge(&merged, content, code_type)?;
            }
        }
        
        // Apply post-processing
        merged = self.post_process(&merged, code_type)?;
        
        Ok(merged)
    }
    
    /// Default merge behavior
    fn default_merge(&self, existing: &str, new: &str, code_type: &CodeType) -> Result<String, MergerError> {
        match code_type {
            CodeType::HTML => {
                Ok(format!("{}{}", existing, new))
            }
            CodeType::CSS => {
                if existing.is_empty() {
                    Ok(new.to_string())
                } else {
                    Ok(format!("{}\n{}", existing, new))
                }
            }
            CodeType::JavaScript => {
                if existing.is_empty() {
                    Ok(new.to_string())
                } else {
                    Ok(format!("{}\n{}", existing, new))
                }
            }
            CodeType::CHTL => {
                Ok(format!("{}{}", existing, new))
            }
            CodeType::CHTLJS => {
                if existing.is_empty() {
                    Ok(new.to_string())
                } else {
                    Ok(format!("{}\n{}", existing, new))
                }
            }
        }
    }
    
    /// Post-process merged code
    fn post_process(&self, code: &str, code_type: &CodeType) -> Result<String, MergerError> {
        let mut processed = code.to_string();
        
        // Apply minification if enabled
        if self.options.minify {
            processed = self.minify_code(&processed, code_type)?;
        }
        
        // Remove comments if not preserving
        if !self.options.preserve_comments {
            processed = self.remove_comments(&processed, code_type)?;
        }
        
        // Add source maps if enabled
        if self.options.source_maps {
            processed = self.add_source_maps(&processed, code_type)?;
        }
        
        Ok(processed)
    }
    
    /// Minify code
    fn minify_code(&self, code: &str, code_type: &CodeType) -> Result<String, MergerError> {
        match code_type {
            CodeType::HTML => {
                // Basic HTML minification
                let mut minified = code.to_string();
                minified = minified.replace("  ", " ");
                minified = minified.replace("\n", "");
                minified = minified.replace("\t", "");
                Ok(minified)
            }
            CodeType::CSS => {
                // Basic CSS minification
                let mut minified = code.to_string();
                minified = minified.replace("  ", " ");
                minified = minified.replace("\n", "");
                minified = minified.replace("\t", "");
                minified = minified.replace("; ", ";");
                minified = minified.replace(" {", "{");
                minified = minified.replace("{ ", "{");
                minified = minified.replace(" }", "}");
                Ok(minified)
            }
            CodeType::JavaScript => {
                // Basic JavaScript minification
                let mut minified = code.to_string();
                minified = minified.replace("  ", " ");
                minified = minified.replace("\n", "");
                minified = minified.replace("\t", "");
                minified = minified.replace("; ", ";");
                minified = minified.replace(" {", "{");
                minified = minified.replace("{ ", "{");
                minified = minified.replace(" }", "}");
                Ok(minified)
            }
            _ => Ok(code.to_string()),
        }
    }
    
    /// Remove comments
    fn remove_comments(&self, code: &str, code_type: &CodeType) -> Result<String, MergerError> {
        match code_type {
            CodeType::HTML => {
                // Remove HTML comments
                let mut processed = code.to_string();
                processed = regex::Regex::new(r"<!--.*?-->")
                    .unwrap()
                    .replace_all(&processed, "")
                    .to_string();
                Ok(processed)
            }
            CodeType::CSS => {
                // Remove CSS comments
                let mut processed = code.to_string();
                processed = regex::Regex::new(r"/\*.*?\*/")
                    .unwrap()
                    .replace_all(&processed, "")
                    .to_string();
                Ok(processed)
            }
            CodeType::JavaScript => {
                // Remove JavaScript comments
                let mut processed = code.to_string();
                processed = regex::Regex::new(r"//.*")
                    .unwrap()
                    .replace_all(&processed, "")
                    .to_string();
                processed = regex::Regex::new(r"/\*.*?\*/")
                    .unwrap()
                    .replace_all(&processed, "")
                    .to_string();
                Ok(processed)
            }
            _ => Ok(code.to_string()),
        }
    }
    
    /// Add source maps
    fn add_source_maps(&self, code: &str, code_type: &CodeType) -> Result<String, MergerError> {
        // TODO: Implement source map generation
        Ok(code.to_string())
    }
    
    /// Convert fragment type to code type
    fn fragment_type_to_code_type(&self, fragment_type: &FragmentType) -> CodeType {
        match fragment_type {
            FragmentType::CHTL => CodeType::CHTL,
            FragmentType::CHTLJS => CodeType::CHTLJS,
            FragmentType::CSS => CodeType::CSS,
            FragmentType::JavaScript => CodeType::JavaScript,
            FragmentType::HTML => CodeType::HTML,
            _ => CodeType::HTML, // Default fallback
        }
    }
    
    /// Get merged code
    pub fn get_merged_code(&self) -> &HashMap<CodeType, String> {
        &self.merged_code
    }
    
    /// Get merged code by type
    pub fn get_merged_code_by_type(&self, code_type: &CodeType) -> Option<&String> {
        self.merged_code.get(code_type)
    }
    
    /// Get fragments
    pub fn get_fragments(&self) -> &[CodeFragment] {
        &self.fragments
    }
    
    /// Get options
    pub fn get_options(&self) -> &MergerOptions {
        &self.options
    }
    
    /// Set options
    pub fn set_options(&mut self, options: MergerOptions) {
        self.options = options;
    }
    
    /// Clear merged code
    pub fn clear(&mut self) {
        self.merged_code.clear();
        self.fragments.clear();
    }
    
    /// Get merge statistics
    pub fn get_statistics(&self) -> MergerStatistics {
        let mut type_counts = HashMap::new();
        
        for fragment in &self.fragments {
            let code_type = self.fragment_type_to_code_type(&fragment.fragment_type);
            let count = type_counts.get(&code_type).unwrap_or(&0) + 1;
            type_counts.insert(code_type, count);
        }
        
        MergerStatistics {
            total_fragments: self.fragments.len(),
            merged_types: self.merged_code.len(),
            type_counts,
        }
    }
}

/// Merger statistics
#[derive(Debug, Clone)]
pub struct MergerStatistics {
    pub total_fragments: usize,
    pub merged_types: usize,
    pub type_counts: HashMap<CodeType, usize>,
}

impl Default for CodeMerger {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for MergerOptions {
    fn default() -> Self {
        Self {
            minify: false,
            source_maps: false,
            preserve_comments: true,
            merge_order: vec![
                CodeType::HTML,
                CodeType::CSS,
                CodeType::JavaScript,
                CodeType::CHTL,
                CodeType::CHTLJS,
            ],
            custom_merge_functions: HashMap::new(),
        }
    }
}