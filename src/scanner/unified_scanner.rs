//! CHTL unified scanner
//! 
//! This module provides the unified scanning functionality for CHTL,
//! which is responsible for separating different types of code and managing placeholders.

use std::collections::HashMap;
use crate::chtl::context::*;

/// CHTL unified scanner
#[derive(Debug, Clone)]
pub struct CHTLUnifiedScanner {
    /// Source code to scan
    source: String,
    /// Current position
    position: usize,
    /// Current line
    line: usize,
    /// Current column
    column: usize,
    /// Code fragments
    fragments: Vec<CodeFragment>,
    /// Placeholder manager
    placeholder_manager: PlaceholderManager,
    /// Boundary detector
    boundary_detector: BoundaryDetector,
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

/// Placeholder manager
#[derive(Debug, Clone)]
pub struct PlaceholderManager {
    /// Placeholder counter
    counter: usize,
    /// Placeholder mappings
    mappings: HashMap<String, String>,
    /// Reverse mappings
    reverse_mappings: HashMap<String, String>,
}

/// Boundary detector
#[derive(Debug, Clone)]
pub struct BoundaryDetector {
    /// CHTL boundaries
    chtl_boundaries: Vec<Boundary>,
    /// CHTL JS boundaries
    chtl_js_boundaries: Vec<Boundary>,
    /// CSS boundaries
    css_boundaries: Vec<Boundary>,
    /// JavaScript boundaries
    javascript_boundaries: Vec<Boundary>,
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
}

/// Scanner error
#[derive(Debug, Clone)]
pub struct ScannerError {
    pub message: String,
    pub position: usize,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

impl CHTLUnifiedScanner {
    /// Create a new unified scanner
    pub fn new(source: String) -> Self {
        Self {
            source,
            position: 0,
            line: 1,
            column: 1,
            fragments: Vec::new(),
            placeholder_manager: PlaceholderManager::new(),
            boundary_detector: BoundaryDetector::new(),
        }
    }
    
    /// Scan the source code
    pub fn scan(&mut self) -> Result<Vec<CodeFragment>, ScannerError> {
        self.fragments.clear();
        self.position = 0;
        self.line = 1;
        self.column = 1;
        
        // Detect boundaries first
        self.detect_boundaries()?;
        
        // Process code based on boundaries
        self.process_code()?;
        
        Ok(self.fragments.clone())
    }
    
    /// Detect code boundaries
    fn detect_boundaries(&mut self) -> Result<(), ScannerError> {
        self.boundary_detector.detect_boundaries(&self.source)?;
        Ok(())
    }
    
    /// Process code based on boundaries
    fn process_code(&mut self) -> Result<(), ScannerError> {
        let boundaries = self.boundary_detector.get_all_boundaries();
        
        for boundary in boundaries {
            let content = &self.source[boundary.start_position..boundary.end_position];
            
            let fragment = CodeFragment {
                fragment_type: self.boundary_type_to_fragment_type(&boundary.boundary_type),
                content: content.to_string(),
                start_position: boundary.start_position,
                end_position: boundary.end_position,
                line: boundary.start_line,
                column: boundary.start_column,
            };
            
            self.fragments.push(fragment);
        }
        
        Ok(())
    }
    
    /// Convert boundary type to fragment type
    fn boundary_type_to_fragment_type(&self, boundary_type: &BoundaryType) -> FragmentType {
        match boundary_type {
            BoundaryType::CHTL => FragmentType::CHTL,
            BoundaryType::CHTLJS => FragmentType::CHTLJS,
            BoundaryType::CSS => FragmentType::CSS,
            BoundaryType::JavaScript => FragmentType::JavaScript,
            BoundaryType::HTML => FragmentType::HTML,
            BoundaryType::Comment => FragmentType::Comment,
            _ => FragmentType::Unknown,
        }
    }
    
    /// Get code fragments
    pub fn get_fragments(&self) -> &[CodeFragment] {
        &self.fragments
    }
    
    /// Get fragments by type
    pub fn get_fragments_by_type(&self, fragment_type: FragmentType) -> Vec<&CodeFragment> {
        self.fragments
            .iter()
            .filter(|f| f.fragment_type == fragment_type)
            .collect()
    }
    
    /// Get placeholder manager
    pub fn get_placeholder_manager(&self) -> &PlaceholderManager {
        &self.placeholder_manager
    }
    
    /// Get mutable placeholder manager
    pub fn get_placeholder_manager_mut(&mut self) -> &mut PlaceholderManager {
        &mut self.placeholder_manager
    }
    
    /// Get boundary detector
    pub fn get_boundary_detector(&self) -> &BoundaryDetector {
        &self.boundary_detector
    }
}

impl PlaceholderManager {
    /// Create a new placeholder manager
    pub fn new() -> Self {
        Self {
            counter: 0,
            mappings: HashMap::new(),
            reverse_mappings: HashMap::new(),
        }
    }
    
    /// Create a placeholder
    pub fn create_placeholder(&mut self, content: &str) -> String {
        let placeholder = format!("_PLACEHOLDER_{}_", self.counter);
        self.counter += 1;
        
        self.mappings.insert(placeholder.clone(), content.to_string());
        self.reverse_mappings.insert(content.to_string(), placeholder.clone());
        
        placeholder
    }
    
    /// Resolve a placeholder
    pub fn resolve_placeholder(&self, placeholder: &str) -> Option<&String> {
        self.mappings.get(placeholder)
    }
    
    /// Get placeholder for content
    pub fn get_placeholder(&self, content: &str) -> Option<&String> {
        self.reverse_mappings.get(content)
    }
    
    /// Replace placeholders in content
    pub fn replace_placeholders(&self, content: &str) -> String {
        let mut result = content.to_string();
        
        for (placeholder, original) in &self.mappings {
            result = result.replace(placeholder, original);
        }
        
        result
    }
    
    /// Get all mappings
    pub fn get_mappings(&self) -> &HashMap<String, String> {
        &self.mappings
    }
    
    /// Clear all placeholders
    pub fn clear(&mut self) {
        self.counter = 0;
        self.mappings.clear();
        self.reverse_mappings.clear();
    }
}

impl BoundaryDetector {
    /// Create a new boundary detector
    pub fn new() -> Self {
        Self {
            chtl_boundaries: Vec::new(),
            chtl_js_boundaries: Vec::new(),
            css_boundaries: Vec::new(),
            javascript_boundaries: Vec::new(),
        }
    }
    
    /// Detect boundaries in source code
    pub fn detect_boundaries(&mut self, source: &str) -> Result<(), ScannerError> {
        self.chtl_boundaries.clear();
        self.chtl_js_boundaries.clear();
        self.css_boundaries.clear();
        self.javascript_boundaries.clear();
        
        let mut position = 0;
        let mut line = 1;
        let mut column = 1;
        
        while position < source.len() {
            let current_char = source.chars().nth(position).unwrap();
            
            match current_char {
                // CHTL boundaries
                '[' => {
                    if let Some(boundary) = self.detect_chtl_boundary(source, position, line, column)? {
                        self.chtl_boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // CHTL JS boundaries
                '{' => {
                    if let Some(boundary) = self.detect_chtl_js_boundary(source, position, line, column)? {
                        self.chtl_js_boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // CSS boundaries
                '<' if source[position..].starts_with("<style") => {
                    if let Some(boundary) = self.detect_css_boundary(source, position, line, column)? {
                        self.css_boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // JavaScript boundaries
                '<' if source[position..].starts_with("<script") => {
                    if let Some(boundary) = self.detect_javascript_boundary(source, position, line, column)? {
                        self.javascript_boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // Newline
                '\n' => {
                    line += 1;
                    column = 1;
                    position += 1;
                }
                // Other characters
                _ => {
                    position += 1;
                    column += 1;
                }
            }
        }
        
        Ok(())
    }
    
    /// Detect CHTL boundary
    fn detect_chtl_boundary(&self, source: &str, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<Boundary>, ScannerError> {
        // Look for CHTL-specific patterns
        if source[start_pos..].starts_with("[Template]") ||
           source[start_pos..].starts_with("[Custom]") ||
           source[start_pos..].starts_with("[Origin]") ||
           source[start_pos..].starts_with("[Import]") ||
           source[start_pos..].starts_with("[Namespace]") ||
           source[start_pos..].starts_with("[Configuration]") {
            
            // Find matching closing bracket
            let mut depth = 0;
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < source.len() {
                let current_char = source.chars().nth(position).unwrap();
                
                match current_char {
                    '[' => {
                        depth += 1;
                        position += 1;
                        column += 1;
                    }
                    ']' => {
                        depth -= 1;
                        if depth == 0 {
                            return Ok(Some(Boundary {
                                boundary_type: BoundaryType::CHTL,
                                start_position: start_pos,
                                end_position: position + 1,
                                start_line,
                                end_line: line,
                                start_column,
                                end_column: column,
                            }));
                        }
                        position += 1;
                        column += 1;
                    }
                    '\n' => {
                        line += 1;
                        column = 1;
                        position += 1;
                    }
                    _ => {
                        position += 1;
                        column += 1;
                    }
                }
            }
        }
        
        Ok(None)
    }
    
    /// Detect CHTL JS boundary
    fn detect_chtl_js_boundary(&self, source: &str, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<Boundary>, ScannerError> {
        // Look for CHTL JS-specific patterns
        if source[start_pos..].starts_with("{{") {
            // Find matching closing braces
            let mut position = start_pos + 2;
            let mut line = start_line;
            let mut column = start_column + 2;
            
            while position < source.len() {
                if source[position..].starts_with("}}") {
                    return Ok(Some(Boundary {
                        boundary_type: BoundaryType::CHTLJS,
                        start_position: start_pos,
                        end_position: position + 2,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 2,
                    }));
                }
                
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
        
        Ok(None)
    }
    
    /// Detect CSS boundary
    fn detect_css_boundary(&self, source: &str, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<Boundary>, ScannerError> {
        // Look for <style> tag
        if source[start_pos..].starts_with("<style") {
            // Find closing </style> tag
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < source.len() {
                if source[position..].starts_with("</style>") {
                    return Ok(Some(Boundary {
                        boundary_type: BoundaryType::CSS,
                        start_position: start_pos,
                        end_position: position + 8,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 8,
                    }));
                }
                
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
        
        Ok(None)
    }
    
    /// Detect JavaScript boundary
    fn detect_javascript_boundary(&self, source: &str, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<Boundary>, ScannerError> {
        // Look for <script> tag
        if source[start_pos..].starts_with("<script") {
            // Find closing </script> tag
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < source.len() {
                if source[position..].starts_with("</script>") {
                    return Ok(Some(Boundary {
                        boundary_type: BoundaryType::JavaScript,
                        start_position: start_pos,
                        end_position: position + 9,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 9,
                    }));
                }
                
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
        
        Ok(None)
    }
    
    /// Get all boundaries
    pub fn get_all_boundaries(&self) -> Vec<Boundary> {
        let mut all_boundaries = Vec::new();
        all_boundaries.extend(self.chtl_boundaries.clone());
        all_boundaries.extend(self.chtl_js_boundaries.clone());
        all_boundaries.extend(self.css_boundaries.clone());
        all_boundaries.extend(self.javascript_boundaries.clone());
        
        // Sort by start position
        all_boundaries.sort_by_key(|b| b.start_position);
        
        all_boundaries
    }
    
    /// Get CHTL boundaries
    pub fn get_chtl_boundaries(&self) -> &[Boundary] {
        &self.chtl_boundaries
    }
    
    /// Get CHTL JS boundaries
    pub fn get_chtl_js_boundaries(&self) -> &[Boundary] {
        &self.chtl_js_boundaries
    }
    
    /// Get CSS boundaries
    pub fn get_css_boundaries(&self) -> &[Boundary] {
        &self.css_boundaries
    }
    
    /// Get JavaScript boundaries
    pub fn get_javascript_boundaries(&self) -> &[Boundary] {
        &self.javascript_boundaries
    }
}

impl Default for CHTLUnifiedScanner {
    fn default() -> Self {
        Self::new(String::new())
    }
}

impl Default for PlaceholderManager {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for BoundaryDetector {
    fn default() -> Self {
        Self::new()
    }
}