//! CHTL - A hypertext language based on Rust
//! 
//! CHTL is a hypertext language that provides a more developer-friendly way
//! to write HTML code, using MIT open source license.

pub mod lexer;
pub mod parser;
pub mod parser_simple;
pub mod ast;
pub mod ast_enum;
pub mod generator;
pub mod error;
pub mod utils;

pub use ast_enum::*;

pub use error::{ChtlError, Result};

/// CHTL compiler main entry point
pub struct ChtlCompiler {
    configuration: Option<Configuration>,
}

impl ChtlCompiler {
    /// Create a new CHTL compiler instance
    pub fn new() -> Self {
        Self {
            configuration: None,
        }
    }
    
    /// Compile CHTL source code to HTML
    pub fn compile(&self, source: &str) -> Result<String> {
        // Step 1: Lexical analysis
        let mut lexer = lexer::Lexer::new(source.to_string());
        let tokens = lexer.tokenize()?;
        
        // Step 2: Parsing
        let mut parser = parser_simple::Parser::new(tokens, source.to_string());
        let ast = parser.parse()?;
        
        // Step 3: Code generation
        let mut html_generator = generator::HtmlGenerator::new();
        let html = html_generator.generate_enum(&ast)?;
        
        Ok(html)
    }
    
    /// Compile CHTL source code to HTML with CSS and JS
    pub fn compile_full(&self, source: &str) -> Result<(String, String, String)> {
        // Step 1: Lexical analysis
        let mut lexer = lexer::Lexer::new(source.to_string());
        let tokens = lexer.tokenize()?;
        
        // Step 2: Parsing
        let mut parser = parser_simple::Parser::new(tokens, source.to_string());
        let ast = parser.parse()?;
        
        // Step 3: Code generation
        let mut html_generator = generator::HtmlGenerator::new();
        let html = html_generator.generate_enum(&ast)?;
        
        let mut css_generator = generator::CssGenerator::new();
        let css = css_generator.generate_enum(&ast)?;
        
        let mut js_generator = generator::JavaScriptGenerator::new();
        let js = js_generator.generate_enum(&ast)?;
        
        Ok((html, css, js))
    }
    
    /// Set configuration
    pub fn set_configuration(&mut self, config: Configuration) {
        self.configuration = Some(config);
    }
}

impl Default for ChtlCompiler {
    fn default() -> Self {
        Self::new()
    }
}