//! Boundary detector
//! 
//! This module provides boundary detection functionality for CHTL,
//! including detecting different types of code boundaries and managing boundary information.

use std::collections::HashMap;
use regex::Regex;
use lazy_static::lazy_static;

/// Boundary detector
#[derive(Debug, Clone)]
pub struct BoundaryDetector {
    /// Detected boundaries
    boundaries: Vec<Boundary>,
    /// Boundary patterns
    patterns: Vec<BoundaryPattern>,
    /// Boundary metadata
    metadata: HashMap<String, String>,
}

/// Boundary
#[derive(Debug, Clone)]
pub struct Boundary {
    /// Boundary type
    pub boundary_type: BoundaryType,
    /// Start position
    pub start_position: usize,
    /// End position
    pub end_position: usize,
    /// Start line
    pub start_line: usize,
    /// End line
    pub end_line: usize,
    /// Start column
    pub start_column: usize,
    /// End column
    pub end_column: usize,
    /// Boundary content
    pub content: String,
    /// Boundary metadata
    pub metadata: HashMap<String, String>,
}

/// Boundary type
#[derive(Debug, Clone, PartialEq)]
pub enum BoundaryType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Comment,
    String,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
    Whitespace,
    Unknown,
}

/// Boundary pattern
#[derive(Debug, Clone)]
pub struct BoundaryPattern {
    /// Pattern name
    pub name: String,
    /// Start pattern
    pub start_pattern: Regex,
    /// End pattern
    pub end_pattern: Option<Regex>,
    /// Boundary type
    pub boundary_type: BoundaryType,
    /// Pattern metadata
    pub metadata: HashMap<String, String>,
}

/// Boundary error
#[derive(Debug, Clone)]
pub struct BoundaryError {
    pub message: String,
    pub position: usize,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

impl BoundaryDetector {
    /// Create a new boundary detector
    pub fn new() -> Self {
        let mut detector = Self {
            boundaries: Vec::new(),
            patterns: Vec::new(),
            metadata: HashMap::new(),
        };
        
        // Initialize default patterns
        detector.initialize_patterns();
        
        detector
    }
    
    /// Initialize default patterns
    fn initialize_patterns(&mut self) {
        // CHTL patterns
        self.add_pattern(BoundaryPattern {
            name: "chtl_template".to_string(),
            start_pattern: Regex::new(r"\[Template\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_custom".to_string(),
            start_pattern: Regex::new(r"\[Custom\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_origin".to_string(),
            start_pattern: Regex::new(r"\[Origin\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_import".to_string(),
            start_pattern: Regex::new(r"\[Import\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_namespace".to_string(),
            start_pattern: Regex::new(r"\[Namespace\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_configuration".to_string(),
            start_pattern: Regex::new(r"\[Configuration\]").unwrap(),
            end_pattern: Some(Regex::new(r"\]").unwrap()),
            boundary_type: BoundaryType::CHTL,
            metadata: HashMap::new(),
        });
        
        // CHTL JS patterns
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_selector".to_string(),
            start_pattern: Regex::new(r"\{\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_virtual".to_string(),
            start_pattern: Regex::new(r"vir\s+\w+\s*=").unwrap(),
            end_pattern: None,
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_listen".to_string(),
            start_pattern: Regex::new(r"listen\s*\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_delegate".to_string(),
            start_pattern: Regex::new(r"delegate\s*\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_animate".to_string(),
            start_pattern: Regex::new(r"animate\s*\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_router".to_string(),
            start_pattern: Regex::new(r"router\s*\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "chtl_js_fileloader".to_string(),
            start_pattern: Regex::new(r"fileloader\s*\{").unwrap(),
            end_pattern: Some(Regex::new(r"\}").unwrap()),
            boundary_type: BoundaryType::CHTLJS,
            metadata: HashMap::new(),
        });
        
        // CSS patterns
        self.add_pattern(BoundaryPattern {
            name: "css_style".to_string(),
            start_pattern: Regex::new(r"<style[^>]*>").unwrap(),
            end_pattern: Some(Regex::new(r"</style>").unwrap()),
            boundary_type: BoundaryType::CSS,
            metadata: HashMap::new(),
        });
        
        // JavaScript patterns
        self.add_pattern(BoundaryPattern {
            name: "javascript_script".to_string(),
            start_pattern: Regex::new(r"<script[^>]*>").unwrap(),
            end_pattern: Some(Regex::new(r"</script>").unwrap()),
            boundary_type: BoundaryType::JavaScript,
            metadata: HashMap::new(),
        });
        
        // HTML patterns
        self.add_pattern(BoundaryPattern {
            name: "html_tag".to_string(),
            start_pattern: Regex::new(r"<[^>]+>").unwrap(),
            end_pattern: None,
            boundary_type: BoundaryType::HTML,
            metadata: HashMap::new(),
        });
        
        // Comment patterns
        self.add_pattern(BoundaryPattern {
            name: "comment_single".to_string(),
            start_pattern: Regex::new(r"//").unwrap(),
            end_pattern: None,
            boundary_type: BoundaryType::Comment,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "comment_multi".to_string(),
            start_pattern: Regex::new(r"/\*").unwrap(),
            end_pattern: Some(Regex::new(r"\*/").unwrap()),
            boundary_type: BoundaryType::Comment,
            metadata: HashMap::new(),
        });
        
        // String patterns
        self.add_pattern(BoundaryPattern {
            name: "string_double".to_string(),
            start_pattern: Regex::new(r#"""#).unwrap(),
            end_pattern: Some(Regex::new(r#"""#).unwrap()),
            boundary_type: BoundaryType::String,
            metadata: HashMap::new(),
        });
        
        self.add_pattern(BoundaryPattern {
            name: "string_single".to_string(),
            start_pattern: Regex::new(r"'").unwrap(),
            end_pattern: Some(Regex::new(r"'").unwrap()),
            boundary_type: BoundaryType::String,
            metadata: HashMap::new(),
        });
    }
    
    /// Add a pattern
    pub fn add_pattern(&mut self, pattern: BoundaryPattern) {
        self.patterns.push(pattern);
    }
    
    /// Detect boundaries in source code
    pub fn detect_boundaries(&mut self, source: &str) -> Result<(), BoundaryError> {
        self.boundaries.clear();
        
        let mut position = 0;
        let mut line = 1;
        let mut column = 1;
        
        while position < source.len() {
            let current_char = source.chars().nth(position).unwrap();
            
            // Check each pattern
            for pattern in &self.patterns {
                if let Some(mat) = pattern.start_pattern.find(&source[position..]) {
                    let start_pos = position + mat.start();
                    let start_line = line;
                    let start_column = column;
                    
                    // Find end position
                    let end_pos = if let Some(end_pattern) = &pattern.end_pattern {
                        if let Some(end_mat) = end_pattern.find(&source[start_pos..]) {
                            start_pos + end_mat.end()
                        } else {
                            // No end pattern found, treat as single character
                            start_pos + 1
                        }
                    } else {
                        // No end pattern, treat as single character
                        start_pos + 1
                    };
                    
                    // Calculate end line and column
                    let mut end_line = start_line;
                    let mut end_column = start_column;
                    let mut pos = start_pos;
                    
                    while pos < end_pos && pos < source.len() {
                        let ch = source.chars().nth(pos).unwrap();
                        if ch == '\n' {
                            end_line += 1;
                            end_column = 1;
                        } else {
                            end_column += 1;
                        }
                        pos += 1;
                    }
                    
                    // Create boundary
                    let boundary = Boundary {
                        boundary_type: pattern.boundary_type.clone(),
                        start_position: start_pos,
                        end_position: end_pos,
                        start_line,
                        end_line,
                        start_column,
                        end_column,
                        content: source[start_pos..end_pos].to_string(),
                        metadata: pattern.metadata.clone(),
                    };
                    
                    self.boundaries.push(boundary);
                    
                    // Update position
                    position = end_pos;
                    line = end_line;
                    column = end_column;
                    
                    break;
                }
            }
            
            // If no pattern matched, advance position
            if position < source.len() {
                let current_char = source.chars().nth(position).unwrap();
                if current_char == '\n' {
                    line += 1;
                    column = 1;
                } else {
                    column += 1;
                }
                position += 1;
            }
        }
        
        // Sort boundaries by start position
        self.boundaries.sort_by_key(|b| b.start_position);
        
        Ok(())
    }
    
    /// Get all boundaries
    pub fn get_all_boundaries(&self) -> &[Boundary] {
        &self.boundaries
    }
    
    /// Get boundaries by type
    pub fn get_boundaries_by_type(&self, boundary_type: BoundaryType) -> Vec<&Boundary> {
        self.boundaries
            .iter()
            .filter(|b| b.boundary_type == boundary_type)
            .collect()
    }
    
    /// Get boundaries in range
    pub fn get_boundaries_in_range(&self, start: usize, end: usize) -> Vec<&Boundary> {
        self.boundaries
            .iter()
            .filter(|b| b.start_position >= start && b.end_position <= end)
            .collect()
    }
    
    /// Get boundaries at position
    pub fn get_boundaries_at_position(&self, position: usize) -> Vec<&Boundary> {
        self.boundaries
            .iter()
            .filter(|b| b.start_position <= position && b.end_position > position)
            .collect()
    }
    
    /// Get patterns
    pub fn get_patterns(&self) -> &[BoundaryPattern] {
        &self.patterns
    }
    
    /// Clear boundaries
    pub fn clear_boundaries(&mut self) {
        self.boundaries.clear();
    }
    
    /// Clear patterns
    pub fn clear_patterns(&mut self) {
        self.patterns.clear();
    }
    
    /// Get boundary count
    pub fn get_boundary_count(&self) -> usize {
        self.boundaries.len()
    }
    
    /// Get pattern count
    pub fn get_pattern_count(&self) -> usize {
        self.patterns.len()
    }
    
    /// Get boundary statistics
    pub fn get_statistics(&self) -> BoundaryStatistics {
        let mut type_counts = HashMap::new();
        
        for boundary in &self.boundaries {
            let count = type_counts.get(&boundary.boundary_type).unwrap_or(&0) + 1;
            type_counts.insert(boundary.boundary_type.clone(), count);
        }
        
        BoundaryStatistics {
            total_boundaries: self.boundaries.len(),
            total_patterns: self.patterns.len(),
            type_counts,
        }
    }
    
    /// Validate boundary
    pub fn validate_boundary(&self, boundary: &Boundary) -> bool {
        boundary.start_position < boundary.end_position &&
        boundary.start_line <= boundary.end_line &&
        (boundary.start_line < boundary.end_line || boundary.start_column <= boundary.end_column)
    }
    
    /// Merge overlapping boundaries
    pub fn merge_overlapping_boundaries(&mut self) {
        if self.boundaries.is_empty() {
            return;
        }
        
        let mut merged = Vec::new();
        let mut current = self.boundaries[0].clone();
        
        for boundary in &self.boundaries[1..] {
            if boundary.start_position <= current.end_position {
                // Overlapping boundaries, merge them
                current.end_position = current.end_position.max(boundary.end_position);
                current.end_line = current.end_line.max(boundary.end_line);
                current.end_column = current.end_column.max(boundary.end_column);
                current.content = self.boundaries[0].content.clone(); // TODO: Fix this
            } else {
                // Non-overlapping boundaries, add current and start new
                merged.push(current);
                current = boundary.clone();
            }
        }
        
        merged.push(current);
        self.boundaries = merged;
    }
}

/// Boundary statistics
#[derive(Debug, Clone)]
pub struct BoundaryStatistics {
    pub total_boundaries: usize,
    pub total_patterns: usize,
    pub type_counts: HashMap<BoundaryType, usize>,
}

impl Default for BoundaryDetector {
    fn default() -> Self {
        Self::new()
    }
}