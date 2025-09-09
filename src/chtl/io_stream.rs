//! CHTL I/O stream utilities
//! 
//! This module provides I/O stream functionality for CHTL,
//! including file reading, writing, and stream processing.

use std::io::{Read, Write, BufRead, BufReader, BufWriter};
use std::fs::{File, OpenOptions};
use std::path::{Path, PathBuf};
use std::collections::VecDeque;

/// CHTL I/O stream
#[derive(Debug)]
pub struct CHTLIOStream {
    /// Input stream
    input: Option<Box<dyn Read>>,
    /// Output stream
    output: Option<Box<dyn Write>>,
    /// Error stream
    error: Option<Box<dyn Write>>,
    /// Buffer for input
    input_buffer: VecDeque<u8>,
    /// Buffer for output
    output_buffer: Vec<u8>,
    /// Buffer for error
    error_buffer: Vec<u8>,
    /// Current position
    position: usize,
    /// Line number
    line: usize,
    /// Column number
    column: usize,
}

/// I/O stream error
#[derive(Debug, Clone)]
pub struct IOStreamError {
    pub message: String,
    pub suggestion: Option<String>,
}

impl CHTLIOStream {
    /// Create a new CHTL I/O stream
    pub fn new() -> Self {
        Self {
            input: None,
            output: None,
            error: None,
            input_buffer: VecDeque::new(),
            output_buffer: Vec::new(),
            error_buffer: Vec::new(),
            position: 0,
            line: 1,
            column: 1,
        }
    }
    
    /// Create a new I/O stream with input
    pub fn with_input(input: Box<dyn Read>) -> Self {
        let mut stream = Self::new();
        stream.input = Some(input);
        stream
    }
    
    /// Create a new I/O stream with output
    pub fn with_output(output: Box<dyn Write>) -> Self {
        let mut stream = Self::new();
        stream.output = Some(output);
        stream
    }
    
    /// Create a new I/O stream with input and output
    pub fn with_input_output(input: Box<dyn Read>, output: Box<dyn Write>) -> Self {
        let mut stream = Self::new();
        stream.input = Some(input);
        stream.output = Some(output);
        stream
    }
    
    /// Create a new I/O stream with all streams
    pub fn with_all(input: Box<dyn Read>, output: Box<dyn Write>, error: Box<dyn Write>) -> Self {
        let mut stream = Self::new();
        stream.input = Some(input);
        stream.output = Some(output);
        stream.error = Some(error);
        stream
    }
    
    /// Set input stream
    pub fn set_input(&mut self, input: Box<dyn Read>) {
        self.input = Some(input);
    }
    
    /// Set output stream
    pub fn set_output(&mut self, output: Box<dyn Write>) {
        self.output = Some(output);
    }
    
    /// Set error stream
    pub fn set_error(&mut self, error: Box<dyn Write>) {
        self.error = Some(error);
    }
    
    /// Read a single character
    pub fn read_char(&mut self) -> Result<Option<char>, IOStreamError> {
        if let Some(input) = &mut self.input {
            let mut buffer = [0; 1];
            match input.read(&mut buffer) {
                Ok(0) => Ok(None), // EOF
                Ok(1) => {
                    let ch = buffer[0] as char;
                    self.position += 1;
                    if ch == '\n' {
                        self.line += 1;
                        self.column = 1;
                    } else {
                        self.column += 1;
                    }
                    Ok(Some(ch))
                }
                Ok(_) => unreachable!(),
                Err(e) => Err(IOStreamError {
                    message: format!("Failed to read from input: {}", e),
                    suggestion: Some("Check input stream".to_string()),
                }),
            }
        } else {
            Err(IOStreamError {
                message: "No input stream available".to_string(),
                suggestion: Some("Set input stream first".to_string()),
            })
        }
    }
    
    /// Read a line
    pub fn read_line(&mut self) -> Result<Option<String>, IOStreamError> {
        if let Some(input) = &mut self.input {
            let mut reader = BufReader::new(input);
            let mut line = String::new();
            match reader.read_line(&mut line) {
                Ok(0) => Ok(None), // EOF
                Ok(_) => {
                    self.position += line.len();
                    self.line += 1;
                    self.column = 1;
                    Ok(Some(line))
                }
                Err(e) => Err(IOStreamError {
                    message: format!("Failed to read line: {}", e),
                    suggestion: Some("Check input stream".to_string()),
                }),
            }
        } else {
            Err(IOStreamError {
                message: "No input stream available".to_string(),
                suggestion: Some("Set input stream first".to_string()),
            })
        }
    }
    
    /// Read all content
    pub fn read_all(&mut self) -> Result<String, IOStreamError> {
        if let Some(input) = &mut self.input {
            let mut content = String::new();
            match input.read_to_string(&mut content) {
                Ok(_) => {
                    self.position += content.len();
                    // Count lines and columns
                    for ch in content.chars() {
                        if ch == '\n' {
                            self.line += 1;
                            self.column = 1;
                        } else {
                            self.column += 1;
                        }
                    }
                    Ok(content)
                }
                Err(e) => Err(IOStreamError {
                    message: format!("Failed to read all content: {}", e),
                    suggestion: Some("Check input stream".to_string()),
                }),
            }
        } else {
            Err(IOStreamError {
                message: "No input stream available".to_string(),
                suggestion: Some("Set input stream first".to_string()),
            })
        }
    }
    
    /// Write a string
    pub fn write(&mut self, content: &str) -> Result<(), IOStreamError> {
        if let Some(output) = &mut self.output {
            match output.write_all(content.as_bytes()) {
                Ok(_) => {
                    self.output_buffer.extend_from_slice(content.as_bytes());
                    Ok(())
                }
                Err(e) => Err(IOStreamError {
                    message: format!("Failed to write content: {}", e),
                    suggestion: Some("Check output stream".to_string()),
                }),
            }
        } else {
            Err(IOStreamError {
                message: "No output stream available".to_string(),
                suggestion: Some("Set output stream first".to_string()),
            })
        }
    }
    
    /// Write a line
    pub fn write_line(&mut self, content: &str) -> Result<(), IOStreamError> {
        self.write(&format!("{}\n", content))
    }
    
    /// Write error message
    pub fn write_error(&mut self, message: &str) -> Result<(), IOStreamError> {
        if let Some(error) = &mut self.error {
            match error.write_all(message.as_bytes()) {
                Ok(_) => {
                    self.error_buffer.extend_from_slice(message.as_bytes());
                    Ok(())
                }
                Err(e) => Err(IOStreamError {
                    message: format!("Failed to write error: {}", e),
                    suggestion: Some("Check error stream".to_string()),
                }),
            }
        } else {
            // Fallback to output stream
            self.write(&format!("ERROR: {}\n", message))
        }
    }
    
    /// Flush output
    pub fn flush(&mut self) -> Result<(), IOStreamError> {
        if let Some(output) = &mut self.output {
            output.flush().map_err(|e| IOStreamError {
                message: format!("Failed to flush output: {}", e),
                suggestion: Some("Check output stream".to_string()),
            })
        } else {
            Ok(())
        }
    }
    
    /// Flush error
    pub fn flush_error(&mut self) -> Result<(), IOStreamError> {
        if let Some(error) = &mut self.error {
            error.flush().map_err(|e| IOStreamError {
                message: format!("Failed to flush error: {}", e),
                suggestion: Some("Check error stream".to_string()),
            })
        } else {
            Ok(())
        }
    }
    
    /// Get current position
    pub fn position(&self) -> usize {
        self.position
    }
    
    /// Get current line
    pub fn line(&self) -> usize {
        self.line
    }
    
    /// Get current column
    pub fn column(&self) -> usize {
        self.column
    }
    
    /// Get output buffer
    pub fn output_buffer(&self) -> &[u8] {
        &self.output_buffer
    }
    
    /// Get error buffer
    pub fn error_buffer(&self) -> &[u8] {
        &self.error_buffer
    }
    
    /// Clear output buffer
    pub fn clear_output_buffer(&mut self) {
        self.output_buffer.clear();
    }
    
    /// Clear error buffer
    pub fn clear_error_buffer(&mut self) {
        self.error_buffer.clear();
    }
    
    /// Clear all buffers
    pub fn clear_buffers(&mut self) {
        self.output_buffer.clear();
        self.error_buffer.clear();
        self.input_buffer.clear();
    }
    
    /// Reset position
    pub fn reset_position(&mut self) {
        self.position = 0;
        self.line = 1;
        self.column = 1;
    }
}

impl Default for CHTLIOStream {
    fn default() -> Self {
        Self::new()
    }
}

/// File I/O stream
#[derive(Debug)]
pub struct FileIOStream {
    /// Input file
    input_file: Option<File>,
    /// Output file
    output_file: Option<File>,
    /// Error file
    error_file: Option<File>,
    /// I/O stream
    stream: CHTLIOStream,
}

impl FileIOStream {
    /// Create a new file I/O stream
    pub fn new() -> Self {
        Self {
            input_file: None,
            output_file: None,
            error_file: None,
            stream: CHTLIOStream::new(),
        }
    }
    
    /// Open input file
    pub fn open_input(&mut self, path: &Path) -> Result<(), IOStreamError> {
        let file = File::open(path).map_err(|e| IOStreamError {
            message: format!("Failed to open input file {}: {}", path.display(), e),
            suggestion: Some("Check file path and permissions".to_string()),
        })?;
        
        self.input_file = Some(file);
        self.stream.set_input(Box::new(self.input_file.as_ref().unwrap()));
        Ok(())
    }
    
    /// Open output file
    pub fn open_output(&mut self, path: &Path) -> Result<(), IOStreamError> {
        let file = OpenOptions::new()
            .create(true)
            .write(true)
            .truncate(true)
            .open(path)
            .map_err(|e| IOStreamError {
                message: format!("Failed to open output file {}: {}", path.display(), e),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.output_file = Some(file);
        self.stream.set_output(Box::new(self.output_file.as_ref().unwrap()));
        Ok(())
    }
    
    /// Open error file
    pub fn open_error(&mut self, path: &Path) -> Result<(), IOStreamError> {
        let file = OpenOptions::new()
            .create(true)
            .write(true)
            .truncate(true)
            .open(path)
            .map_err(|e| IOStreamError {
                message: format!("Failed to open error file {}: {}", path.display(), e),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.error_file = Some(file);
        self.stream.set_error(Box::new(self.error_file.as_ref().unwrap()));
        Ok(())
    }
    
    /// Get the I/O stream
    pub fn stream(&mut self) -> &mut CHTLIOStream {
        &mut self.stream
    }
    
    /// Close all files
    pub fn close(&mut self) -> Result<(), IOStreamError> {
        if let Some(mut file) = self.input_file.take() {
            file.flush().map_err(|e| IOStreamError {
                message: format!("Failed to flush input file: {}", e),
                suggestion: None,
            })?;
        }
        
        if let Some(mut file) = self.output_file.take() {
            file.flush().map_err(|e| IOStreamError {
                message: format!("Failed to flush output file: {}", e),
                suggestion: None,
            })?;
        }
        
        if let Some(mut file) = self.error_file.take() {
            file.flush().map_err(|e| IOStreamError {
                message: format!("Failed to flush error file: {}", e),
                suggestion: None,
            })?;
        }
        
        Ok(())
    }
}

impl Default for FileIOStream {
    fn default() -> Self {
        Self::new()
    }
}

/// Memory I/O stream
#[derive(Debug)]
pub struct MemoryIOStream {
    /// Input data
    input_data: Vec<u8>,
    /// Output data
    output_data: Vec<u8>,
    /// Error data
    error_data: Vec<u8>,
    /// I/O stream
    stream: CHTLIOStream,
}

impl MemoryIOStream {
    /// Create a new memory I/O stream
    pub fn new() -> Self {
        Self {
            input_data: Vec::new(),
            output_data: Vec::new(),
            error_data: Vec::new(),
            stream: CHTLIOStream::new(),
        }
    }
    
    /// Create a new memory I/O stream with input data
    pub fn with_input(input_data: Vec<u8>) -> Self {
        let mut stream = Self::new();
        stream.input_data = input_data;
        stream.stream.set_input(Box::new(std::io::Cursor::new(&stream.input_data)));
        stream
    }
    
    /// Set input data
    pub fn set_input_data(&mut self, data: Vec<u8>) {
        self.input_data = data;
        self.stream.set_input(Box::new(std::io::Cursor::new(&self.input_data)));
    }
    
    /// Get output data
    pub fn get_output_data(&self) -> &[u8] {
        &self.output_data
    }
    
    /// Get error data
    pub fn get_error_data(&self) -> &[u8] {
        &self.error_data
    }
    
    /// Get the I/O stream
    pub fn stream(&mut self) -> &mut CHTLIOStream {
        &mut self.stream
    }
    
    /// Clear all data
    pub fn clear(&mut self) {
        self.input_data.clear();
        self.output_data.clear();
        self.error_data.clear();
        self.stream.clear_buffers();
    }
}

impl Default for MemoryIOStream {
    fn default() -> Self {
        Self::new()
    }
}