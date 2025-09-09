//! CHTL JS Compiler
//! 
//! This module contains the CHTL JS compiler implementation.

pub mod context;
pub mod generator;
pub mod lexer;
pub mod loader;
pub mod manage;
pub mod node;
pub mod parser;
pub mod state;
pub mod io_stream;
pub mod cjmod_system;
pub mod cjmod_api;

use anyhow::Result;
use std::path::Path;

/// Main CHTL JS Compiler
pub struct ChtlJSCompiler {
    context: context::ChtlJSContext,
    lexer: lexer::ChtlJSLexer,
    parser: parser::ChtlJSParser,
    generator: generator::ChtlJSGenerator,
}

impl ChtlJSCompiler {
    /// Create a new CHTL JS compiler instance
    pub fn new() -> Self {
        Self {
            context: context::ChtlJSContext::new(),
            lexer: lexer::ChtlJSLexer::new(),
            parser: parser::ChtlJSParser::new(),
            generator: generator::ChtlJSGenerator::new(),
        }
    }
    
    /// Compile CHTL JS source code to JavaScript
    pub fn compile_source(&mut self, source: &str) -> Result<String> {
        // Tokenize the input
        let tokens = self.lexer.tokenize(source)?;
        
        // Parse the tokens into AST
        let ast = self.parser.parse(tokens)?;
        
        // Generate JavaScript output
        let js = self.generator.generate(ast)?;
        
        Ok(js)
    }
}