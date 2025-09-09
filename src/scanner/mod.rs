//! Unified Scanner for CHTL
//! 
//! This module provides precise code cutting for CHTL, CHTL JS, CSS, and JS.

use anyhow::Result;

/// Fragment types
#[derive(Debug, Clone, Copy, PartialEq)]
pub enum FragmentType {
    Chtl,
    ChtlJS,
    CSS,
    JS,
}

/// Code fragment
#[derive(Debug, Clone)]
pub struct CodeFragment {
    pub fragment_type: FragmentType,
    pub content: String,
    pub start_pos: usize,
    pub end_pos: usize,
    pub start_line: usize,
    pub end_line: usize,
}

/// Unified scanner for CHTL
pub struct UnifiedScanner {
    source: String,
    position: usize,
    line: usize,
    column: usize,
}

impl UnifiedScanner {
    /// Create a new unified scanner
    pub fn new() -> Self {
        Self {
            source: String::new(),
            position: 0,
            line: 1,
            column: 1,
        }
    }
    
    /// Scan source code and separate into fragments
    pub fn scan(&self, source: &str) -> Result<Vec<CodeFragment>> {
        let mut scanner = Self {
            source: source.to_string(),
            position: 0,
            line: 1,
            column: 1,
        };
        
        let mut fragments = Vec::new();
        
        while !scanner.is_at_end() {
            scanner.skip_whitespace();
            
            if scanner.is_at_end() {
                break;
            }
            
            if let Some(fragment) = scanner.next_fragment()? {
                fragments.push(fragment);
            }
        }
        
        Ok(fragments)
    }
    
    /// Get the next code fragment
    fn next_fragment(&mut self) -> Result<Option<CodeFragment>> {
        let start_pos = self.position;
        let start_line = self.line;
        
        // Determine fragment type based on content
        let fragment_type = if self.is_chtl_js_start() {
            FragmentType::ChtlJS
        } else if self.is_css_start() {
            FragmentType::CSS
        } else if self.is_js_start() {
            FragmentType::JS
        } else {
            FragmentType::Chtl
        };
        
        // Scan until end of fragment
        while !self.is_at_end() {
            if self.is_fragment_end(fragment_type) {
                break;
            }
            self.advance();
        }
        
        let end_pos = self.position;
        let end_line = self.line;
        
        if start_pos == end_pos {
            return Ok(None);
        }
        
        Ok(Some(CodeFragment {
            fragment_type,
            content: self.source[start_pos..end_pos].to_string(),
            start_pos,
            end_pos,
            start_line,
            end_line,
        }))
    }
    
    /// Check if this is the start of CHTL JS code
    fn is_chtl_js_start(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("fileloader") ||
        remaining.starts_with("script") ||
        remaining.starts_with("listen") ||
        remaining.starts_with("delegate") ||
        remaining.starts_with("animate") ||
        remaining.starts_with("router") ||
        remaining.starts_with("vir")
    }
    
    /// Check if this is the start of CSS code
    fn is_css_start(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("style") ||
        remaining.starts_with("@") ||
        remaining.starts_with(".") ||
        remaining.starts_with("#")
    }
    
    /// Check if this is the start of JS code
    fn is_js_start(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("function") ||
        remaining.starts_with("var ") ||
        remaining.starts_with("let ") ||
        remaining.starts_with("const ") ||
        remaining.starts_with("if ") ||
        remaining.starts_with("for ") ||
        remaining.starts_with("while ")
    }
    
    /// Check if this is the end of a fragment
    fn is_fragment_end(&self, fragment_type: FragmentType) -> bool {
        match fragment_type {
            FragmentType::ChtlJS => self.is_chtl_js_end(),
            FragmentType::CSS => self.is_css_end(),
            FragmentType::JS => self.is_js_end(),
            FragmentType::Chtl => self.is_chtl_end(),
        }
    }
    
    /// Check if this is the end of a CHTL JS fragment
    fn is_chtl_js_end(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("}") ||
        remaining.starts_with(";") ||
        remaining.starts_with("\n")
    }
    
    /// Check if this is the end of a CSS fragment
    fn is_css_end(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("}") ||
        remaining.starts_with(";") ||
        remaining.starts_with("\n")
    }
    
    /// Check if this is the end of a JS fragment
    fn is_js_end(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("}") ||
        remaining.starts_with(";") ||
        remaining.starts_with("\n")
    }
    
    /// Check if this is the end of a CHTL fragment
    fn is_chtl_end(&self) -> bool {
        let remaining = &self.source[self.position..];
        remaining.starts_with("}") ||
        remaining.starts_with(";") ||
        remaining.starts_with("\n")
    }
    
    fn is_at_end(&self) -> bool {
        self.position >= self.source.len()
    }
    
    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.position)
    }
    
    fn peek_next(&self) -> Option<char> {
        self.source.chars().nth(self.position + 1)
    }
    
    fn advance(&mut self) -> Option<char> {
        if self.position < self.source.len() {
            let ch = self.source.chars().nth(self.position);
            self.position += 1;
            if ch == Some('\n') {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            ch
        } else {
            None
        }
    }
    
    fn skip_whitespace(&mut self) {
        while !self.is_at_end() && self.peek().map_or(false, |c| c.is_whitespace()) {
            self.advance();
        }
    }
}