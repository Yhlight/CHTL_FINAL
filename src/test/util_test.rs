//! Utility tests
//! 
//! This module provides utility test functionality for CHTL,
//! including testing utility functions and helper methods.

use std::collections::HashMap;
use crate::util::*;

/// Utility test runner
#[derive(Debug, Clone)]
pub struct UtilTestRunner {
    /// Test results
    test_results: HashMap<String, TestResult>,
    /// Test options
    options: TestOptions,
}

/// Test result
#[derive(Debug, Clone)]
pub struct TestResult {
    /// Test name
    pub name: String,
    /// Test status
    pub status: TestStatus,
    /// Test duration
    pub duration: std::time::Duration,
    /// Test message
    pub message: Option<String>,
    /// Test error
    pub error: Option<String>,
}

/// Test status
#[derive(Debug, Clone, PartialEq)]
pub enum TestStatus {
    Passed,
    Failed,
    Skipped,
    Error,
}

/// Test options
#[derive(Debug, Clone)]
pub struct TestOptions {
    /// Verbose output
    pub verbose: bool,
    /// Stop on first failure
    pub stop_on_failure: bool,
    /// Include skipped tests
    pub include_skipped: bool,
    /// Test timeout
    pub timeout: Option<std::time::Duration>,
}

impl UtilTestRunner {
    /// Create a new utility test runner
    pub fn new() -> Self {
        Self {
            test_results: HashMap::new(),
            options: TestOptions::default(),
        }
    }
    
    /// Create a new utility test runner with options
    pub fn with_options(options: TestOptions) -> Self {
        Self {
            test_results: HashMap::new(),
            options,
        }
    }
    
    /// Run all utility tests
    pub fn run_all_tests(&mut self) -> Result<(), TestError> {
        self.run_file_system_tests()?;
        self.run_string_util_tests()?;
        self.run_zip_util_tests()?;
        
        Ok(())
    }
    
    /// Run file system tests
    pub fn run_file_system_tests(&mut self) -> Result<(), TestError> {
        let test_name = "file_system_tests";
        let start_time = std::time::Instant::now();
        
        let mut fs_utils = FileSystemUtils::new();
        
        // Test file existence
        self.run_test("file_exists", || {
            let temp_file = std::env::temp_dir().join("chtl_test_file.txt");
            std::fs::write(&temp_file, "test content").unwrap();
            let exists = fs_utils.exists(&temp_file);
            std::fs::remove_file(&temp_file).unwrap();
            assert!(exists, "File should exist");
        })?;
        
        // Test file reading
        self.run_test("file_read", || {
            let temp_file = std::env::temp_dir().join("chtl_test_read.txt");
            std::fs::write(&temp_file, "test content").unwrap();
            let content = fs_utils.read_file(&temp_file).unwrap();
            std::fs::remove_file(&temp_file).unwrap();
            assert_eq!(content, "test content", "File content should match");
        })?;
        
        // Test file writing
        self.run_test("file_write", || {
            let temp_file = std::env::temp_dir().join("chtl_test_write.txt");
            fs_utils.write_file(&temp_file, "test content").unwrap();
            let content = std::fs::read_to_string(&temp_file).unwrap();
            std::fs::remove_file(&temp_file).unwrap();
            assert_eq!(content, "test content", "File content should match");
        })?;
        
        // Test directory creation
        self.run_test("directory_create", || {
            let temp_dir = std::env::temp_dir().join("chtl_test_dir");
            fs_utils.create_directory(&temp_dir).unwrap();
            assert!(fs_utils.is_directory(&temp_dir), "Directory should exist");
            std::fs::remove_dir(&temp_dir).unwrap();
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All file system tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run string utility tests
    pub fn run_string_util_tests(&mut self) -> Result<(), TestError> {
        let test_name = "string_util_tests";
        let start_time = std::time::Instant::now();
        
        let string_utils = StringUtils::new();
        
        // Test HTML escaping
        self.run_test("html_escape", || {
            let escaped = string_utils.escape_html("<div>Hello & World</div>");
            assert_eq!(escaped, "&lt;div&gt;Hello &amp; World&lt;/div&gt;", "HTML should be escaped");
        })?;
        
        // Test HTML unescaping
        self.run_test("html_unescape", || {
            let unescaped = string_utils.unescape_html("&lt;div&gt;Hello &amp; World&lt;/div&gt;");
            assert_eq!(unescaped, "<div>Hello & World</div>", "HTML should be unescaped");
        })?;
        
        // Test case conversion
        self.run_test("case_conversion", || {
            let camel_case = string_utils.to_camel_case("hello world");
            assert_eq!(camel_case, "helloWorld", "Should convert to camel case");
            
            let snake_case = string_utils.to_snake_case("Hello World");
            assert_eq!(snake_case, "hello_world", "Should convert to snake case");
            
            let kebab_case = string_utils.to_kebab_case("Hello World");
            assert_eq!(kebab_case, "hello-world", "Should convert to kebab case");
        })?;
        
        // Test pattern matching
        self.run_test("pattern_matching", || {
            let html_tag = "<div>Hello</div>";
            assert!(string_utils.matches_pattern(html_tag, "html_tag"), "Should match HTML tag pattern");
            
            let not_html_tag = "Hello World";
            assert!(!string_utils.matches_pattern(not_html_tag, "html_tag"), "Should not match HTML tag pattern");
        })?;
        
        // Test string validation
        self.run_test("string_validation", || {
            assert!(string_utils.is_numeric("123.45"), "Should be numeric");
            assert!(!string_utils.is_numeric("abc"), "Should not be numeric");
            
            assert!(string_utils.is_integer("123"), "Should be integer");
            assert!(!string_utils.is_integer("123.45"), "Should not be integer");
            
            assert!(string_utils.is_boolean("true"), "Should be boolean");
            assert!(string_utils.is_boolean("false"), "Should be boolean");
            assert!(!string_utils.is_boolean("maybe"), "Should not be boolean");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All string utility tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run zip utility tests
    pub fn run_zip_util_tests(&mut self) -> Result<(), TestError> {
        let test_name = "zip_util_tests";
        let start_time = std::time::Instant::now();
        
        let zip_utils = ZipUtils::new();
        
        // Test archive creation
        self.run_test("archive_creation", || {
            let temp_dir = std::env::temp_dir().join("chtl_test_zip");
            std::fs::create_dir_all(&temp_dir).unwrap();
            
            let file1 = temp_dir.join("file1.txt");
            let file2 = temp_dir.join("file2.txt");
            std::fs::write(&file1, "content1").unwrap();
            std::fs::write(&file2, "content2").unwrap();
            
            let archive_path = std::env::temp_dir().join("chtl_test_archive.zip");
            zip_utils.create_archive(&archive_path, &[file1, file2]).unwrap();
            
            assert!(zip_utils.is_valid_archive(&archive_path), "Archive should be valid");
            
            std::fs::remove_file(&archive_path).unwrap();
            std::fs::remove_dir_all(&temp_dir).unwrap();
        })?;
        
        // Test archive extraction
        self.run_test("archive_extraction", || {
            let temp_dir = std::env::temp_dir().join("chtl_test_zip");
            std::fs::create_dir_all(&temp_dir).unwrap();
            
            let file1 = temp_dir.join("file1.txt");
            let file2 = temp_dir.join("file2.txt");
            std::fs::write(&file1, "content1").unwrap();
            std::fs::write(&file2, "content2").unwrap();
            
            let archive_path = std::env::temp_dir().join("chtl_test_archive.zip");
            zip_utils.create_archive(&archive_path, &[file1, file2]).unwrap();
            
            let extract_dir = std::env::temp_dir().join("chtl_test_extract");
            std::fs::create_dir_all(&extract_dir).unwrap();
            
            zip_utils.extract_archive(&archive_path, &extract_dir).unwrap();
            
            let extracted_file1 = extract_dir.join("file1.txt");
            let extracted_file2 = extract_dir.join("file2.txt");
            assert!(extracted_file1.exists(), "Extracted file1 should exist");
            assert!(extracted_file2.exists(), "Extracted file2 should exist");
            
            let content1 = std::fs::read_to_string(&extracted_file1).unwrap();
            let content2 = std::fs::read_to_string(&extracted_file2).unwrap();
            assert_eq!(content1, "content1", "Extracted file1 content should match");
            assert_eq!(content2, "content2", "Extracted file2 content should match");
            
            std::fs::remove_file(&archive_path).unwrap();
            std::fs::remove_dir_all(&temp_dir).unwrap();
            std::fs::remove_dir_all(&extract_dir).unwrap();
        })?;
        
        // Test archive listing
        self.run_test("archive_listing", || {
            let temp_dir = std::env::temp_dir().join("chtl_test_zip");
            std::fs::create_dir_all(&temp_dir).unwrap();
            
            let file1 = temp_dir.join("file1.txt");
            let file2 = temp_dir.join("file2.txt");
            std::fs::write(&file1, "content1").unwrap();
            std::fs::write(&file2, "content2").unwrap();
            
            let archive_path = std::env::temp_dir().join("chtl_test_archive.zip");
            zip_utils.create_archive(&archive_path, &[file1, file2]).unwrap();
            
            let contents = zip_utils.list_archive_contents(&archive_path).unwrap();
            assert_eq!(contents.len(), 2, "Archive should contain 2 files");
            assert!(contents.contains(&"file1.txt".to_string()), "Archive should contain file1.txt");
            assert!(contents.contains(&"file2.txt".to_string()), "Archive should contain file2.txt");
            
            std::fs::remove_file(&archive_path).unwrap();
            std::fs::remove_dir_all(&temp_dir).unwrap();
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All zip utility tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run a single test
    fn run_test<F>(&mut self, test_name: &str, test_func: F) -> Result<(), TestError>
    where
        F: FnOnce() -> (),
    {
        let start_time = std::time::Instant::now();
        
        match std::panic::catch_unwind(|| {
            test_func();
        }) {
            Ok(_) => {
                let duration = start_time.elapsed();
                self.test_results.insert(test_name.to_string(), TestResult {
                    name: test_name.to_string(),
                    status: TestStatus::Passed,
                    duration,
                    message: Some("Test passed".to_string()),
                    error: None,
                });
                Ok(())
            }
            Err(panic_info) => {
                let duration = start_time.elapsed();
                let error_msg = if let Some(s) = panic_info.downcast_ref::<String>() {
                    s.clone()
                } else if let Some(s) = panic_info.downcast_ref::<&str>() {
                    s.to_string()
                } else {
                    "Unknown panic".to_string()
                };
                
                self.test_results.insert(test_name.to_string(), TestResult {
                    name: test_name.to_string(),
                    status: TestStatus::Failed,
                    duration,
                    message: Some("Test failed".to_string()),
                    error: Some(error_msg),
                });
                
                if self.options.stop_on_failure {
                    return Err(TestError {
                        message: format!("Test {} failed", test_name),
                        suggestion: Some("Check test implementation".to_string()),
                    });
                }
                
                Ok(())
            }
        }
    }
    
    /// Get test results
    pub fn get_test_results(&self) -> &HashMap<String, TestResult> {
        &self.test_results
    }
    
    /// Get test result by name
    pub fn get_test_result(&self, name: &str) -> Option<&TestResult> {
        self.test_results.get(name)
    }
    
    /// Get test statistics
    pub fn get_test_statistics(&self) -> TestStatistics {
        let mut passed = 0;
        let mut failed = 0;
        let mut skipped = 0;
        let mut error = 0;
        let mut total_duration = std::time::Duration::from_secs(0);
        
        for result in self.test_results.values() {
            match result.status {
                TestStatus::Passed => passed += 1,
                TestStatus::Failed => failed += 1,
                TestStatus::Skipped => skipped += 1,
                TestStatus::Error => error += 1,
            }
            total_duration += result.duration;
        }
        
        TestStatistics {
            total_tests: self.test_results.len(),
            passed,
            failed,
            skipped,
            error,
            total_duration,
        }
    }
    
    /// Print test results
    pub fn print_test_results(&self) {
        println!("Test Results:");
        println!("=============");
        
        for (name, result) in &self.test_results {
            let status = match result.status {
                TestStatus::Passed => "PASS",
                TestStatus::Failed => "FAIL",
                TestStatus::Skipped => "SKIP",
                TestStatus::Error => "ERROR",
            };
            
            println!("{} {} ({:?})", status, name, result.duration);
            
            if let Some(message) = &result.message {
                println!("  Message: {}", message);
            }
            
            if let Some(error) = &result.error {
                println!("  Error: {}", error);
            }
        }
        
        let stats = self.get_test_statistics();
        println!("\nStatistics:");
        println!("Total: {}", stats.total_tests);
        println!("Passed: {}", stats.passed);
        println!("Failed: {}", stats.failed);
        println!("Skipped: {}", stats.skipped);
        println!("Error: {}", stats.error);
        println!("Total Duration: {:?}", stats.total_duration);
    }
}

/// Test error
#[derive(Debug, Clone)]
pub struct TestError {
    pub message: String,
    pub suggestion: Option<String>,
}

/// Test statistics
#[derive(Debug, Clone)]
pub struct TestStatistics {
    pub total_tests: usize,
    pub passed: usize,
    pub failed: usize,
    pub skipped: usize,
    pub error: usize,
    pub total_duration: std::time::Duration,
}

impl Default for UtilTestRunner {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for TestOptions {
    fn default() -> Self {
        Self {
            verbose: false,
            stop_on_failure: false,
            include_skipped: false,
            timeout: None,
        }
    }
}