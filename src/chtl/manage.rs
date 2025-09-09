//! CHTL compilation manager
//! 
//! This module provides high-level management functionality for CHTL compilation,
//! including compilation orchestration, error handling, and result management.

use std::collections::HashMap;
use std::path::{Path, PathBuf};
use crate::chtl::context::*;
use crate::chtl::lexer::*;
use crate::chtl::parser::*;
use crate::chtl::generator::*;
use crate::chtl::loader::*;
use crate::chtl::state::*;

/// CHTL compilation manager
#[derive(Debug, Clone)]
pub struct CHTLManager {
    /// Compilation context
    context: CHTLContext,
    /// State manager
    state: CHTLState,
    /// File loader
    loader: CHTLLoader,
    /// Compilation results
    results: HashMap<PathBuf, CompilationResult>,
    /// Compilation statistics
    statistics: CompilationStatistics,
}

/// Compilation statistics
#[derive(Debug, Clone, Default)]
pub struct CompilationStatistics {
    /// Total files processed
    pub files_processed: usize,
    /// Total compilation time
    pub total_time: std::time::Duration,
    /// Lexical analysis time
    pub lexing_time: std::time::Duration,
    /// Parsing time
    pub parsing_time: std::time::Duration,
    /// Semantic analysis time
    pub semantic_time: std::time::Duration,
    /// Generation time
    pub generation_time: std::time::Duration,
    /// Total errors
    pub total_errors: usize,
    /// Total warnings
    pub total_warnings: usize,
    /// Memory usage
    pub memory_usage: usize,
}

/// Manager error
#[derive(Debug, Clone)]
pub struct ManagerError {
    pub message: String,
    pub path: Option<PathBuf>,
    pub suggestion: Option<String>,
}

impl CHTLManager {
    /// Create a new CHTL manager
    pub fn new(base_dir: PathBuf) -> Self {
        Self {
            context: CHTLContext::new(),
            state: CHTLState::new(),
            loader: CHTLLoader::new(base_dir),
            results: HashMap::new(),
            statistics: CompilationStatistics::default(),
        }
    }
    
    /// Compile a single file
    pub fn compile_file(&mut self, file_path: &Path) -> Result<CompilationResult, ManagerError> {
        let start_time = std::time::Instant::now();
        
        // Reset state
        self.state.reset();
        self.statistics = CompilationStatistics::default();
        
        // Load file
        let loaded_file = self.loader.load_file(file_path)
            .map_err(|e| ManagerError {
                message: e.message,
                path: e.path,
                suggestion: e.suggestion,
            })?;
        
        // Transition to lexing state
        self.state.transition_to(CompilationState::Lexing)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        // Lexical analysis
        let lexing_start = std::time::Instant::now();
        let mut lexer = CHTLLexer::with_context(
            loaded_file.content.clone(),
            file_path.to_string_lossy().to_string(),
            self.context.clone(),
        );
        
        let tokens = lexer.tokenize()
            .map_err(|e| ManagerError {
                message: format!("Lexical error: {}", e.message),
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        self.statistics.lexing_time = lexing_start.elapsed();
        
        // Transition to parsing state
        self.state.transition_to(CompilationState::Parsing)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        // Parsing
        let parsing_start = std::time::Instant::now();
        let mut parser = CHTLParser::new(tokens, self.context.clone());
        
        let ast = parser.parse()
            .map_err(|e| ManagerError {
                message: format!("Parse error: {}", e.message),
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        self.statistics.parsing_time = parsing_start.elapsed();
        
        // Transition to semantic analysis state
        self.state.transition_to(CompilationState::SemanticAnalysis)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        // Semantic analysis
        let semantic_start = std::time::Instant::now();
        self.perform_semantic_analysis(&ast)?;
        self.statistics.semantic_time = semantic_start.elapsed();
        
        // Transition to generation state
        self.state.transition_to(CompilationState::Generating)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        // Code generation
        let generation_start = std::time::Instant::now();
        let mut generator = CHTLGenerator::new(self.context.clone());
        
        let result = generator.generate(&ast)
            .map_err(|e| ManagerError {
                message: format!("Generation error: {}", e.message),
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        self.statistics.generation_time = generation_start.elapsed();
        
        // Transition to completed state
        self.state.transition_to(CompilationState::Completed)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(file_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        // Update statistics
        self.statistics.total_time = start_time.elapsed();
        self.statistics.files_processed = 1;
        self.statistics.total_errors = self.context.get_errors().len();
        self.statistics.total_warnings = self.context.get_warnings().len();
        
        // Store result
        self.results.insert(file_path.to_path_buf(), result.clone());
        
        Ok(result)
    }
    
    /// Compile multiple files
    pub fn compile_files(&mut self, file_paths: &[PathBuf]) -> Result<HashMap<PathBuf, CompilationResult>, ManagerError> {
        let mut results = HashMap::new();
        
        for file_path in file_paths {
            match self.compile_file(file_path) {
                Ok(result) => {
                    results.insert(file_path.clone(), result);
                }
                Err(e) => {
                    // Log error but continue with other files
                    eprintln!("Failed to compile {}: {}", file_path.display(), e.message);
                }
            }
        }
        
        Ok(results)
    }
    
    /// Compile a directory
    pub fn compile_directory(&mut self, dir_path: &Path) -> Result<HashMap<PathBuf, CompilationResult>, ManagerError> {
        let loaded_files = self.loader.load_directory(dir_path)
            .map_err(|e| ManagerError {
                message: e.message,
                path: Some(dir_path.to_path_buf()),
                suggestion: e.suggestion,
            })?;
        
        let file_paths: Vec<PathBuf> = loaded_files
            .iter()
            .map(|f| f.path.clone())
            .collect();
        
        self.compile_files(&file_paths)
    }
    
    /// Compile files matching a pattern
    pub fn compile_files_by_pattern(&mut self, pattern: &str) -> Result<HashMap<PathBuf, CompilationResult>, ManagerError> {
        let loaded_files = self.loader.load_files_by_pattern(pattern)
            .map_err(|e| ManagerError {
                message: e.message,
                path: None,
                suggestion: e.suggestion,
            })?;
        
        let file_paths: Vec<PathBuf> = loaded_files
            .iter()
            .map(|f| f.path.clone())
            .collect();
        
        self.compile_files(&file_paths)
    }
    
    /// Perform semantic analysis
    fn perform_semantic_analysis(&mut self, ast: &Node) -> Result<(), ManagerError> {
        // TODO: Implement semantic analysis
        // This would include:
        // - Symbol table construction
        // - Type checking
        // - Scope analysis
        // - Template resolution
        // - Custom definition processing
        // - Import resolution
        // - Namespace handling
        // - Constraint validation
        
        Ok(())
    }
    
    /// Get compilation result for a file
    pub fn get_result(&self, file_path: &Path) -> Option<&CompilationResult> {
        self.results.get(file_path)
    }
    
    /// Get all compilation results
    pub fn get_results(&self) -> &HashMap<PathBuf, CompilationResult> {
        &self.results
    }
    
    /// Get compilation statistics
    pub fn get_statistics(&self) -> &CompilationStatistics {
        &self.statistics
    }
    
    /// Get compilation context
    pub fn get_context(&self) -> &CHTLContext {
        &self.context
    }
    
    /// Get mutable compilation context
    pub fn get_context_mut(&mut self) -> &mut CHTLContext {
        &mut self.context
    }
    
    /// Get state manager
    pub fn get_state(&self) -> &CHTLState {
        &self.state
    }
    
    /// Get file loader
    pub fn get_loader(&self) -> &CHTLLoader {
        &self.loader
    }
    
    /// Get mutable file loader
    pub fn get_loader_mut(&mut self) -> &mut CHTLLoader {
        &mut self.loader
    }
    
    /// Clear all results
    pub fn clear_results(&mut self) {
        self.results.clear();
        self.statistics = CompilationStatistics::default();
    }
    
    /// Reset manager state
    pub fn reset(&mut self) {
        self.context = CHTLContext::new();
        self.state.reset();
        self.loader.clear_cache();
        self.clear_results();
    }
    
    /// Add search path
    pub fn add_search_path(&mut self, path: PathBuf) {
        self.loader.add_search_path(path);
    }
    
    /// Set configuration
    pub fn set_configuration(&mut self, config: Configuration) {
        self.context.config = config;
    }
    
    /// Get configuration
    pub fn get_configuration(&self) -> &Configuration {
        &self.context.config
    }
    
    /// Check if compilation is in progress
    pub fn is_compiling(&self) -> bool {
        self.state.is_in_progress()
    }
    
    /// Check if compilation is complete
    pub fn is_complete(&self) -> bool {
        self.state.is_complete()
    }
    
    /// Check if compilation failed
    pub fn has_failed(&self) -> bool {
        self.state.has_failed()
    }
    
    /// Get compilation summary
    pub fn get_summary(&self) -> CompilationSummary {
        CompilationSummary {
            files_processed: self.statistics.files_processed,
            total_time: self.statistics.total_time,
            total_errors: self.statistics.total_errors,
            total_warnings: self.statistics.total_warnings,
            is_complete: self.is_complete(),
            has_failed: self.has_failed(),
            is_in_progress: self.is_compiling(),
        }
    }
}

/// Compilation summary
#[derive(Debug, Clone)]
pub struct CompilationSummary {
    pub files_processed: usize,
    pub total_time: std::time::Duration,
    pub total_errors: usize,
    pub total_warnings: usize,
    pub is_complete: bool,
    pub has_failed: bool,
    pub is_in_progress: bool,
}

impl Default for CHTLManager {
    fn default() -> Self {
        Self::new(PathBuf::from("."))
    }
}