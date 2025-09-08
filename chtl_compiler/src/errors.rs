use std::path::PathBuf;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum ChtlError {
    #[error("I/O Error in '{path}': {source}")]
    IoError {
        path: PathBuf,
        #[source]
        source: std::io::Error,
    },

    #[error("Parsing Error in '{path}':\n{message}")]
    ParseError {
        path: PathBuf,
        message: String,
    },

    #[error("Circular dependency detected: '{path}' was already imported.")]
    CircularDependency {
        path: PathBuf,
    },

    #[error("Compilation failed: {0}")]
    Generic(String),
}
