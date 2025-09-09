//! Error types for CHTL compiler

use thiserror::Error;

/// Result type alias for CHTL operations
pub type Result<T> = std::result::Result<T, ChtlError>;

/// Main error type for CHTL compiler
#[derive(Error, Debug)]
pub enum ChtlError {
    #[error("Lexical error: {message} at position {position}")]
    Lexical { message: String, position: usize },
    
    #[error("Syntax error: {message} at line {line}, column {column}")]
    Syntax { message: String, line: usize, column: usize },
    
    #[error("Semantic error: {message}")]
    Semantic { message: String },
    
    #[error("IO error: {0}")]
    Io(#[from] std::io::Error),
    
    #[error("Parse error: {message}")]
    Parse { message: String },
    
    #[error("Generation error: {message}")]
    Generation { message: String },
    
    #[error("Module error: {message}")]
    Module { message: String },
    
    #[error("Configuration error: {message}")]
    Configuration { message: String },
    
    #[error("Internal error: {message}")]
    Internal { message: String },
}

impl ChtlError {
    /// Create a lexical error
    pub fn lexical(message: impl Into<String>, position: usize) -> Self {
        Self::Lexical {
            message: message.into(),
            position,
        }
    }
    
    /// Create a syntax error
    pub fn syntax(message: impl Into<String>, line: usize, column: usize) -> Self {
        Self::Syntax {
            message: message.into(),
            line,
            column,
        }
    }
    
    /// Create a semantic error
    pub fn semantic(message: impl Into<String>) -> Self {
        Self::Semantic {
            message: message.into(),
        }
    }
    
    /// Create a parse error
    pub fn parse(message: impl Into<String>) -> Self {
        Self::Parse {
            message: message.into(),
        }
    }
    
    /// Create a generation error
    pub fn generation(message: impl Into<String>) -> Self {
        Self::Generation {
            message: message.into(),
        }
    }
    
    /// Create a module error
    pub fn module(message: impl Into<String>) -> Self {
        Self::Module {
            message: message.into(),
        }
    }
    
    /// Create a configuration error
    pub fn configuration(message: impl Into<String>) -> Self {
        Self::Configuration {
            message: message.into(),
        }
    }
    
    /// Create an internal error
    pub fn internal(message: impl Into<String>) -> Self {
        Self::Internal {
            message: message.into(),
        }
    }
}