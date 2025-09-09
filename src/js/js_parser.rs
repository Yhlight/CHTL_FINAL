//! JavaScript parser
//! 
//! This module provides JavaScript parsing functionality for CHTL,
//! including JavaScript function parsing, variable parsing, and expression parsing.

use super::*;
use std::collections::HashMap;

/// JavaScript parser
#[derive(Debug, Clone)]
pub struct JSParser {
    /// Parser options
    options: JSParserOptions,
    /// Current position
    position: usize,
    /// Current line
    line: usize,
    /// Current column
    column: usize,
    /// Source content
    content: String,
}

/// JavaScript parser options
#[derive(Debug, Clone)]
pub struct JSParserOptions {
    /// Strict mode
    pub strict: bool,
    /// Allow unknown functions
    pub allow_unknown_functions: bool,
    /// Allow unknown variables
    pub allow_unknown_variables: bool,
    /// Validate JavaScript
    pub validate: bool,
    /// Parse comments
    pub parse_comments: bool,
    /// Parse arrow functions
    pub parse_arrow_functions: bool,
    /// Parse async functions
    pub parse_async_functions: bool,
}

/// JavaScript parsing result
#[derive(Debug, Clone)]
pub struct JSParsingResult {
    /// Parsed functions
    pub functions: Vec<JSFunction>,
    /// JavaScript variables
    pub variables: HashMap<String, String>,
    /// Comments
    pub comments: Vec<Comment>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// Comment
#[derive(Debug, Clone)]
pub struct Comment {
    /// Comment content
    pub content: String,
    /// Comment type
    pub comment_type: CommentType,
    /// Line number
    pub line: usize,
    /// Column number
    pub column: usize,
}

/// Comment type
#[derive(Debug, Clone, PartialEq)]
pub enum CommentType {
    SingleLine,
    MultiLine,
    JSDoc,
}

/// JavaScript parsing error
#[derive(Debug, Clone)]
pub struct JSParsingError {
    pub message: String,
    pub line: usize,
    pub column: usize,
    pub suggestion: Option<String>,
}

impl JSParser {
    /// Create a new JavaScript parser
    pub fn new() -> Self {
        Self {
            options: JSParserOptions::default(),
            position: 0,
            line: 1,
            column: 1,
            content: String::new(),
        }
    }
    
    /// Create a new JavaScript parser with options
    pub fn with_options(options: JSParserOptions) -> Self {
        Self {
            options,
            position: 0,
            line: 1,
            column: 1,
            content: String::new(),
        }
    }
    
    /// Parse JavaScript content
    pub fn parse(&mut self, content: &str) -> Result<JSParsingResult, JSParsingError> {
        self.content = content.to_string();
        self.position = 0;
        self.line = 1;
        self.column = 1;
        
        let mut result = JSParsingResult {
            functions: Vec::new(),
            variables: HashMap::new(),
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
                '/' if self.peek_char() == Some('/') => {
                    if self.options.parse_comments {
                        self.parse_single_line_comment(&mut result)?;
                    } else {
                        self.skip_single_line_comment();
                    }
                }
                '/' if self.peek_char() == Some('*') => {
                    if self.options.parse_comments {
                        self.parse_multi_line_comment(&mut result)?;
                    } else {
                        self.skip_multi_line_comment();
                    }
                }
                'f' if self.peek_string("unction") => {
                    self.parse_function(&mut result)?;
                }
                '=' if self.peek_char() == Some('>') => {
                    if self.options.parse_arrow_functions {
                        self.parse_arrow_function(&mut result)?;
                    } else {
                        self.skip_arrow_function();
                    }
                }
                'a' if self.peek_string("sync") => {
                    if self.options.parse_async_functions {
                        self.parse_async_function(&mut result)?;
                    } else {
                        self.skip_async_function();
                    }
                }
                _ => {
                    self.advance();
                }
            }
        }
        
        Ok(result)
    }
    
    /// Parse a JavaScript function
    fn parse_function(&mut self, result: &mut JSParsingResult) -> Result<(), JSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Skip 'function'
        self.advance_by(8);
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse function name
        let name = self.parse_function_name()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect opening parenthesis
        if self.current_char() != '(' {
            return Err(JSParsingError {
                message: "Expected opening parenthesis after function name".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '(' after function name".to_string()),
            });
        }
        
        self.advance(); // Skip '('
        
        // Parse parameters
        let parameters = self.parse_parameters()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect opening brace
        if self.current_char() != '{' {
            return Err(JSParsingError {
                message: "Expected opening brace after function parameters".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '{' after function parameters".to_string()),
            });
        }
        
        self.advance(); // Skip '{'
        
        // Parse function body
        let body = self.parse_function_body()?;
        
        // Create function
        let mut function = JSFunction::new(name, JSFunctionType::Function, start_line, start_column);
        function.parameters = parameters;
        function.set_body(body);
        
        result.functions.push(function);
        
        Ok(())
    }
    
    /// Parse an arrow function
    fn parse_arrow_function(&mut self, result: &mut JSParsingResult) -> Result<(), JSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Parse function name (variable name)
        let name = self.parse_variable_name()?;
        
        // Skip '='
        self.advance();
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip '>'
        self.advance();
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse parameters
        let parameters = if self.current_char() == '(' {
            self.advance(); // Skip '('
            let params = self.parse_parameters()?;
            self.advance(); // Skip ')'
            params
        } else {
            vec![self.parse_single_parameter()?]
        };
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse function body
        let body = if self.current_char() == '{' {
            self.advance(); // Skip '{'
            self.parse_function_body()?
        } else {
            format!("return {};", self.parse_expression()?)
        };
        
        // Create function
        let mut function = JSFunction::new(name, JSFunctionType::ArrowFunction, start_line, start_column);
        function.parameters = parameters;
        function.set_body(body);
        
        result.functions.push(function);
        
        Ok(())
    }
    
    /// Parse an async function
    fn parse_async_function(&mut self, result: &mut JSParsingResult) -> Result<(), JSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        
        // Skip 'async'
        self.advance_by(5);
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip 'function'
        if self.peek_string("function") {
            self.advance_by(8);
        }
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Parse function name
        let name = self.parse_function_name()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect opening parenthesis
        if self.current_char() != '(' {
            return Err(JSParsingError {
                message: "Expected opening parenthesis after async function name".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '(' after async function name".to_string()),
            });
        }
        
        self.advance(); // Skip '('
        
        // Parse parameters
        let parameters = self.parse_parameters()?;
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Expect opening brace
        if self.current_char() != '{' {
            return Err(JSParsingError {
                message: "Expected opening brace after async function parameters".to_string(),
                line: self.line,
                column: self.column,
                suggestion: Some("Add '{' after async function parameters".to_string()),
            });
        }
        
        self.advance(); // Skip '{'
        
        // Parse function body
        let body = self.parse_function_body()?;
        
        // Create function
        let mut function = JSFunction::new(name, JSFunctionType::Async, start_line, start_column);
        function.parameters = parameters;
        function.set_body(body);
        
        result.functions.push(function);
        
        Ok(())
    }
    
    /// Parse function name
    fn parse_function_name(&mut self) -> Result<String, JSParsingError> {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '(' => break,
                _ => {
                    name.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(name.trim().to_string())
    }
    
    /// Parse variable name
    fn parse_variable_name(&mut self) -> Result<String, JSParsingError> {
        let mut name = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '=' => break,
                _ => {
                    name.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(name.trim().to_string())
    }
    
    /// Parse function parameters
    fn parse_parameters(&mut self) -> Result<Vec<String>, JSParsingError> {
        let mut parameters = Vec::new();
        let mut current_param = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ')' => break,
                ',' => {
                    if !current_param.trim().is_empty() {
                        parameters.push(current_param.trim().to_string());
                    }
                    current_param.clear();
                    self.advance();
                }
                _ => {
                    current_param.push(ch);
                    self.advance();
                }
            }
        }
        
        if !current_param.trim().is_empty() {
            parameters.push(current_param.trim().to_string());
        }
        
        Ok(parameters)
    }
    
    /// Parse single parameter
    fn parse_single_parameter(&mut self) -> Result<String, JSParsingError> {
        let mut param = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '=' | '{' => break,
                _ => {
                    param.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(param.trim().to_string())
    }
    
    /// Parse function body
    fn parse_function_body(&mut self) -> Result<String, JSParsingError> {
        let mut body = String::new();
        let mut brace_count = 0;
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                '{' => {
                    brace_count += 1;
                    body.push(ch);
                    self.advance();
                }
                '}' => {
                    if brace_count == 0 {
                        break;
                    }
                    brace_count -= 1;
                    body.push(ch);
                    self.advance();
                }
                _ => {
                    body.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(body)
    }
    
    /// Parse expression
    fn parse_expression(&mut self) -> Result<String, JSParsingError> {
        let mut expression = String::new();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ';' | '\n' | '\r' => break,
                _ => {
                    expression.push(ch);
                    self.advance();
                }
            }
        }
        
        Ok(expression.trim().to_string())
    }
    
    /// Parse single line comment
    fn parse_single_line_comment(&mut self, result: &mut JSParsingResult) -> Result<(), JSParsingError> {
        let start_line = self.line;
        let start_column = self.column;
        let mut content = String::new();
        
        // Skip '//'
        self.advance();
        self.advance();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            if ch == '\n' || ch == '\r' {
                break;
            }
            
            content.push(ch);
            self.advance();
        }
        
        result.comments.push(Comment {
            content,
            comment_type: CommentType::SingleLine,
            line: start_line,
            column: start_column,
        });
        
        Ok(())
    }
    
    /// Parse multi-line comment
    fn parse_multi_line_comment(&mut self, result: &mut JSParsingResult) -> Result<(), JSParsingError> {
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
        
        let comment_type = if content.trim().starts_with('*') {
            CommentType::JSDoc
        } else {
            CommentType::MultiLine
        };
        
        result.comments.push(Comment {
            content,
            comment_type,
            line: start_line,
            column: start_column,
        });
        
        Ok(())
    }
    
    /// Skip single line comment
    fn skip_single_line_comment(&mut self) {
        // Skip '//'
        self.advance();
        self.advance();
        
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            if ch == '\n' || ch == '\r' {
                break;
            }
            
            self.advance();
        }
    }
    
    /// Skip multi-line comment
    fn skip_multi_line_comment(&mut self) {
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
    
    /// Skip arrow function
    fn skip_arrow_function(&mut self) {
        // Skip '='
        self.advance();
        
        // Skip '>'
        self.advance();
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip function body
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
        } else {
            // Skip expression
            while self.position < self.content.len() {
                let ch = self.current_char();
                
                if ch == ';' || ch == '\n' || ch == '\r' {
                    break;
                }
                
                self.advance();
            }
        }
    }
    
    /// Skip async function
    fn skip_async_function(&mut self) {
        // Skip 'async'
        self.advance_by(5);
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip 'function'
        if self.peek_string("function") {
            self.advance_by(8);
        }
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip function name
        while self.position < self.content.len() {
            let ch = self.current_char();
            
            match ch {
                ' ' | '\t' | '\n' | '\r' | '(' => break,
                _ => self.advance(),
            }
        }
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip parameters
        if self.current_char() == '(' {
            self.advance(); // Skip '('
            
            let mut paren_count = 0;
            while self.position < self.content.len() {
                let ch = self.current_char();
                
                match ch {
                    '(' => {
                        paren_count += 1;
                        self.advance();
                    }
                    ')' => {
                        if paren_count == 0 {
                            self.advance();
                            break;
                        }
                        paren_count -= 1;
                        self.advance();
                    }
                    _ => self.advance(),
                }
            }
        }
        
        // Skip whitespace
        self.skip_whitespace();
        
        // Skip function body
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
    
    /// Peek at next string
    fn peek_string(&self, s: &str) -> bool {
        if self.position + s.len() > self.content.len() {
            return false;
        }
        
        &self.content[self.position..self.position + s.len()] == s
    }
    
    /// Advance position
    fn advance(&mut self) {
        self.position += 1;
        self.column += 1;
    }
    
    /// Advance by n characters
    fn advance_by(&mut self, n: usize) {
        for _ in 0..n {
            self.advance();
        }
    }
}

impl Default for JSParser {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for JSParserOptions {
    fn default() -> Self {
        Self {
            strict: false,
            allow_unknown_functions: true,
            allow_unknown_variables: true,
            validate: true,
            parse_comments: true,
            parse_arrow_functions: true,
            parse_async_functions: true,
        }
    }
}