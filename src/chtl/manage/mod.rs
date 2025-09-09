//! CHTL Manager
//! 
//! This module provides high-level management of CHTL compilation.

use anyhow::Result;
use crate::chtl::node::*;
use crate::chtl::lexer::ChtlLexer;
use crate::chtl::parser::ChtlParser;
use crate::chtl::generator::ChtlGenerator;
use crate::chtl::loader::ChtlLoader;

/// CHTL Manager for high-level operations
pub struct ChtlManager {
    lexer: ChtlLexer,
    parser: ChtlParser,
    generator: ChtlGenerator,
    loader: ChtlLoader,
}

impl ChtlManager {
    /// Create a new CHTL manager
    pub fn new() -> Self {
        Self {
            lexer: ChtlLexer::new(),
            parser: ChtlParser::new(),
            generator: ChtlGenerator::new(),
            loader: ChtlLoader::new(),
        }
    }
    
    /// Compile a CHTL project
    pub fn compile_project(&mut self, source_dir: &str, output_dir: &str) -> Result<()> {
        // TODO: Implement project compilation
        // This would involve:
        // 1. Finding all .chtl files in source directory
        // 2. Compiling each file
        // 3. Resolving imports and dependencies
        // 4. Generating final output files
        
        Ok(())
    }
    
    /// Watch for file changes and recompile
    pub fn watch(&mut self, source_dir: &str, output_dir: &str) -> Result<()> {
        // TODO: Implement file watching
        // This would involve:
        // 1. Setting up file system watchers
        // 2. Detecting changes to .chtl files
        // 3. Recompiling changed files
        // 4. Updating output files
        
        Ok(())
    }
    
    /// Build a single CHTL file
    pub fn build_file(&mut self, input_file: &str, output_file: Option<&str>) -> Result<String> {
        // Load the file
        let content = self.loader.load_file(input_file)?;
        
        // Tokenize
        let tokens = self.lexer.tokenize(&content)?;
        
        // Parse
        let ast = self.parser.parse(tokens)?;
        
        // Generate
        let html = self.generator.generate(ast.nodes)?;
        
        // Write output if specified
        if let Some(output_path) = output_file {
            std::fs::write(output_path, &html)?;
        }
        
        Ok(html)
    }
}