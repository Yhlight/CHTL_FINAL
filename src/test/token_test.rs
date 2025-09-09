//! Token tests
//! 
//! This module provides token test functionality for CHTL,
//! including testing tokenization and token validation.

use std::collections::HashMap;
use crate::chtl::lexer::*;

/// Token test runner
#[derive(Debug, Clone)]
pub struct TokenTestRunner {
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

impl TokenTestRunner {
    /// Create a new token test runner
    pub fn new() -> Self {
        Self {
            test_results: HashMap::new(),
            options: TestOptions::default(),
        }
    }
    
    /// Create a new token test runner with options
    pub fn with_options(options: TestOptions) -> Self {
        Self {
            test_results: HashMap::new(),
            options,
        }
    }
    
    /// Run all token tests
    pub fn run_all_tests(&mut self) -> Result<(), TestError> {
        self.run_lexer_tests()?;
        self.run_token_validation_tests()?;
        self.run_token_stream_tests()?;
        
        Ok(())
    }
    
    /// Run lexer tests
    pub fn run_lexer_tests(&mut self) -> Result<(), TestError> {
        let test_name = "lexer_tests";
        let start_time = std::time::Instant::now();
        
        let mut lexer = CHTLLexer::new();
        
        // Test basic tokenization
        self.run_test("basic_tokenization", || {
            let input = "div { color: red; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            assert_eq!(tokens[0].token_type, TokenType::Identifier, "First token should be identifier");
            assert_eq!(tokens[0].value, "div", "First token value should be 'div'");
        })?;
        
        // Test comment tokenization
        self.run_test("comment_tokenization", || {
            let input = "// This is a comment\ndiv { color: red; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            assert_eq!(tokens[0].token_type, TokenType::Comment, "First token should be comment");
            assert_eq!(tokens[0].value, "// This is a comment", "Comment value should match");
        })?;
        
        // Test string literal tokenization
        self.run_test("string_literal_tokenization", || {
            let input = r#"div { content: "Hello World"; }"#;
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let string_token = tokens.iter().find(|t| t.token_type == TokenType::StringLiteral);
            assert!(string_token.is_some(), "Should find string literal token");
            assert_eq!(string_token.unwrap().value, "\"Hello World\"", "String literal value should match");
        })?;
        
        // Test number literal tokenization
        self.run_test("number_literal_tokenization", || {
            let input = "div { width: 100px; height: 200px; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let number_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::NumberLiteral).collect();
            assert_eq!(number_tokens.len(), 2, "Should find 2 number literal tokens");
            assert_eq!(number_tokens[0].value, "100", "First number should be 100");
            assert_eq!(number_tokens[1].value, "200", "Second number should be 200");
        })?;
        
        // Test operator tokenization
        self.run_test("operator_tokenization", || {
            let input = "div { width: 100px; height: 200px; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let colon_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::Colon).collect();
            assert_eq!(colon_tokens.len(), 2, "Should find 2 colon tokens");
            let semicolon_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::Semicolon).collect();
            assert_eq!(semicolon_tokens.len(), 2, "Should find 2 semicolon tokens");
        })?;
        
        // Test bracket tokenization
        self.run_test("bracket_tokenization", || {
            let input = "div { color: red; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let open_brace = tokens.iter().find(|t| t.token_type == TokenType::OpenBrace);
            let close_brace = tokens.iter().find(|t| t.token_type == TokenType::CloseBrace);
            assert!(open_brace.is_some(), "Should find open brace");
            assert!(close_brace.is_some(), "Should find close brace");
        })?;
        
        // Test identifier tokenization
        self.run_test("identifier_tokenization", || {
            let input = "myDiv { color: red; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let identifier_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::Identifier).collect();
            assert!(identifier_tokens.len() >= 2, "Should find at least 2 identifier tokens");
            assert_eq!(identifier_tokens[0].value, "myDiv", "First identifier should be 'myDiv'");
            assert_eq!(identifier_tokens[1].value, "color", "Second identifier should be 'color'");
        })?;
        
        // Test keyword tokenization
        self.run_test("keyword_tokenization", || {
            let input = "if (true) { color: red; }";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let if_token = tokens.iter().find(|t| t.token_type == TokenType::If);
            let true_token = tokens.iter().find(|t| t.token_type == TokenType::True);
            assert!(if_token.is_some(), "Should find 'if' keyword");
            assert!(true_token.is_some(), "Should find 'true' keyword");
        })?;
        
        // Test whitespace handling
        self.run_test("whitespace_handling", || {
            let input = "  div  {  color  :  red  ;  }  ";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let whitespace_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::Whitespace).collect();
            assert!(whitespace_tokens.is_empty(), "Should not produce whitespace tokens");
        })?;
        
        // Test newline handling
        self.run_test("newline_handling", || {
            let input = "div\n{\ncolor: red;\n}";
            let tokens = lexer.tokenize(input).unwrap();
            assert!(!tokens.is_empty(), "Should produce tokens");
            let newline_tokens: Vec<_> = tokens.iter().filter(|t| t.token_type == TokenType::Newline).collect();
            assert!(newline_tokens.is_empty(), "Should not produce newline tokens");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All lexer tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run token validation tests
    pub fn run_token_validation_tests(&mut self) -> Result<(), TestError> {
        let test_name = "token_validation_tests";
        let start_time = std::time::Instant::now();
        
        // Test valid tokens
        self.run_test("valid_tokens", || {
            let valid_tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::StringLiteral, "\"hello\"", 1, 5),
                Token::new(TokenType::NumberLiteral, "123", 1, 10),
                Token::new(TokenType::Colon, ":", 1, 15),
                Token::new(TokenType::Semicolon, ";", 1, 20),
            ];
            
            for token in valid_tokens {
                assert!(token.is_valid(), "Token should be valid: {:?}", token);
            }
        })?;
        
        // Test invalid tokens
        self.run_test("invalid_tokens", || {
            let invalid_tokens = vec![
                Token::new(TokenType::Identifier, "", 1, 1), // Empty identifier
                Token::new(TokenType::StringLiteral, "unclosed", 1, 1), // Unclosed string
                Token::new(TokenType::NumberLiteral, "abc", 1, 1), // Non-numeric number
            ];
            
            for token in invalid_tokens {
                assert!(!token.is_valid(), "Token should be invalid: {:?}", token);
            }
        })?;
        
        // Test token equality
        self.run_test("token_equality", || {
            let token1 = Token::new(TokenType::Identifier, "div", 1, 1);
            let token2 = Token::new(TokenType::Identifier, "div", 1, 1);
            let token3 = Token::new(TokenType::Identifier, "span", 1, 1);
            
            assert_eq!(token1, token2, "Identical tokens should be equal");
            assert_ne!(token1, token3, "Different tokens should not be equal");
        })?;
        
        // Test token cloning
        self.run_test("token_cloning", || {
            let original = Token::new(TokenType::Identifier, "div", 1, 1);
            let cloned = original.clone();
            
            assert_eq!(original, cloned, "Cloned token should equal original");
            assert_eq!(original.token_type, cloned.token_type, "Token types should match");
            assert_eq!(original.value, cloned.value, "Token values should match");
            assert_eq!(original.line, cloned.line, "Token lines should match");
            assert_eq!(original.column, cloned.column, "Token columns should match");
        })?;
        
        // Test token display
        self.run_test("token_display", || {
            let token = Token::new(TokenType::Identifier, "div", 1, 1);
            let display = format!("{}", token);
            assert!(display.contains("Identifier"), "Display should contain token type");
            assert!(display.contains("div"), "Display should contain token value");
            assert!(display.contains("1:1"), "Display should contain position");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All token validation tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run token stream tests
    pub fn run_token_stream_tests(&mut self) -> Result<(), TestError> {
        let test_name = "token_stream_tests";
        let start_time = std::time::Instant::now();
        
        // Test token stream creation
        self.run_test("token_stream_creation", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::Identifier, "color", 1, 7),
                Token::new(TokenType::Colon, ":", 1, 12),
                Token::new(TokenType::StringLiteral, "\"red\"", 1, 14),
                Token::new(TokenType::Semicolon, ";", 1, 19),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let stream = TokenStream::new(tokens);
            assert_eq!(stream.len(), 7, "Stream should have 7 tokens");
            assert!(!stream.is_empty(), "Stream should not be empty");
        })?;
        
        // Test token stream iteration
        self.run_test("token_stream_iteration", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let stream = TokenStream::new(tokens);
            let collected: Vec<_> = stream.iter().collect();
            assert_eq!(collected.len(), 3, "Should iterate over all tokens");
            assert_eq!(collected[0].value, "div", "First token should be 'div'");
            assert_eq!(collected[1].value, "{", "Second token should be '{'");
            assert_eq!(collected[2].value, "}", "Third token should be '}'");
        })?;
        
        // Test token stream peek
        self.run_test("token_stream_peek", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let mut stream = TokenStream::new(tokens);
            assert_eq!(stream.peek().unwrap().value, "div", "Peek should return first token");
            assert_eq!(stream.peek().unwrap().value, "div", "Peek should not advance position");
            
            stream.advance();
            assert_eq!(stream.peek().unwrap().value, "{", "Peek should return second token after advance");
        })?;
        
        // Test token stream advance
        self.run_test("token_stream_advance", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let mut stream = TokenStream::new(tokens);
            assert_eq!(stream.current_position(), 0, "Initial position should be 0");
            
            stream.advance();
            assert_eq!(stream.current_position(), 1, "Position should be 1 after advance");
            
            stream.advance();
            assert_eq!(stream.current_position(), 2, "Position should be 2 after second advance");
        })?;
        
        // Test token stream position
        self.run_test("token_stream_position", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let mut stream = TokenStream::new(tokens);
            assert_eq!(stream.current_position(), 0, "Initial position should be 0");
            assert!(!stream.is_at_end(), "Should not be at end initially");
            
            stream.advance();
            stream.advance();
            stream.advance();
            assert!(stream.is_at_end(), "Should be at end after advancing past all tokens");
        })?;
        
        // Test token stream reset
        self.run_test("token_stream_reset", || {
            let tokens = vec![
                Token::new(TokenType::Identifier, "div", 1, 1),
                Token::new(TokenType::OpenBrace, "{", 1, 5),
                Token::new(TokenType::CloseBrace, "}", 1, 21),
            ];
            
            let mut stream = TokenStream::new(tokens);
            stream.advance();
            stream.advance();
            assert_eq!(stream.current_position(), 2, "Position should be 2 after advances");
            
            stream.reset();
            assert_eq!(stream.current_position(), 0, "Position should be 0 after reset");
            assert_eq!(stream.peek().unwrap().value, "div", "Should be back at first token after reset");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All token stream tests passed".to_string()),
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

impl Default for TokenTestRunner {
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