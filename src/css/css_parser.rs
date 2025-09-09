//! CSS parser
//! 
//! This module provides CSS parsing functionality for CHTL,
//! including CSS rule parsing, property parsing, and selector parsing.

use super::*;
use std::collections::HashMap;

/// CSS parser
#[derive(Debug, Clone)]
pub struct CSSParser {
    /// Parser options
    options: CSSParserOptions,
    /// Current position
    position: usize,
    /// Current line
    line: usize,
    /// Current column
    column: usize,
    /// Source content
    content: String,
}

/// CSS parser options
#[derive(Debug, Clone)]
pub struct CSSParserOptions {
    /// Strict mode
    pub strict: bool,
    /// Allow unknown properties
    pub allow_unknown_properties: bool,
    /// Allow unknown selectors
    pub allow_unknown_selectors: bool,
    /// Validate CSS
    pub validate: bool,
    /// Parse comments
    pub parse_comments: bool,
    /// Parse at-rules
    pub parse_at_rules: bool,
}

/// CSS parsing result
#[derive(Debug, Clone)]
pub struct CSSParsingResult {
    /// Parsed rules
    pub rules: Vec<CSSRule>,
    /// CSS variables
    pub variables: HashMap<String, String>,
    /// At-rules
    pub at_rules: Vec<AtRule>,
    /// Comments
    pub comments: Vec<Comment>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// At-rule
#[derive(Debug, Clone)]
pub struct AtRule {
    /// Rule name
    pub name: String,
    /// Rule parameters
    pub parameters: String,
    /// Rule content
    pub content: String,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

/// Comment
#[derive(Debug, Clone)]
pub struct Comment {
    /// Comment content
    pub content: String,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

/// CSS parsing error
#[derive(Debug, Clone)]
pub struct CSSParsingError {
    pub message: String,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

impl CSSParser {
    /// Create a new CSS parser
    pub fn new() -> Self {
        Self {
            options: CSSParserOptions::default(),
            position: 0,
            line: 1,
            column: 1,
            content: String::new(),
        }
    }
    
    /// Create a new CSS parser with options
    pub fn with_options(options: CSSParserOptions) -> Self {
        Self {
            options,
            position: 0,
            line: 1,
            column: 1,
            content: String::new(),
        }
    }
    
    /// Parse CSS content
    pub fn parse(&mut self, content: &str) -> Result<CSSParsingResult, CSSParsingError> {
        self.content = content.to_string();
        self.position = 0;
        self.line = 1;
        self.column = 1;
        
        let mut result = CSSParsingResult {
            rules: Vec::new(),
            variables: HashMap::new(),
            at_rules: Vec::new(),
            comments: Vec::new(),
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        while self.position < self.content.len() {
            self.skip_whitespace();
            
            if self.position >= self.content.len() {
                break;
            }
            
            let ch = self.current_char();
            
            match ch {
                '/' if self.peek_char() == Some('*') => {
                    if self.options.parse_comments {
                        self.parse_comment(&mut result)?;
                    } else {
                        self.skip_comment();
                    }
                }
                '@' => {
                    if self.options.parse_at_rules {
                        self.parse_at_rule(&mut result)?;
                    } else {
                        self.skip_at_rule();
                    }
                }
                _ => {
                    self.parse_rule(&mut result)?;
                }
            }
        }
        
        Ok(result)
    }
    
    /// Parse a CSS rule
    fn parse_rule(&mut self, result: &mut CSSParsingResult) -> Result<(), CSSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Parse selector
        let selector = self.parse_selector()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect opening brace
        if self.current_char() != '{' {
            return Err(CSSParsingError {
                message: "Expected opening brace after selector".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '{' after selector".to_string()),
            });
        }
        
        self.advance(); // Skip '{'
        
        // Parse properties
        let mut rule = CSSRule::new(selector, start_line, start_column);
        
        while self.position < self.content.len() {
            self.skip_whitespace();
            
            if self.current_char() == '}' {
                break;
            }
            
            let property = self.parse_property()?;
            rule.add_property(property.name, property.value);
            
            // Skip semicolon
            if self.current_char() == ';' {
                self.advance();
            }
        }
        
        // Expect closing brace
        if self.current_char() != '}' {
            return Err(CSSParsingError {
                message: "Expected closing brace".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '}' to close rule".to_string()),
            });
        }
        
        self.advance(); // Skip '}'
        
        // Calculate specificity
        rule.calculate_specificity();
        
        // Add rule to result
        result.rules.push(rule);
        
        Ok(())
    }
    
    /// Parse a CSS selector
    fn parse_selector(&mut self) -> Result<String, CSSParsingError> {
        let mut selector = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '{' => break,
                '\n' | '\r' => {
                    self.advance();
                    self.line += 1;
                    self.column = 1;
                }
                _ => {
                    selector.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(selector.trim().to_string())
    }
    
    /// Parse a CSS property
    fn parse_property(&mut self) -> Result<Property, CSSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Parse property name
        let name = self.parse_property_name()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect colon
        if self.current_char() != ':' {
            return Err(CSSParsingError {
                message: "Expected colon after property name".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add ':' after property name".to_string()),
            });
        }
        
        self.advance(); // Skip ':'
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse property value
        let value = self.parse_property_value()?;
        
        Ok(Property {
            name,
            value,
            line: start_line,
            column: start_column,
        })
    }
    
    /// Parse property name
    fn parse_property_name(&mut self) -> Result<String, CSSParsingError> {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ':' => break,
                ' ' | '\t' | '\n' | '\r' => break,
                _ => {
                    name.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(name.trim().to_string())
    }
    
    /// Parse property value
    fn parse_property_value(&mut self) -> Result<String, CSSParsingError> {
        let mut value = String::new();
        let mut in_string = false;
        let mut string_char = '\0';
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '"' | '\'' if !in_string => {
                    in_string = true;
                    string_char = ch;
                    value.push(ch);
                    self.advance();
                }
                _ if in_string && ch == string_char => {
                    in_string = false;
                    value.push(ch);
                    self.advance();
                }
                ';' if !in_string => break,
                '}' if !in_string => break,
                _ => {
                    value.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(value.trim().to_string())
    }
    
    /// Parse CSS comment
    fn parse_comment(&mut self, result: &mut CSSParsingResult) -> Result<(), CSSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        let mut content = String::new();
        
        // Skip '/*'
        self.advance();
        self.advance();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            if ch == '*' && self.peek_char() == Some('/') {
                self.advance(); // Skip '*'
                self.advance(); // Skip '/'
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        result.comments.push(Comment {
            content,
            line: start_line,
            column: start_column,
        });
        
        Ok(())
    }
    
    /// Parse at-rule
    fn parse_at_rule(&mut self, result: &mut CSSParsingResult) -> Result<(), CSSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Skip '@'
        self.advance();
        
        // Parse rule name
        let name = self.parse_at_rule_name()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse parameters
        let parameters = self.parse_at_rule_parameters()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse content
        let content = if self.current_char() == '{' {
            self.parse_at_rule_content()?
        } else {
            String::new()
        };
        
        result.at_rules.push(AtRule {
            name,
            parameters,
            content,
            line: start_line,
            column: start_column,
        });
        
        Ok(())
    }
    
    /// Parse at-rule name
    fn parse_at_rule_name(&mut self) -> Result<String, CSSParsingError> {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '{' | ';' => break,
                _ => {
                    name.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(name)
    }
    
    /// Parse at-rule parameters
    fn parse_at_rule_parameters(&mut self) -> Result<String, CSSParsingError> {
        let mut parameters = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '{' | ';' => break,
                _ => {
                    parameters.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(parameters.trim().to_string())
    }
    
    /// Parse at-rule content
    fn parse_at_rule_content(&mut self) -> Result<String, CSSParsingError> {
        let mut content = String::new();
        let mut brace_count = 0;
        
        // Skip opening brace
        self.advance();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '{' => {
                    brace_count += 1;
                    content.push(ch);
                    self.advance();
                }
                '}' => {
                    if brace_count == 0 {
                        self.advance(); // Skip closing brace
                        break;
                    }
                    brace_count -= 1;
                    content.push(ch);
                    self.advance();
                }
                _ => {
                    content.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(content)
    }
    
    /// Skip comment
    fn skip_comment(&mut self) {
        // Skip '/*'
        self.advance();
        self.advance();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            if ch == '*' && self.peek_char() == Some('/') {
                self.advance(); // Skip '*'
                self.advance(); // Skip '/'
                break;
            }
            
            self.advance();
        }
    }
    
    /// Skip at-rule
    fn skip_at_rule(&mut self) {
        // Skip '@'
        self.advance();
        
        // Skip rule name
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '{' | ';' => break,
                _ => self.advance(),
            }
        }
        
        // Skip parameters
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '{' | ';' => break,
                _ => self.advance(),
            }
        }
        
        // Skip content
        if self.current_char() == '{' {
            let mut brace_count = 0;
            
            while self.position < self.content.len() {
                let ch = self.current_char();
                
                match ch {
                    '{' => {
                        brace_count += 1;
                        self.advance();
                    }
                    '}' => {
                        if brace_count == 0 {
                            self.advance();
                            break;
                        }
                        brace_count -= 1;
                        self.advance();
                    }
                    _ => self.advance(),
                }
            }
        }
    }
    
    /// Skip whitespace
    fn skip_whitespace(&mut self) {
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' => {
                    self.advance();
                }
                '\n' | '\r' => {
                    self.advance();
                    self.line += 1;
                    self.column = 1;
                }
                _ => break,
            }
        }
    }
    
    /// Get current character
    fn current_char(&self) -> char {
        self.content.chars().nth(self.position).unwrap_or('\0')
    }
    
    /// Peek at next character
    fn peek_char(&self) -> Option<char> {
        self.content.chars().nth(self.position + 1)
    }
    
    /// Advance position
    fn advance(&mut self) {
        self.position += 1;
        self.column += 1;
    }
}

/// Property
#[derive(Debug, Clone)]
pub struct Property {
    /// Property name
    pub name: String,
    /// Property value
    pub value: String,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

impl Default for CSSParser {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CSSParserOptions {
    fn default() -> Self {
        Self {
            strict: false,
            allow_unknown_properties: true,
            allow_unknown_selectors: true,
            validate: true,
            parse_comments: true,
            parse_at_rules: true,
        }
    }
}