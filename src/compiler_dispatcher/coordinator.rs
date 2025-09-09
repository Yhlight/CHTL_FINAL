//! Compilation coordinator
//! 
//! This module provides the compilation coordinator functionality for CHTL,
//! including coordinating multiple compilers and managing compilation resources.

use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use std::thread;
use std::path::{Path, PathBuf};

/// Compilation coordinator
#[derive(Debug, Clone)]
pub struct CompilationCoordinator {
    /// Available compilers
    compilers: HashMap<String, Arc<Mutex<dyn Compiler + Send + Sync>>>,
    /// Compilation queue
    compilation_queue: Arc<Mutex<Vec<CompilationTask>>>,
    /// Active compilations
    active_compilations: Arc<Mutex<HashMap<String, CompilationTask>>>,
    /// Compilation results
    compilation_results: Arc<Mutex<HashMap<String, CompilationResult>>>,
    /// Coordinator options
    options: CoordinatorOptions,
    /// Coordinator state
    state: CoordinatorState,
}

/// Compiler trait
pub trait Compiler {
    /// Compile source code
    fn compile(&mut self, source: &str, options: &CompilationOptions) -> Result<CompilationResult, CompilationError>;
    
    /// Get compiler name
    fn get_name(&self) -> String;
    
    /// Get compiler version
    fn get_version(&self) -> String;
    
    /// Get supported file types
    fn get_supported_file_types(&self) -> Vec<String>;
    
    /// Check if compiler supports file type
    fn supports_file_type(&self, file_type: &str) -> bool;
}

/// Compilation task
#[derive(Debug, Clone)]
pub struct CompilationTask {
    /// Task ID
    pub id: String,
    /// Source code
    pub source: String,
    /// File path
    pub file_path: PathBuf,
    /// File type
    pub file_type: String,
    /// Compilation options
    pub options: CompilationOptions,
    /// Task priority
    pub priority: TaskPriority,
    /// Task status
    pub status: TaskStatus,
    /// Task metadata
    pub metadata: HashMap<String, String>,
}

/// Task priority
#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum TaskPriority {
    Low,
    Normal,
    High,
    Critical,
}

/// Task status
#[derive(Debug, Clone, PartialEq)]
pub enum TaskStatus {
    Pending,
    Running,
    Completed,
    Failed,
    Cancelled,
}

/// Compilation options
#[derive(Debug, Clone)]
pub struct CompilationOptions {
    /// Output format
    pub output_format: OutputFormat,
    /// Optimization level
    pub optimization_level: OptimizationLevel,
    /// Enable debugging
    pub debug: bool,
    /// Enable warnings
    pub warnings: bool,
    /// Custom options
    pub custom_options: HashMap<String, String>,
}

/// Output format
#[derive(Debug, Clone, PartialEq)]
pub enum OutputFormat {
    HTML,
    CSS,
    JavaScript,
    All,
    Custom(String),
}

/// Optimization level
#[derive(Debug, Clone, PartialEq)]
pub enum OptimizationLevel {
    None,
    Low,
    Medium,
    High,
    Maximum,
}

/// Compilation result
#[derive(Debug, Clone)]
pub struct CompilationResult {
    /// Generated HTML
    pub html: String,
    /// Generated CSS
    pub css: String,
    /// Generated JavaScript
    pub javascript: String,
    /// Compilation metadata
    pub metadata: HashMap<String, String>,
    /// Compilation statistics
    pub statistics: CompilationStatistics,
}

/// Compilation statistics
#[derive(Debug, Clone)]
pub struct CompilationStatistics {
    /// Compilation time
    pub compilation_time: std::time::Duration,
    /// Memory usage
    pub memory_usage: usize,
    /// Error count
    pub error_count: usize,
    /// Warning count
    pub warning_count: usize,
    /// File size
    pub file_size: usize,
    /// Output size
    pub output_size: usize,
}

/// Compilation error
#[derive(Debug, Clone)]
pub struct CompilationError {
    pub message: String,
    pub file_path: Option<PathBuf>,
    pub line: Option<usize>,
    pub column: Option<usize>,
    pub suggestion: Option<String>,
}

/// Coordinator options
#[derive(Debug, Clone)]
pub struct CoordinatorOptions {
    /// Max concurrent compilations
    pub max_concurrent: usize,
    /// Task timeout
    pub task_timeout: Option<std::time::Duration>,
    /// Enable caching
    pub caching: bool,
    /// Cache size
    pub cache_size: usize,
    /// Enable load balancing
    pub load_balancing: bool,
    /// Enable failover
    pub failover: bool,
}

/// Coordinator state
#[derive(Debug, Clone, PartialEq)]
pub enum CoordinatorState {
    Stopped,
    Running,
    Paused,
    Stopping,
}

impl CompilationCoordinator {
    /// Create a new compilation coordinator
    pub fn new() -> Self {
        Self {
            compilers: HashMap::new(),
            compilation_queue: Arc::new(Mutex::new(Vec::new())),
            active_compilations: Arc::new(Mutex::new(HashMap::new())),
            compilation_results: Arc::new(Mutex::new(HashMap::new())),
            options: CoordinatorOptions::default(),
            state: CoordinatorState::Stopped,
        }
    }
    
    /// Create a new compilation coordinator with options
    pub fn with_options(options: CoordinatorOptions) -> Self {
        Self {
            compilers: HashMap::new(),
            compilation_queue: Arc::new(Mutex::new(Vec::new())),
            active_compilations: Arc::new(Mutex::new(HashMap::new())),
            compilation_results: Arc::new(Mutex::new(HashMap::new())),
            options,
            state: CoordinatorState::Stopped,
        }
    }
    
    /// Register a compiler
    pub fn register_compiler(&mut self, name: String, compiler: Arc<Mutex<dyn Compiler + Send + Sync>>) {
        self.compilers.insert(name, compiler);
    }
    
    /// Unregister a compiler
    pub fn unregister_compiler(&mut self, name: &str) {
        self.compilers.remove(name);
    }
    
    /// Get compiler by name
    pub fn get_compiler(&self, name: &str) -> Option<&Arc<Mutex<dyn Compiler + Send + Sync>>> {
        self.compilers.get(name)
    }
    
    /// Get all compilers
    pub fn get_compilers(&self) -> &HashMap<String, Arc<Mutex<dyn Compiler + Send + Sync>>> {
        &self.compilers
    }
    
    /// Submit a compilation task
    pub fn submit_task(&mut self, task: CompilationTask) -> Result<String, CoordinatorError> {
        if self.state != CoordinatorState::Running {
            return Err(CoordinatorError {
                message: "Coordinator is not running".to_string(),
                suggestion: Some("Start coordinator first".to_string()),
            });
        }
        
        let task_id = task.id.clone();
        
        // Add task to queue
        {
            let mut queue = self.compilation_queue.lock().unwrap();
            queue.push(task);
            queue.sort_by_key(|t| t.priority);
        }
        
        Ok(task_id)
    }
    
    /// Get task status
    pub fn get_task_status(&self, task_id: &str) -> Option<TaskStatus> {
        // Check active compilations
        if let Ok(active) = self.active_compilations.lock() {
            if let Some(task) = active.get(task_id) {
                return Some(task.status.clone());
            }
        }
        
        // Check results
        if let Ok(results) = self.compilation_results.lock() {
            if results.contains_key(task_id) {
                return Some(TaskStatus::Completed);
            }
        }
        
        None
    }
    
    /// Get task result
    pub fn get_task_result(&self, task_id: &str) -> Option<CompilationResult> {
        if let Ok(results) = self.compilation_results.lock() {
            results.get(task_id).cloned()
        } else {
            None
        }
    }
    
    /// Cancel a task
    pub fn cancel_task(&mut self, task_id: &str) -> Result<(), CoordinatorError> {
        // Remove from queue
        {
            let mut queue = self.compilation_queue.lock().unwrap();
            queue.retain(|t| t.id != task_id);
        }
        
        // Update active task status
        {
            let mut active = self.active_compilations.lock().unwrap();
            if let Some(task) = active.get_mut(task_id) {
                task.status = TaskStatus::Cancelled;
            }
        }
        
        Ok(())
    }
    
    /// Start the coordinator
    pub fn start(&mut self) -> Result<(), CoordinatorError> {
        if self.state != CoordinatorState::Stopped {
            return Err(CoordinatorError {
                message: "Coordinator is already running".to_string(),
                suggestion: None,
            });
        }
        
        self.state = CoordinatorState::Running;
        
        // Start worker threads
        for i in 0..self.options.max_concurrent {
            let queue = Arc::clone(&self.compilation_queue);
            let active = Arc::clone(&self.active_compilations);
            let results = Arc::clone(&self.compilation_results);
            let compilers = self.compilers.clone();
            let options = self.options.clone();
            
            thread::spawn(move || {
                Self::worker_thread(i, queue, active, results, compilers, options);
            });
        }
        
        Ok(())
    }
    
    /// Stop the coordinator
    pub fn stop(&mut self) -> Result<(), CoordinatorError> {
        if self.state != CoordinatorState::Running {
            return Err(CoordinatorError {
                message: "Coordinator is not running".to_string(),
                suggestion: None,
            });
        }
        
        self.state = CoordinatorState::Stopping;
        
        // Wait for all tasks to complete
        while {
            let active = self.active_compilations.lock().unwrap();
            !active.is_empty()
        } {
            thread::sleep(std::time::Duration::from_millis(100));
        }
        
        self.state = CoordinatorState::Stopped;
        Ok(())
    }
    
    /// Pause the coordinator
    pub fn pause(&mut self) {
        if self.state == CoordinatorState::Running {
            self.state = CoordinatorState::Paused;
        }
    }
    
    /// Resume the coordinator
    pub fn resume(&mut self) {
        if self.state == CoordinatorState::Paused {
            self.state = CoordinatorState::Running;
        }
    }
    
    /// Worker thread function
    fn worker_thread(
        worker_id: usize,
        queue: Arc<Mutex<Vec<CompilationTask>>>,
        active: Arc<Mutex<HashMap<String, CompilationTask>>>,
        results: Arc<Mutex<HashMap<String, CompilationResult>>>,
        compilers: HashMap<String, Arc<Mutex<dyn Compiler + Send + Sync>>>,
        options: CoordinatorOptions,
    ) {
        loop {
            // Get next task
            let task = {
                let mut queue = queue.lock().unwrap();
                queue.pop()
            };
            
            if let Some(mut task) = task {
                // Update task status
                task.status = TaskStatus::Running;
                {
                    let mut active = active.lock().unwrap();
                    active.insert(task.id.clone(), task.clone());
                }
                
                // Find appropriate compiler
                let compiler = compilers.values().find(|c| {
                    if let Ok(compiler) = c.lock() {
                        compiler.supports_file_type(&task.file_type)
                    } else {
                        false
                    }
                });
                
                if let Some(compiler) = compiler {
                    // Compile
                    let result = {
                        let mut compiler = compiler.lock().unwrap();
                        compiler.compile(&task.source, &task.options)
                    };
                    
                    // Update results
                    match result {
                        Ok(compilation_result) => {
                            let mut results = results.lock().unwrap();
                            results.insert(task.id.clone(), compilation_result);
                        }
                        Err(e) => {
                            eprintln!("Compilation failed for task {}: {}", task.id, e.message);
                        }
                    }
                } else {
                    eprintln!("No compiler found for file type: {}", task.file_type);
                }
                
                // Remove from active
                {
                    let mut active = active.lock().unwrap();
                    active.remove(&task.id);
                }
            } else {
                // No tasks available, sleep
                thread::sleep(std::time::Duration::from_millis(100));
            }
        }
    }
    
    /// Get coordinator state
    pub fn get_state(&self) -> &CoordinatorState {
        &self.state
    }
    
    /// Get coordinator options
    pub fn get_options(&self) -> &CoordinatorOptions {
        &self.options
    }
    
    /// Set coordinator options
    pub fn set_options(&mut self, options: CoordinatorOptions) {
        self.options = options;
    }
    
    /// Get compilation statistics
    pub fn get_statistics(&self) -> CoordinatorStatistics {
        let queue_size = {
            let queue = self.compilation_queue.lock().unwrap();
            queue.len()
        };
        
        let active_count = {
            let active = self.active_compilations.lock().unwrap();
            active.len()
        };
        
        let completed_count = {
            let results = self.compilation_results.lock().unwrap();
            results.len()
        };
        
        CoordinatorStatistics {
            queue_size,
            active_count,
            completed_count,
            total_compilers: self.compilers.len(),
            state: self.state.clone(),
        }
    }
}

/// Coordinator error
#[derive(Debug, Clone)]
pub struct CoordinatorError {
    pub message: String,
    pub suggestion: Option<String>,
}

/// Coordinator statistics
#[derive(Debug, Clone)]
pub struct CoordinatorStatistics {
    pub queue_size: usize,
    pub active_count: usize,
    pub completed_count: usize,
    pub total_compilers: usize,
    pub state: CoordinatorState,
}

impl Default for CompilationCoordinator {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CoordinatorOptions {
    fn default() -> Self {
        Self {
            max_concurrent: 4,
            task_timeout: None,
            caching: true,
            cache_size: 1000,
            load_balancing: true,
            failover: true,
        }
    }
}

impl Default for CompilationOptions {
    fn default() -> Self {
        Self {
            output_format: OutputFormat::All,
            optimization_level: OptimizationLevel::Medium,
            debug: false,
            warnings: true,
            custom_options: HashMap::new(),
        }
    }
}