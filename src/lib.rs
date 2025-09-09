//! CHTL - A hypertext language for more developer-friendly HTML coding
//! 
//! CHTL is a language that provides a more developer-friendly way to write HTML code.
//! It supports templates, custom elements, style blocks, and much more.

pub mod chtl;
// pub mod chtl_js;
// pub mod css;
// pub mod js;
// pub mod scanner;
// pub mod code_merger;
// pub mod compiler_dispatcher;
// pub mod util;
// pub mod test;

/// Main CHTL compiler entry point
pub struct CHTLCompiler {
    // TODO: Add compiler state and configuration
}

impl CHTLCompiler {
    /// Create a new CHTL compiler instance
    pub fn new() -> Self {
        Self {}
    }
    
    /// Compile CHTL source code to HTML, CSS, and JavaScript
    pub fn compile(&self, _source: &str) -> Result<CompilationResult, CompilationError> {
        // TODO: Implement compilation pipeline
        Ok(CompilationResult {
            html: String::new(),
            css: String::new(),
            javascript: String::new(),
        })
    }
}

/// Result of CHTL compilation
#[derive(Debug, Clone)]
pub struct CompilationResult {
    pub html: String,
    pub css: String,
    pub javascript: String,
}

/// CHTL compilation error
#[derive(Debug, thiserror::Error)]
pub enum CompilationError {
    #[error("Lexical error: {0}")]
    Lexical(String),
    
    #[error("Parse error: {0}")]
    Parse(String),
    
    #[error("Semantic error: {0}")]
    Semantic(String),
    
    #[error("Generation error: {0}")]
    Generation(String),
    
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
}

impl Default for CHTLCompiler {
    fn default() -> Self {
        Self::new()
    }
}