//! CHTL JS CJMOD API
//! 
//! This module provides the C++ interface for CHTL JS extensions.

use anyhow::Result;
use std::collections::HashMap;

/// CJMOD API for C++ interface
pub struct CjmodAPI {
    /// Syntax definitions
    syntax: HashMap<String, Syntax>,
    /// Function definitions
    functions: HashMap<String, CHTLJSFunction>,
}

/// Syntax definition
#[derive(Debug, Clone)]
pub struct Syntax {
    pub name: String,
    pub pattern: String,
    pub handler: String,
}

/// Function argument
#[derive(Debug, Clone)]
pub struct Arg {
    pub name: String,
    pub arg_type: ArgType,
    pub required: bool,
    pub default_value: Option<String>,
}

/// Argument type
#[derive(Debug, Clone)]
pub enum ArgType {
    String,
    Number,
    Boolean,
    Object,
    Array,
    Function,
}

/// CHTL JS Function definition
#[derive(Debug, Clone)]
pub struct CHTLJSFunction {
    pub name: String,
    pub args: Vec<Arg>,
    pub return_type: ArgType,
    pub handler: String,
}

/// CJMOD Scanner for parsing CHTL JS code
pub struct CjmodScanner {
    source: String,
    current: usize,
    line: usize,
    column: usize,
}

/// CJMOD Generator for generating CHTL JS code
pub struct CjmodGenerator {
    output: String,
    indent_level: usize,
}

impl CjmodAPI {
    /// Create a new CJMOD API
    pub fn new() -> Self {
        Self {
            syntax: HashMap::new(),
            functions: HashMap::new(),
        }
    }
    
    /// Register a syntax definition
    pub fn register_syntax(&mut self, syntax: Syntax) {
        self.syntax.insert(syntax.name.clone(), syntax);
    }
    
    /// Register a function definition
    pub fn register_function(&mut self, function: CHTLJSFunction) {
        self.functions.insert(function.name.clone(), function);
    }
    
    /// Get syntax definition
    pub fn get_syntax(&self, name: &str) -> Option<&Syntax> {
        self.syntax.get(name)
    }
    
    /// Get function definition
    pub fn get_function(&self, name: &str) -> Option<&CHTLJSFunction> {
        self.functions.get(name)
    }
    
    /// List all registered syntaxes
    pub fn list_syntaxes(&self) -> Vec<&Syntax> {
        self.syntax.values().collect()
    }
    
    /// List all registered functions
    pub fn list_functions(&self) -> Vec<&CHTLJSFunction> {
        self.functions.values().collect()
    }
}

impl CjmodScanner {
    /// Create a new CJMOD scanner
    pub fn new(source: String) -> Self {
        Self {
            source,
            current: 0,
            line: 1,
            column: 1,
        }
    }
    
    /// Scan the source code
    pub fn scan(&mut self) -> Result<Vec<AtomArg>> {
        let mut atoms = Vec::new();
        
        while !self.is_at_end() {
            self.skip_whitespace();
            
            if self.is_at_end() {
                break;
            }
            
            if let Some(atom) = self.scan_atom()? {
                atoms.push(atom);
            }
        }
        
        Ok(atoms)
    }
    
    /// Scan a single atom
    fn scan_atom(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        let ch = self.peek().unwrap();
        
        match ch {
            '"' => self.scan_string(),
            '\'' => self.scan_string(),
            '0'..='9' => self.scan_number(),
            'a'..='z' | 'A'..='Z' | '_' => self.scan_identifier(),
            '{' => self.scan_object(),
            '[' => self.scan_array(),
            '(' => self.scan_function(),
            _ => {
                self.advance();
                Ok(None)
            }
        }
    }
    
    /// Scan a string literal
    fn scan_string(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        let quote = self.advance().unwrap();
        
        while !self.is_at_end() && self.peek() != Some(quote) {
            if self.peek() == Some('\\') {
                self.advance(); // consume backslash
            }
            self.advance();
        }
        
        if self.peek() == Some(quote) {
            self.advance(); // consume closing quote
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::String(value)))
    }
    
    /// Scan a number literal
    fn scan_number(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        
        while !self.is_at_end() && self.peek().map_or(false, |c| c.is_ascii_digit() || c == '.') {
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::Number(value)))
    }
    
    /// Scan an identifier
    fn scan_identifier(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        
        while !self.is_at_end() && self.peek().map_or(false, |c| c.is_ascii_alphanumeric() || c == '_') {
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::Identifier(value)))
    }
    
    /// Scan an object
    fn scan_object(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        self.advance(); // consume opening brace
        
        let mut brace_count = 1;
        while !self.is_at_end() && brace_count > 0 {
            match self.peek() {
                Some('{') => brace_count += 1,
                Some('}') => brace_count -= 1,
                _ => {}
            }
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::Object(value)))
    }
    
    /// Scan an array
    fn scan_array(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        self.advance(); // consume opening bracket
        
        let mut bracket_count = 1;
        while !self.is_at_end() && bracket_count > 0 {
            match self.peek() {
                Some('[') => bracket_count += 1,
                Some(']') => bracket_count -= 1,
                _ => {}
            }
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::Array(value)))
    }
    
    /// Scan a function
    fn scan_function(&mut self) -> Result<Option<AtomArg>> {
        let start = self.current;
        self.advance(); // consume opening parenthesis
        
        let mut paren_count = 1;
        while !self.is_at_end() && paren_count > 0 {
            match self.peek() {
                Some('(') => paren_count += 1,
                Some(')') => paren_count -= 1,
                _ => {}
            }
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(AtomArg::Function(value)))
    }
    
    /// Skip whitespace characters
    fn skip_whitespace(&mut self) {
        while !self.is_at_end() && self.peek().map_or(false, |c| c.is_whitespace()) {
            if self.peek() == Some('\n') {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            self.advance();
        }
    }
    
    /// Check if we're at the end of the source
    fn is_at_end(&self) -> bool {
        self.current >= self.source.len()
    }
    
    /// Peek at the current character
    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.current)
    }
    
    /// Advance to the next character
    fn advance(&mut self) -> Option<char> {
        if self.current < self.source.len() {
            let ch = self.source.chars().nth(self.current);
            self.current += 1;
            self.column += 1;
            ch
        } else {
            None
        }
    }
}

impl CjmodGenerator {
    /// Create a new CJMOD generator
    pub fn new() -> Self {
        Self {
            output: String::new(),
            indent_level: 0,
        }
    }
    
    /// Generate CHTL JS code
    pub fn generate(&mut self, atoms: Vec<AtomArg>) -> Result<String> {
        for atom in atoms {
            self.generate_atom(atom)?;
        }
        Ok(self.output.clone())
    }
    
    /// Generate code for a single atom
    fn generate_atom(&mut self, atom: AtomArg) -> Result<()> {
        match atom {
            AtomArg::String(s) => {
                self.output.push_str(&format!("\"{}\"", s));
            }
            AtomArg::Number(n) => {
                self.output.push_str(&n);
            }
            AtomArg::Identifier(id) => {
                self.output.push_str(&id);
            }
            AtomArg::Object(obj) => {
                self.output.push_str(&obj);
            }
            AtomArg::Array(arr) => {
                self.output.push_str(&arr);
            }
            AtomArg::Function(func) => {
                self.output.push_str(&func);
            }
        }
        Ok(())
    }
}

/// Atom argument for CJMOD scanning
#[derive(Debug, Clone)]
pub enum AtomArg {
    String(String),
    Number(String),
    Identifier(String),
    Object(String),
    Array(String),
    Function(String),
}