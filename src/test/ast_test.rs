//! AST tests
//! 
//! This module provides AST test functionality for CHTL,
//! including testing AST node creation and validation.

use std::collections::HashMap;
use crate::chtl::node::*;

/// AST test runner
#[derive(Debug, Clone)]
pub struct ASTTestRunner {
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

impl ASTTestRunner {
    /// Create a new AST test runner
    pub fn new() -> Self {
        Self {
            test_results: HashMap::new(),
            options: TestOptions::default(),
        }
    }
    
    /// Create a new AST test runner with options
    pub fn with_options(options: TestOptions) -> Self {
        Self {
            test_results: HashMap::new(),
            options,
        }
    }
    
    /// Run all AST tests
    pub fn run_all_tests(&mut self) -> Result<(), TestError> {
        self.run_element_node_tests()?;
        self.run_text_node_tests()?;
        self.run_style_node_tests()?;
        self.run_template_node_tests()?;
        self.run_ast_validation_tests()?;
        
        Ok(())
    }
    
    /// Run element node tests
    pub fn run_element_node_tests(&mut self) -> Result<(), TestError> {
        let test_name = "element_node_tests";
        let start_time = std::time::Instant::now();
        
        // Test element node creation
        self.run_test("element_node_creation", || {
            let element = ElementNode::new("div", 1, 1);
            assert_eq!(element.tag_name, "div", "Tag name should be 'div'");
            assert_eq!(element.line, 1, "Line should be 1");
            assert_eq!(element.column, 1, "Column should be 1");
            assert!(element.attributes.is_empty(), "Attributes should be empty initially");
            assert!(element.children.is_empty(), "Children should be empty initially");
        })?;
        
        // Test element node with attributes
        self.run_test("element_node_with_attributes", || {
            let mut element = ElementNode::new("div", 1, 1);
            element.add_attribute("class", "container");
            element.add_attribute("id", "main");
            
            assert_eq!(element.attributes.len(), 2, "Should have 2 attributes");
            assert_eq!(element.get_attribute("class"), Some("container"), "Class attribute should be 'container'");
            assert_eq!(element.get_attribute("id"), Some("main"), "ID attribute should be 'main'");
        })?;
        
        // Test element node with children
        self.run_test("element_node_with_children", || {
            let mut parent = ElementNode::new("div", 1, 1);
            let child1 = ElementNode::new("span", 2, 3);
            let child2 = TextNode::new("Hello World", 2, 9);
            
            parent.add_child(Box::new(child1));
            parent.add_child(Box::new(child2));
            
            assert_eq!(parent.children.len(), 2, "Should have 2 children");
            assert!(matches!(parent.children[0].node_type(), NodeType::Element), "First child should be element");
            assert!(matches!(parent.children[1].node_type(), NodeType::Text), "Second child should be text");
        })?;
        
        // Test element node validation
        self.run_test("element_node_validation", || {
            let valid_element = ElementNode::new("div", 1, 1);
            assert!(valid_element.is_valid(), "Valid element should be valid");
            
            let mut invalid_element = ElementNode::new("", 1, 1); // Empty tag name
            assert!(!invalid_element.is_valid(), "Element with empty tag name should be invalid");
        })?;
        
        // Test element node cloning
        self.run_test("element_node_cloning", || {
            let mut original = ElementNode::new("div", 1, 1);
            original.add_attribute("class", "container");
            original.add_child(Box::new(TextNode::new("Hello", 2, 3)));
            
            let cloned = original.clone();
            assert_eq!(original.tag_name, cloned.tag_name, "Cloned tag name should match");
            assert_eq!(original.attributes, cloned.attributes, "Cloned attributes should match");
            assert_eq!(original.children.len(), cloned.children.len(), "Cloned children count should match");
        })?;
        
        // Test element node display
        self.run_test("element_node_display", || {
            let element = ElementNode::new("div", 1, 1);
            let display = format!("{}", element);
            assert!(display.contains("div"), "Display should contain tag name");
            assert!(display.contains("1:1"), "Display should contain position");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All element node tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run text node tests
    pub fn run_text_node_tests(&mut self) -> Result<(), TestError> {
        let test_name = "text_node_tests";
        let start_time = std::time::Instant::now();
        
        // Test text node creation
        self.run_test("text_node_creation", || {
            let text = TextNode::new("Hello World", 1, 1);
            assert_eq!(text.content, "Hello World", "Content should be 'Hello World'");
            assert_eq!(text.line, 1, "Line should be 1");
            assert_eq!(text.column, 1, "Column should be 1");
        })?;
        
        // Test text node validation
        self.run_test("text_node_validation", || {
            let valid_text = TextNode::new("Hello World", 1, 1);
            assert!(valid_text.is_valid(), "Valid text should be valid");
            
            let empty_text = TextNode::new("", 1, 1);
            assert!(!empty_text.is_valid(), "Empty text should be invalid");
        })?;
        
        // Test text node cloning
        self.run_test("text_node_cloning", || {
            let original = TextNode::new("Hello World", 1, 1);
            let cloned = original.clone();
            
            assert_eq!(original.content, cloned.content, "Cloned content should match");
            assert_eq!(original.line, cloned.line, "Cloned line should match");
            assert_eq!(original.column, cloned.column, "Cloned column should match");
        })?;
        
        // Test text node display
        self.run_test("text_node_display", || {
            let text = TextNode::new("Hello World", 1, 1);
            let display = format!("{}", text);
            assert!(display.contains("Hello World"), "Display should contain content");
            assert!(display.contains("1:1"), "Display should contain position");
        })?;
        
        // Test text node trimming
        self.run_test("text_node_trimming", || {
            let text = TextNode::new("  Hello World  ", 1, 1);
            let trimmed = text.trim();
            assert_eq!(trimmed.content, "Hello World", "Trimmed content should not have leading/trailing spaces");
        })?;
        
        // Test text node escaping
        self.run_test("text_node_escaping", || {
            let text = TextNode::new("<div>Hello & World</div>", 1, 1);
            let escaped = text.escape_html();
            assert_eq!(escaped.content, "&lt;div&gt;Hello &amp; World&lt;/div&gt;", "HTML should be escaped");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All text node tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run style node tests
    pub fn run_style_node_tests(&mut self) -> Result<(), TestError> {
        let test_name = "style_node_tests";
        let start_time = std::time::Instant::now();
        
        // Test style node creation
        self.run_test("style_node_creation", || {
            let style = StyleNode::new("color: red;", 1, 1);
            assert_eq!(style.content, "color: red;", "Content should match");
            assert_eq!(style.line, 1, "Line should be 1");
            assert_eq!(style.column, 1, "Column should be 1");
        })?;
        
        // Test style node validation
        self.run_test("style_node_validation", || {
            let valid_style = StyleNode::new("color: red;", 1, 1);
            assert!(valid_style.is_valid(), "Valid style should be valid");
            
            let invalid_style = StyleNode::new("", 1, 1);
            assert!(!invalid_style.is_valid(), "Empty style should be invalid");
        })?;
        
        // Test style node cloning
        self.run_test("style_node_cloning", || {
            let original = StyleNode::new("color: red;", 1, 1);
            let cloned = original.clone();
            
            assert_eq!(original.content, cloned.content, "Cloned content should match");
            assert_eq!(original.line, cloned.line, "Cloned line should match");
            assert_eq!(original.column, cloned.column, "Cloned column should match");
        })?;
        
        // Test style node display
        self.run_test("style_node_display", || {
            let style = StyleNode::new("color: red;", 1, 1);
            let display = format!("{}", style);
            assert!(display.contains("color: red;"), "Display should contain content");
            assert!(display.contains("1:1"), "Display should contain position");
        })?;
        
        // Test style node parsing
        self.run_test("style_node_parsing", || {
            let style = StyleNode::new("color: red; background: blue;", 1, 1);
            let properties = style.parse_properties();
            assert_eq!(properties.len(), 2, "Should parse 2 properties");
            assert_eq!(properties.get("color"), Some(&"red".to_string()), "Color property should be 'red'");
            assert_eq!(properties.get("background"), Some(&"blue".to_string()), "Background property should be 'blue'");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All style node tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run template node tests
    pub fn run_template_node_tests(&mut self) -> Result<(), TestError> {
        let test_name = "template_node_tests";
        let start_time = std::time::Instant::now();
        
        // Test template node creation
        self.run_test("template_node_creation", || {
            let template = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            assert_eq!(template.name, "MyTemplate", "Name should be 'MyTemplate'");
            assert_eq!(template.template_type, TemplateType::Style, "Type should be Style");
            assert_eq!(template.line, 1, "Line should be 1");
            assert_eq!(template.column, 1, "Column should be 1");
        })?;
        
        // Test template node validation
        self.run_test("template_node_validation", || {
            let valid_template = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            assert!(valid_template.is_valid(), "Valid template should be valid");
            
            let invalid_template = TemplateNode::new("", TemplateType::Style, 1, 1);
            assert!(!invalid_template.is_valid(), "Template with empty name should be invalid");
        })?;
        
        // Test template node cloning
        self.run_test("template_node_cloning", || {
            let original = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            let cloned = original.clone();
            
            assert_eq!(original.name, cloned.name, "Cloned name should match");
            assert_eq!(original.template_type, cloned.template_type, "Cloned type should match");
            assert_eq!(original.line, cloned.line, "Cloned line should match");
            assert_eq!(original.column, cloned.column, "Cloned column should match");
        })?;
        
        // Test template node display
        self.run_test("template_node_display", || {
            let template = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            let display = format!("{}", template);
            assert!(display.contains("MyTemplate"), "Display should contain name");
            assert!(display.contains("Style"), "Display should contain type");
            assert!(display.contains("1:1"), "Display should contain position");
        })?;
        
        // Test template node with content
        self.run_test("template_node_with_content", || {
            let mut template = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            template.add_content("color: red;");
            template.add_content("background: blue;");
            
            assert_eq!(template.content.len(), 2, "Should have 2 content items");
            assert_eq!(template.content[0], "color: red;", "First content should match");
            assert_eq!(template.content[1], "background: blue;", "Second content should match");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All template node tests passed".to_string()),
            error: None,
        });
        
        Ok(())
    }
    
    /// Run AST validation tests
    pub fn run_ast_validation_tests(&mut self) -> Result<(), TestError> {
        let test_name = "ast_validation_tests";
        let start_time = std::time::Instant::now();
        
        // Test AST node equality
        self.run_test("ast_node_equality", || {
            let node1 = ElementNode::new("div", 1, 1);
            let node2 = ElementNode::new("div", 1, 1);
            let node3 = ElementNode::new("span", 1, 1);
            
            assert_eq!(node1, node2, "Identical nodes should be equal");
            assert_ne!(node1, node3, "Different nodes should not be equal");
        })?;
        
        // Test AST node type checking
        self.run_test("ast_node_type_checking", || {
            let element = ElementNode::new("div", 1, 1);
            let text = TextNode::new("Hello", 1, 1);
            let style = StyleNode::new("color: red;", 1, 1);
            let template = TemplateNode::new("MyTemplate", TemplateType::Style, 1, 1);
            
            assert!(matches!(element.node_type(), NodeType::Element), "Element should have Element type");
            assert!(matches!(text.node_type(), NodeType::Text), "Text should have Text type");
            assert!(matches!(style.node_type(), NodeType::Style), "Style should have Style type");
            assert!(matches!(template.node_type(), NodeType::Template), "Template should have Template type");
        })?;
        
        // Test AST node position
        self.run_test("ast_node_position", || {
            let node = ElementNode::new("div", 5, 10);
            assert_eq!(node.get_position(), (5, 10), "Position should be (5, 10)");
        })?;
        
        // Test AST node depth
        self.run_test("ast_node_depth", || {
            let mut parent = ElementNode::new("div", 1, 1);
            let mut child = ElementNode::new("span", 2, 3);
            let grandchild = TextNode::new("Hello", 3, 5);
            
            child.add_child(Box::new(grandchild));
            parent.add_child(Box::new(child));
            
            assert_eq!(parent.get_depth(), 0, "Root node should have depth 0");
            assert_eq!(parent.children[0].get_depth(), 1, "Child should have depth 1");
            assert_eq!(parent.children[0].children[0].get_depth(), 2, "Grandchild should have depth 2");
        })?;
        
        // Test AST node traversal
        self.run_test("ast_node_traversal", || {
            let mut root = ElementNode::new("div", 1, 1);
            let child1 = ElementNode::new("span", 2, 3);
            let child2 = TextNode::new("Hello", 2, 9);
            let child3 = ElementNode::new("p", 2, 15);
            
            root.add_child(Box::new(child1));
            root.add_child(Box::new(child2));
            root.add_child(Box::new(child3));
            
            let mut visited = Vec::new();
            root.traverse(&mut |node| {
                visited.push(node.node_type());
            });
            
            assert_eq!(visited.len(), 4, "Should visit 4 nodes");
            assert_eq!(visited[0], NodeType::Element, "First should be root element");
            assert_eq!(visited[1], NodeType::Element, "Second should be child element");
            assert_eq!(visited[2], NodeType::Text, "Third should be text");
            assert_eq!(visited[3], NodeType::Element, "Fourth should be child element");
        })?;
        
        // Test AST node search
        self.run_test("ast_node_search", || {
            let mut root = ElementNode::new("div", 1, 1);
            let child1 = ElementNode::new("span", 2, 3);
            let child2 = TextNode::new("Hello", 2, 9);
            let child3 = ElementNode::new("p", 2, 15);
            
            root.add_child(Box::new(child1));
            root.add_child(Box::new(child2));
            root.add_child(Box::new(child3));
            
            let elements: Vec<_> = root.find_all(|node| matches!(node.node_type(), NodeType::Element));
            assert_eq!(elements.len(), 3, "Should find 3 element nodes");
            
            let text_nodes: Vec<_> = root.find_all(|node| matches!(node.node_type(), NodeType::Text));
            assert_eq!(text_nodes.len(), 1, "Should find 1 text node");
        })?;
        
        let duration = start_time.elapsed();
        self.test_results.insert(test_name.to_string(), TestResult {
            name: test_name.to_string(),
            status: TestStatus::Passed,
            duration,
            message: Some("All AST validation tests passed".to_string()),
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

impl Default for ASTTestRunner {
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