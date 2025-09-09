//! CHTL compiler context
//! 
//! This module provides context management for the CHTL compiler,
//! including state, configuration, and error handling.

use std::collections::HashMap;
use std::path::PathBuf;

/// CHTL compiler context
#[derive(Debug, Clone)]
pub struct CHTLContext {
    /// Source file path
    pub source_file: Option<PathBuf>,
    /// Base directory for relative imports
    pub base_dir: PathBuf,
    /// Configuration options
    pub config: CHTLConfig,
    /// Template registry
    pub templates: HashMap<String, TemplateInfo>,
    /// Custom definitions registry
    pub customs: HashMap<String, CustomInfo>,
    /// Namespace stack
    pub namespaces: Vec<String>,
    /// Current scope variables
    pub variables: HashMap<String, VariableInfo>,
}

/// CHTL configuration
#[derive(Debug, Clone)]
pub struct CHTLConfig {
    /// Debug mode
    pub debug_mode: bool,
    /// Index initial count
    pub index_initial_count: usize,
    /// Enable style auto-add
    pub enable_style_auto_add: bool,
    /// Enable script auto-add
    pub enable_script_auto_add: bool,
    /// Custom keyword mappings
    pub keyword_mappings: HashMap<String, String>,
}

/// Template information
#[derive(Debug, Clone)]
pub struct TemplateInfo {
    pub name: String,
    pub template_type: TemplateType,
    pub parameters: Vec<String>,
    pub content: String,
}

/// Template type
#[derive(Debug, Clone, PartialEq)]
pub enum TemplateType {
    Style,
    Element,
    Variable,
}

/// Custom definition information
#[derive(Debug, Clone)]
pub struct CustomInfo {
    pub name: String,
    pub custom_type: CustomType,
    pub base_template: Option<String>,
    pub modifications: Vec<String>,
}

/// Custom type
#[derive(Debug, Clone, PartialEq)]
pub enum CustomType {
    Style,
    Element,
}

/// Variable information
#[derive(Debug, Clone)]
pub struct VariableInfo {
    pub name: String,
    pub value: String,
    pub variable_type: VariableType,
}

/// Variable type
#[derive(Debug, Clone, PartialEq)]
pub enum VariableType {
    String,
    Number,
    Boolean,
    Color,
    Size,
}

/// Module type
#[derive(Debug, Clone, PartialEq)]
pub enum ModuleType {
    CHTL,
    HTML,
    CSS,
    JavaScript,
    CMOD,
    CJMOD,
}

impl CHTLContext {
    /// Create a new CHTL context
    pub fn new() -> Self {
        Self {
            source_file: None,
            base_dir: PathBuf::from("."),
            config: CHTLConfig::default(),
            templates: HashMap::new(),
            customs: HashMap::new(),
            namespaces: Vec::new(),
            variables: HashMap::new(),
        }
    }
    
    /// Set source file
    pub fn set_source_file(&mut self, path: PathBuf) {
        self.source_file = Some(path.clone());
        if let Some(parent) = path.parent() {
            self.base_dir = parent.to_path_buf();
        }
    }
    
    /// Add template
    pub fn add_template(&mut self, template: TemplateInfo) {
        self.templates.insert(template.name.clone(), template);
    }
    
    /// Add custom definition
    pub fn add_custom(&mut self, custom: CustomInfo) {
        self.customs.insert(custom.name.clone(), custom);
    }
    
    /// Enter namespace
    pub fn enter_namespace(&mut self, namespace: String) {
        self.namespaces.push(namespace);
    }
    
    /// Exit namespace
    pub fn exit_namespace(&mut self) {
        self.namespaces.pop();
    }
    
    /// Get current namespace
    pub fn current_namespace(&self) -> String {
        self.namespaces.join("::")
    }
    
    /// Set variable
    pub fn set_variable(&mut self, variable: VariableInfo) {
        self.variables.insert(variable.name.clone(), variable);
    }
    
    /// Get variable
    pub fn get_variable(&self, name: &str) -> Option<&VariableInfo> {
        self.variables.get(name)
    }
    
    /// Get template
    pub fn get_template(&self, name: &str) -> Option<&TemplateInfo> {
        self.templates.get(name)
    }
    
    /// Get custom
    pub fn get_custom(&self, name: &str) -> Option<&CustomInfo> {
        self.customs.get(name)
    }
}

impl Default for CHTLContext {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CHTLConfig {
    fn default() -> Self {
        Self {
            debug_mode: false,
            index_initial_count: 0,
            enable_style_auto_add: true,
            enable_script_auto_add: true,
            keyword_mappings: HashMap::new(),
        }
    }
}