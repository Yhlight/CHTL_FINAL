//! CHTL Parser State
//! 
//! This module manages the parser state during compilation.

use std::collections::HashMap;
use crate::chtl::node::*;

/// Parser state information
pub struct ParserState {
    /// Current scope stack
    pub scope_stack: Vec<Scope>,
    /// Template definitions
    pub templates: HashMap<String, TemplateNode>,
    /// Custom definitions
    pub customs: HashMap<String, CustomNode>,
    /// Imported modules
    pub imports: HashMap<String, ImportNode>,
    /// Current namespace
    pub current_namespace: Option<String>,
}

/// Scope information
#[derive(Debug, Clone)]
pub struct Scope {
    pub scope_type: ScopeType,
    pub variables: HashMap<String, String>,
}

/// Scope types
#[derive(Debug, Clone)]
pub enum ScopeType {
    Global,
    Element,
    Style,
    Script,
    Template,
    Custom,
}

impl ParserState {
    /// Create a new parser state
    pub fn new() -> Self {
        Self {
            scope_stack: vec![Scope {
                scope_type: ScopeType::Global,
                variables: HashMap::new(),
            }],
            templates: HashMap::new(),
            customs: HashMap::new(),
            imports: HashMap::new(),
            current_namespace: None,
        }
    }
    
    /// Push a new scope
    pub fn push_scope(&mut self, scope_type: ScopeType) {
        self.scope_stack.push(Scope {
            scope_type,
            variables: HashMap::new(),
        });
    }
    
    /// Pop the current scope
    pub fn pop_scope(&mut self) -> Option<Scope> {
        self.scope_stack.pop()
    }
    
    /// Get current scope
    pub fn current_scope(&self) -> Option<&Scope> {
        self.scope_stack.last()
    }
    
    /// Set variable in current scope
    pub fn set_variable(&mut self, name: String, value: String) {
        if let Some(scope) = self.scope_stack.last_mut() {
            scope.variables.insert(name, value);
        }
    }
    
    /// Get variable from current scope or parent scopes
    pub fn get_variable(&self, name: &str) -> Option<&String> {
        for scope in self.scope_stack.iter().rev() {
            if let Some(value) = scope.variables.get(name) {
                return Some(value);
            }
        }
        None
    }
    
    /// Add template definition
    pub fn add_template(&mut self, name: String, template: TemplateNode) {
        self.templates.insert(name, template);
    }
    
    /// Get template definition
    pub fn get_template(&self, name: &str) -> Option<&TemplateNode> {
        self.templates.get(name)
    }
    
    /// Add custom definition
    pub fn add_custom(&mut self, name: String, custom: CustomNode) {
        self.customs.insert(name, custom);
    }
    
    /// Get custom definition
    pub fn get_custom(&self, name: &str) -> Option<&CustomNode> {
        self.customs.get(name)
    }
}