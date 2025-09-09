//! Test module
//! 
//! This module provides comprehensive testing functionality for CHTL,
//! including unit tests, integration tests, and test utilities.

pub mod util_test;
pub mod token_test;
pub mod ast_test;

use std::collections::HashMap;
use std::time::Duration;

/// Test runner
#[derive(Debug, Clone)]
pub struct TestRunner {
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
    pub duration: Duration,
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
    pub timeout: Option<Duration>,
    /// Test filter
    pub filter: Option<String>,
    /// Test pattern
    pub pattern: Option<String>,
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
    pub total_duration: Duration,
}

impl TestRunner {
    /// Create a new test runner
    pub fn new() -> Self {
        Self {
            test_results: HashMap::new(),
            options: TestOptions::default(),
        }
    }
    
    /// Create a new test runner with options
    pub fn with_options(options: TestOptions) -> Self {
        Self {
            test_results: HashMap::new(),
            options,
        }
    }
    
    /// Run all tests
    pub fn run_all_tests(&mut self) -> Result<(), TestError> {
        self.run_util_tests()?;
        self.run_token_tests()?;
        self.run_ast_tests()?;
        
        Ok(())
    }
    
    /// Run utility tests
    pub fn run_util_tests(&mut self) -> Result<(), TestError> {
        let mut util_runner = util_test::UtilTestRunner::with_options(self.options.clone());
        util_runner.run_all_tests()?;
        
        // Merge results
        for (name, result) in util_runner.get_test_results() {
            self.test_results.insert(format!("util_{}", name), result.clone());
        }
        
        Ok(())
    }
    
    /// Run token tests
    pub fn run_token_tests(&mut self) -> Result<(), TestError> {
        let mut token_runner = token_test::TokenTestRunner::with_options(self.options.clone());
        token_runner.run_all_tests()?;
        
        // Merge results
        for (name, result) in token_runner.get_test_results() {
            self.test_results.insert(format!("token_{}", name), result.clone());
        }
        
        Ok(())
    }
    
    /// Run AST tests
    pub fn run_ast_tests(&mut self) -> Result<(), TestError> {
        let mut ast_runner = ast_test::ASTTestRunner::with_options(self.options.clone());
        ast_runner.run_all_tests()?;
        
        // Merge results
        for (name, result) in ast_runner.get_test_results() {
            self.test_results.insert(format!("ast_{}", name), result.clone());
        }
        
        Ok(())
    }
    
    /// Run a single test
    pub fn run_test<F>(&mut self, test_name: &str, test_func: F) -> Result<(), TestError>
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
        let mut total_duration = Duration::from_secs(0);
        
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
    
    /// Print test summary
    pub fn print_test_summary(&self) {
        let stats = self.get_test_statistics();
        
        println!("\nTest Summary:");
        println!("=============");
        println!("Total Tests: {}", stats.total_tests);
        println!("Passed: {} ({:.1}%)", stats.passed, 
                (stats.passed as f64 / stats.total_tests as f64) * 100.0);
        println!("Failed: {} ({:.1}%)", stats.failed, 
                (stats.failed as f64 / stats.total_tests as f64) * 100.0);
        println!("Skipped: {} ({:.1}%)", stats.skipped, 
                (stats.skipped as f64 / stats.total_tests as f64) * 100.0);
        println!("Error: {} ({:.1}%)", stats.error, 
                (stats.error as f64 / stats.total_tests as f64) * 100.0);
        println!("Total Duration: {:?}", stats.total_duration);
        
        if stats.failed > 0 || stats.error > 0 {
            println!("\nFailed Tests:");
            for (name, result) in &self.test_results {
                if result.status == TestStatus::Failed || result.status == TestStatus::Error {
                    println!("  - {}: {}", name, result.error.as_deref().unwrap_or("Unknown error"));
                }
            }
        }
    }
    
    /// Export test results to JSON
    pub fn export_to_json(&self) -> Result<String, TestError> {
        let mut json = String::new();
        json.push_str("{\n");
        json.push_str("  \"test_results\": [\n");
        
        let mut first = true;
        for (name, result) in &self.test_results {
            if !first {
                json.push_str(",\n");
            }
            first = false;
            
            json.push_str("    {\n");
            json.push_str(&format!("      \"name\": \"{}\",\n", name));
            json.push_str(&format!("      \"status\": \"{:?}\",\n", result.status));
            json.push_str(&format!("      \"duration_ms\": {},\n", result.duration.as_millis()));
            
            if let Some(message) = &result.message {
                json.push_str(&format!("      \"message\": \"{}\",\n", message));
            }
            
            if let Some(error) = &result.error {
                json.push_str(&format!("      \"error\": \"{}\",\n", error));
            }
            
            json.push_str("    }");
        }
        
        json.push_str("\n  ],\n");
        
        let stats = self.get_test_statistics();
        json.push_str("  \"statistics\": {\n");
        json.push_str(&format!("    \"total_tests\": {},\n", stats.total_tests));
        json.push_str(&format!("    \"passed\": {},\n", stats.passed));
        json.push_str(&format!("    \"failed\": {},\n", stats.failed));
        json.push_str(&format!("    \"skipped\": {},\n", stats.skipped));
        json.push_str(&format!("    \"error\": {},\n", stats.error));
        json.push_str(&format!("    \"total_duration_ms\": {}\n", stats.total_duration.as_millis()));
        json.push_str("  }\n");
        json.push_str("}\n");
        
        Ok(json)
    }
    
    /// Export test results to CSV
    pub fn export_to_csv(&self) -> Result<String, TestError> {
        let mut csv = String::new();
        csv.push_str("name,status,duration_ms,message,error\n");
        
        for (name, result) in &self.test_results {
            csv.push_str(&format!("{},{:?},{},", 
                name, 
                result.status, 
                result.duration.as_millis()
            ));
            
            if let Some(message) = &result.message {
                csv.push_str(&format!("\"{}\",", message.replace("\"", "\"\"")));
            } else {
                csv.push_str(",");
            }
            
            if let Some(error) = &result.error {
                csv.push_str(&format!("\"{}\"", error.replace("\"", "\"\"")));
            }
            
            csv.push_str("\n");
        }
        
        Ok(csv)
    }
}

impl Default for TestRunner {
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
            filter: None,
            pattern: None,
        }
    }
}

/// Test configuration
#[derive(Debug, Clone)]
pub struct TestConfig {
    /// Test directory
    pub test_dir: String,
    /// Output directory
    pub output_dir: String,
    /// Test timeout
    pub timeout: Duration,
    /// Verbose output
    pub verbose: bool,
    /// Stop on failure
    pub stop_on_failure: bool,
    /// Test filter
    pub filter: Option<String>,
    /// Test pattern
    pub pattern: Option<String>,
}

impl Default for TestConfig {
    fn default() -> Self {
        Self {
            test_dir: "tests".to_string(),
            output_dir: "test_output".to_string(),
            timeout: Duration::from_secs(30),
            verbose: false,
            stop_on_failure: false,
            filter: None,
            pattern: None,
        }
    }
}

/// Test suite
#[derive(Debug, Clone)]
pub struct TestSuite {
    /// Suite name
    pub name: String,
    /// Test cases
    pub test_cases: Vec<TestCase>,
    /// Suite options
    pub options: TestOptions,
}

/// Test case
#[derive(Debug, Clone)]
pub struct TestCase {
    /// Case name
    pub name: String,
    /// Test function
    pub test_func: fn() -> (),
    /// Expected result
    pub expected: TestStatus,
    /// Test timeout
    pub timeout: Option<Duration>,
}

impl TestSuite {
    /// Create a new test suite
    pub fn new(name: String) -> Self {
        Self {
            name,
            test_cases: Vec::new(),
            options: TestOptions::default(),
        }
    }
    
    /// Add a test case
    pub fn add_test_case(&mut self, test_case: TestCase) {
        self.test_cases.push(test_case);
    }
    
    /// Run the test suite
    pub fn run(&self) -> Result<TestStatistics, TestError> {
        let mut runner = TestRunner::with_options(self.options.clone());
        
        for test_case in &self.test_cases {
            runner.run_test(&test_case.name, test_case.test_func)?;
        }
        
        Ok(runner.get_test_statistics())
    }
}

/// Test utilities
pub mod test_utils {
    use super::*;
    
    /// Create a test case
    pub fn create_test_case<F>(name: String, test_func: F) -> TestCase
    where
        F: FnOnce() -> () + 'static,
    {
        TestCase {
            name,
            test_func: Box::new(test_func) as Box<dyn FnOnce() -> ()>,
            expected: TestStatus::Passed,
            timeout: None,
        }
    }
    
    /// Create a test case with timeout
    pub fn create_test_case_with_timeout<F>(
        name: String, 
        test_func: F, 
        timeout: Duration
    ) -> TestCase
    where
        F: FnOnce() -> () + 'static,
    {
        TestCase {
            name,
            test_func: Box::new(test_func) as Box<dyn FnOnce() -> ()>,
            expected: TestStatus::Passed,
            timeout: Some(timeout),
        }
    }
    
    /// Create a test case with expected result
    pub fn create_test_case_with_expected<F>(
        name: String, 
        test_func: F, 
        expected: TestStatus
    ) -> TestCase
    where
        F: FnOnce() -> () + 'static,
    {
        TestCase {
            name,
            test_func: Box::new(test_func) as Box<dyn FnOnce() -> ()>,
            expected,
            timeout: None,
        }
    }
}