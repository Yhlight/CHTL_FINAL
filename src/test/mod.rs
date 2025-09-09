//! Test modules
//! 
//! This module contains test utilities and test cases for CHTL.

pub mod util_test;
pub mod token_test;
pub mod ast_test;

/// Test utilities
pub struct TestUtil;

impl TestUtil {
    /// Run all tests
    pub fn run_all_tests() -> Result<(), Box<dyn std::error::Error>> {
        println!("Running CHTL tests...");
        
        // Run utility tests
        util_test::run_tests()?;
        
        // Run token tests
        token_test::run_tests()?;
        
        // Run AST tests
        ast_test::run_tests()?;
        
        println!("All tests passed!");
        Ok(())
    }
    
    /// Run specific test category
    pub fn run_tests(category: &str) -> Result<(), Box<dyn std::error::Error>> {
        match category {
            "util" => util_test::run_tests()?,
            "token" => token_test::run_tests()?,
            "ast" => ast_test::run_tests()?,
            _ => return Err("Unknown test category".into()),
        }
        Ok(())
    }
}