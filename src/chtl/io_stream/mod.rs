//! CHTL IO Stream
//! 
//! This module handles input/output operations for CHTL.

use anyhow::Result;
use std::io::{Read, Write};
use std::fs::File;
use std::path::Path;

/// IO stream for CHTL operations
pub struct ChtlIOStream {
    /// Input source
    input: Box<dyn Read>,
    /// Output destination
    output: Box<dyn Write>,
}

impl ChtlIOStream {
    /// Create a new IO stream from files
    pub fn from_files<P: AsRef<Path>>(input_path: P, output_path: P) -> Result<Self> {
        let input_file = File::open(input_path)?;
        let output_file = File::create(output_path)?;
        
        Ok(Self {
            input: Box::new(input_file),
            output: Box::new(output_file),
        })
    }
    
    /// Create a new IO stream from strings
    pub fn from_strings(input: String, _output: &mut String) -> Self {
        Self {
            input: Box::new(std::io::Cursor::new(input.into_bytes())),
            output: Box::new(std::io::Cursor::new(Vec::new())),
        }
    }
    
    /// Read from input
    pub fn read_to_string(&mut self) -> Result<String> {
        let mut buffer = String::new();
        self.input.read_to_string(&mut buffer)?;
        Ok(buffer)
    }
    
    /// Write to output
    pub fn write_all(&mut self, data: &[u8]) -> Result<()> {
        self.output.write_all(data)?;
        Ok(())
    }
    
    /// Write string to output
    pub fn write_string(&mut self, data: &str) -> Result<()> {
        self.write_all(data.as_bytes())
    }
}