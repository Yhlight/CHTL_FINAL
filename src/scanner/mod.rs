//! CHTL Unified Scanner
//! 
//! This module provides the unified scanner that separates CHTL, CHTL JS, CSS, and JS code.

use anyhow::Result;
use std::collections::VecDeque;

/// Code fragment types
#[derive(Debug, Clone, PartialEq)]
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
    pub position: usize,
    pub line: usize,
    pub column: usize,
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
        let ch = self.peek()?;
        self.position += 1;
        if ch == '\n' {
            self.line += 1;
            self.column = 1;
        } else {
            self.column += 1;
        }
        Some(ch)
    }
    
    fn skip_whitespace(&mut self) {
        while let Some(ch) = self.peek() {
            if ch.is_whitespace() && ch != '\n' {
                self.advance();
            } else {
                break;
            }
        }
    }
    
    fn next_fragment(&mut self) -> Result<Option<CodeFragment>> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_column = self.column;
        
        let ch = match self.peek() {
            Some(c) => c,
            None => return Ok(None),
        };
        
        // Check for CHTL JS syntax
        if self.is_chtl_js_start() {
            return self.scan_chtl_js_fragment(start_pos, start_line, start_column);
        }
        
        // Check for CSS syntax
        if self.is_css_start() {
            return self.scan_css_fragment(start_pos, start_line, start_column);
        }
        
        // Check for JS syntax
        if self.is_js_start() {
            return self.scan_js_fragment(start_pos, start_line, start_column);
        }
        
        // Default to CHTL
        self.scan_chtl_fragment(start_pos, start_line, start_column)
    }
    
    fn is_chtl_js_start(&self) -> bool {
        // Check for CHTL JS specific syntax like {{ }} or vir, listen, etc.
        let remaining = &self.source[self.position..];
        remaining.starts_with("{{") || 
        remaining.starts_with("vir ") ||
        remaining.starts_with("listen ") ||
        remaining.starts_with("animate ") ||
        remaining.starts_with("router ")
    }
    
    fn is_css_start(&self) -> bool {
        // Check for CSS syntax
        let remaining = &self.source[self.position..];
        remaining.starts_with("style") ||
        remaining.starts_with("@media") ||
        remaining.starts_with("@keyframes") ||
        remaining.starts_with("@import")
    }
    
    fn is_js_start(&self) -> bool {
        // Check for JavaScript syntax
        let remaining = &self.source[self.position..];
        remaining.starts_with("function ") ||
        remaining.starts_with("const ") ||
        remaining.starts_with("let ") ||
        remaining.starts_with("var ") ||
        remaining.starts_with("class ") ||
        remaining.starts_with("if ") ||
        remaining.starts_with("for ") ||
        remaining.starts_with("while ")
    }
    
    fn scan_chtl_js_fragment(&mut self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeFragment>> {
        let mut content = String::new();
        
        // Scan until we find the end of CHTL JS fragment
        while !self.is_at_end() {
            let ch = self.peek().unwrap();
            
            if ch == '}' && self.peek_next() == Some('}') {
                // End of CHTL JS fragment
                content.push(ch);
                self.advance();
                content.push(self.advance().unwrap());
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        Ok(Some(CodeFragment {
            fragment_type: FragmentType::ChtlJS,
            content,
            position: start_pos,
            line: start_line,
            column: start_column,
        }))
    }
    
    fn scan_css_fragment(&mut self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeFragment>> {
        let mut content = String::new();
        
        // Scan CSS content
        while !self.is_at_end() {
            let ch = self.peek().unwrap();
            
            if ch == '}' && self.is_css_end() {
                content.push(ch);
                self.advance();
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        Ok(Some(CodeFragment {
            fragment_type: FragmentType::CSS,
            content,
            position: start_pos,
            line: start_line,
            column: start_column,
        }))
    }
    
    fn scan_js_fragment(&mut self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeFragment>> {
        let mut content = String::new();
        
        // Scan JavaScript content
        while !self.is_at_end() {
            let ch = self.peek().unwrap();
            
            if ch == '}' && self.is_js_end() {
                content.push(ch);
                self.advance();
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        Ok(Some(CodeFragment {
            fragment_type: FragmentType::JS,
            content,
            position: start_pos,
            line: start_line,
            column: start_column,
        }))
    }
    
    fn scan_chtl_fragment(&mut self, start_pos: usize, start_line: usize, start_column: usize) -> Result<Option<CodeFragment>> {
        let mut content = String::new();
        
        // Scan CHTL content
        while !self.is_at_end() {
            let ch = self.peek().unwrap();
            
            if ch == '}' && self.is_chtl_end() {
                content.push(ch);
                self.advance();
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        Ok(Some(CodeFragment {
            fragment_type: FragmentType::Chtl,
            content,
            position: start_pos,
            line: start_line,
            column: start_column,
        }))
    }
    
    fn is_css_end(&self) -> bool {
        // Check if we're at the end of a CSS block
        true // Simplified
    }
    
    fn is_js_end(&self) -> bool {
        // Check if we're at the end of a JS block
        true // Simplified
    }
    
    fn is_chtl_end(&self) -> bool {
        // Check if we're at the end of a CHTL block
        true // Simplified
    }
}