//! Code separator
//! 
//! This module provides code separation functionality for CHTL,
//! including separating different types of code and managing code boundaries.

use std::collections::HashMap;
use crate::chtl::context::*;

/// Code separator
#[derive(Debug, Clone)]
pub struct CodeSeparator {
    /// Source code
    source: String,
    /// Separated code blocks
    code_blocks: HashMap<CodeBlockType, Vec<CodeBlock>>,
    /// Code boundaries
    boundaries: Vec<CodeBoundary>,
}

/// Code block type
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum CodeBlockType {
    CHTL,
    CHTLJS,
    CSS,
    JavaScript,
    HTML,
    Comment,
    Whitespace,
    Unknown,
}

/// Code block
#[derive(Debug, Clone)]
pub struct CodeBlock {
    /// Block type
    pub block_type: CodeBlockType,
    /// Block content
    pub content: String,
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
    /// Block metadata
    pub metadata: HashMap<String, String>,
}

/// Code boundary
#[derive(Debug, Clone)]
pub struct CodeBoundary {
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
    /// Boundary metadata
    pub metadata: HashMap<String, String>,
}

/// Boundary type
#[derive(Debug, Clone, PartialEq)]
pub enum BoundaryType {
    Block,
    Inline,
    Comment,
    String,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
}

/// Separator error
#[derive(Debug, Clone)]
pub struct SeparatorError {
    pub message: String,
    pub position: usize,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

impl CodeSeparator {
    /// Create a new code separator
    pub fn new(source: String) -> Self {
        Self {
            source,
            code_blocks: HashMap::new(),
            boundaries: Vec::new(),
        }
    }
    
    /// Separate the source code
    pub fn separate(&mut self) -> Result<(), SeparatorError> {
        self.code_blocks.clear();
        self.boundaries.clear();
        
        // Detect boundaries
        self.detect_boundaries()?;
        
        // Extract code blocks
        self.extract_code_blocks()?;
        
        Ok(())
    }
    
    /// Detect code boundaries
    fn detect_boundaries(&mut self) -> Result<(), SeparatorError> {
        let mut position = 0;
        let mut line = 1;
        let mut column = 1;
        
        while position < self.source.len() {
            let current_char = self.source.chars().nth(position).unwrap();
            
            match current_char {
                // CHTL boundaries
                '[' => {
                    if let Some(boundary) = self.detect_chtl_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
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
                    if let Some(boundary) = self.detect_chtl_js_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // CSS boundaries
                '<' if self.source[position..].starts_with("<style") => {
                    if let Some(boundary) = self.detect_css_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // JavaScript boundaries
                '<' if self.source[position..].starts_with("<script") => {
                    if let Some(boundary) = self.detect_javascript_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // HTML boundaries
                '<' => {
                    if let Some(boundary) = self.detect_html_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // Comment boundaries
                '/' if self.source[position..].starts_with("//") => {
                    if let Some(boundary) = self.detect_comment_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // Multi-line comment boundaries
                '/' if self.source[position..].starts_with("/*") => {
                    if let Some(boundary) = self.detect_multiline_comment_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
                        position = boundary.end_position;
                        line = boundary.end_line;
                        column = boundary.end_column;
                    } else {
                        position += 1;
                        column += 1;
                    }
                }
                // String boundaries
                '"' | '\'' => {
                    if let Some(boundary) = self.detect_string_boundary(position, line, column)? {
                        self.boundaries.push(boundary.clone());
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
    fn detect_chtl_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for CHTL-specific patterns
        if self.source[start_pos..].starts_with("[Template]") ||
           self.source[start_pos..].starts_with("[Custom]") ||
           self.source[start_pos..].starts_with("[Origin]") ||
           self.source[start_pos..].starts_with("[Import]") ||
           self.source[start_pos..].starts_with("[Namespace]") ||
           self.source[start_pos..].starts_with("[Configuration]") {
            
            // Find matching closing bracket
            let mut depth = 0;
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < self.source.len() {
                let current_char = self.source.chars().nth(position).unwrap();
                
                match current_char {
                    '[' => {
                        depth += 1;
                        position += 1;
                        column += 1;
                    }
                    ']' => {
                        depth -= 1;
                        if depth == 0 {
                            return Ok(Some(CodeBoundary {
                                boundary_type: BoundaryType::Block,
                                start_position: start_pos,
                                end_position: position + 1,
                                start_line,
                                end_line: line,
                                start_column,
                                end_column: column,
                                metadata: HashMap::new(),
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
    fn detect_chtl_js_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for CHTL JS-specific patterns
        if self.source[start_pos..].starts_with("{{") {
            // Find matching closing braces
            let mut position = start_pos + 2;
            let mut line = start_line;
            let mut column = start_column + 2;
            
            while position < self.source.len() {
                if self.source[position..].starts_with("}}") {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Inline,
                        start_position: start_pos,
                        end_position: position + 2,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 2,
                        metadata: HashMap::new(),
                    }));
                }
                
                let current_char = self.source.chars().nth(position).unwrap();
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
    fn detect_css_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for <style> tag
        if self.source[start_pos..].starts_with("<style") {
            // Find closing </style> tag
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < self.source.len() {
                if self.source[position..].starts_with("</style>") {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Block,
                        start_position: start_pos,
                        end_position: position + 8,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 8,
                        metadata: HashMap::new(),
                    }));
                }
                
                let current_char = self.source.chars().nth(position).unwrap();
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
    fn detect_javascript_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for <script> tag
        if self.source[start_pos..].starts_with("<script") {
            // Find closing </script> tag
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < self.source.len() {
                if self.source[position..].starts_with("</script>") {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Block,
                        start_position: start_pos,
                        end_position: position + 9,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 9,
                        metadata: HashMap::new(),
                    }));
                }
                
                let current_char = self.source.chars().nth(position).unwrap();
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
    
    /// Detect HTML boundary
    fn detect_html_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for HTML tag
        if self.source[start_pos..].starts_with("<") {
            // Find closing > tag
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < self.source.len() {
                let current_char = self.source.chars().nth(position).unwrap();
                
                if current_char == '>' {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Inline,
                        start_position: start_pos,
                        end_position: position + 1,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 1,
                        metadata: HashMap::new(),
                    }));
                }
                
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
    
    /// Detect comment boundary
    fn detect_comment_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for // comment
        if self.source[start_pos..].starts_with("//") {
            // Find end of line
            let mut position = start_pos;
            let mut line = start_line;
            let mut column = start_column;
            
            while position < self.source.len() {
                let current_char = self.source.chars().nth(position).unwrap();
                
                if current_char == '\n' {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Inline,
                        start_position: start_pos,
                        end_position: position,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column,
                        metadata: HashMap::new(),
                    }));
                }
                
                position += 1;
                column += 1;
            }
        }
        
        Ok(None)
    }
    
    /// Detect multi-line comment boundary
    fn detect_multiline_comment_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        // Look for /* comment
        if self.source[start_pos..].starts_with("/*") {
            // Find closing */
            let mut position = start_pos + 2;
            let mut line = start_line;
            let mut column = start_column + 2;
            
            while position < self.source.len() {
                if self.source[position..].starts_with("*/") {
                    return Ok(Some(CodeBoundary {
                        boundary_type: BoundaryType::Block,
                        start_position: start_pos,
                        end_position: position + 2,
                        start_line,
                        end_line: line,
                        start_column,
                        end_column: column + 2,
                        metadata: HashMap::new(),
                    }));
                }
                
                let current_char = self.source.chars().nth(position).unwrap();
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
    
    /// Detect string boundary
    fn detect_string_boundary(&self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeBoundary>, SeparatorError> {
        let quote_char = self.source.chars().nth(start_pos).unwrap();
        
        // Find closing quote
        let mut position = start_pos + 1;
        let mut line = start_line;
        let mut column = start_column + 1;
        
        while position < self.source.len() {
            let current_char = self.source.chars().nth(position).unwrap();
            
            if current_char == quote_char {
                // Check for escaped quote
                if position > 0 && self.source.chars().nth(position - 1).unwrap() == '\\' {
                    position += 1;
                    column += 1;
                    continue;
                }
                
                return Ok(Some(CodeBoundary {
                    boundary_type: BoundaryType::Inline,
                    start_position: start_pos,
                    end_position: position + 1,
                    start_line,
                    end_line: line,
                    start_column,
                    end_column: column + 1,
                    metadata: HashMap::new(),
                }));
            }
            
            if current_char == '\n' {
                line += 1;
                column = 1;
            } else {
                column += 1;
            }
            position += 1;
        }
        
        Ok(None)
    }
    
    /// Extract code blocks
    fn extract_code_blocks(&mut self) -> Result<(), SeparatorError> {
        // Sort boundaries by start position
        self.boundaries.sort_by_key(|b| b.start_position);
        
        let mut last_position = 0;
        
        for boundary in &self.boundaries {
            // Extract content before boundary
            if boundary.start_position > last_position {
                let content = &self.source[last_position..boundary.start_position];
                if !content.trim().is_empty() {
                    let block_type = self.classify_content(content);
                    let block = CodeBlock {
                        block_type,
                        content: content.to_string(),
                        start_position: last_position,
                        end_position: boundary.start_position,
                        start_line: 1, // TODO: Calculate actual line
                        end_line: 1,   // TODO: Calculate actual line
                        start_column: 1, // TODO: Calculate actual column
                        end_column: 1,   // TODO: Calculate actual column
                        metadata: HashMap::new(),
                    };
                    
                    self.code_blocks.entry(block_type).or_insert_with(Vec::new).push(block);
                }
            }
            
            // Extract boundary content
            let content = &self.source[boundary.start_position..boundary.end_position];
            let block_type = self.classify_boundary(&boundary.boundary_type);
            let block = CodeBlock {
                block_type,
                content: content.to_string(),
                start_position: boundary.start_position,
                end_position: boundary.end_position,
                start_line: boundary.start_line,
                end_line: boundary.end_line,
                start_column: boundary.start_column,
                end_column: boundary.end_column,
                metadata: HashMap::new(),
            };
            
            self.code_blocks.entry(block_type).or_insert_with(Vec::new).push(block);
            
            last_position = boundary.end_position;
        }
        
        // Extract remaining content
        if last_position < self.source.len() {
            let content = &self.source[last_position..];
            if !content.trim().is_empty() {
                let block_type = self.classify_content(content);
                let block = CodeBlock {
                    block_type,
                    content: content.to_string(),
                    start_position: last_position,
                    end_position: self.source.len(),
                    start_line: 1, // TODO: Calculate actual line
                    end_line: 1,   // TODO: Calculate actual line
                    start_column: 1, // TODO: Calculate actual column
                    end_column: 1,   // TODO: Calculate actual column
                    metadata: HashMap::new(),
                };
                
                self.code_blocks.entry(block_type).or_insert_with(Vec::new).push(block);
            }
        }
        
        Ok(())
    }
    
    /// Classify content type
    fn classify_content(&self, content: &str) -> CodeBlockType {
        let trimmed = content.trim();
        
        if trimmed.starts_with('<') && trimmed.ends_with('>') {
            CodeBlockType::HTML
        } else if trimmed.starts_with("//") || trimmed.starts_with("/*") {
            CodeBlockType::Comment
        } else if trimmed.chars().all(|c| c.is_whitespace()) {
            CodeBlockType::Whitespace
        } else {
            CodeBlockType::Unknown
        }
    }
    
    /// Classify boundary type
    fn classify_boundary(&self, boundary_type: &BoundaryType) -> CodeBlockType {
        match boundary_type {
            BoundaryType::Block => CodeBlockType::CHTL,
            BoundaryType::Inline => CodeBlockType::CHTLJS,
            _ => CodeBlockType::Unknown,
        }
    }
    
    /// Get code blocks by type
    pub fn get_code_blocks(&self, block_type: CodeBlockType) -> Option<&Vec<CodeBlock>> {
        self.code_blocks.get(&block_type)
    }
    
    /// Get all code blocks
    pub fn get_all_code_blocks(&self) -> &HashMap<CodeBlockType, Vec<CodeBlock>> {
        &self.code_blocks
    }
    
    /// Get boundaries
    pub fn get_boundaries(&self) -> &[CodeBoundary] {
        &self.boundaries
    }
}

impl Default for CodeSeparator {
    fn default() -> Self {
        Self::new(String::new())
    }
}