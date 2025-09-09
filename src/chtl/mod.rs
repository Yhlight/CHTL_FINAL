//! CHTL Core Compiler
//! 
//! This module contains the core CHTL compiler implementation.

pub mod context;
pub mod generator;
pub mod lexer;
pub mod loader;
pub mod manage;
pub mod node;
pub mod parser;
pub mod state;
pub mod io_stream;
pub mod cmod_system;
pub mod module_parser;
pub mod module_manager;

use anyhow::Result;
use std::path::Path;

/// Main CHTL Compiler
pub struct ChtlCompiler {
    context: context::ChtlContext,
    lexer: lexer::ChtlLexer,
    parser: parser::ChtlParser,
    generator: generator::ChtlGenerator,
}

impl ChtlCompiler {
    /// Create a new CHTL compiler instance
    pub fn new() -> Self {
        Self {
            context: context::ChtlContext::new(),
            lexer: lexer::ChtlLexer::new(),
            parser: parser::ChtlParser::new(),
            generator: generator::ChtlGenerator::new(),
        }
    }
    
    /// Compile a CHTL file to HTML
    pub async fn compile_file<P: AsRef<Path>>(&self, input: P, output: Option<&str>) -> Result<String> {
        let input_path = input.as_ref();
        let content = std::fs::read_to_string(input_path)?;
        
        // Tokenize the input
        let tokens = self.lexer.tokenize(&content)?;
        
        // Parse the tokens into AST
        let ast = self.parser.parse(tokens)?;
        
        // Generate HTML output
        let html = self.generator.generate(ast.nodes)?;
        
        // Write output if specified
        if let Some(output_path) = output {
            std::fs::write(output_path, &html)?;
        }
        
        Ok(html)
    }
    
    /// Compile CHTL source code to HTML
    pub fn compile_source(&self, source: &str) -> Result<String> {
        // Tokenize the input
        let tokens = self.lexer.tokenize(source)?;
        
        // Parse the tokens into AST
        let ast = self.parser.parse(tokens)?;
        
        // Generate HTML output
        let html = self.generator.generate(ast.nodes)?;
        
        Ok(html)
    }
}