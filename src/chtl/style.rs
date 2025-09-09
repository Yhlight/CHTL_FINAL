//! CHTL style system
//! 
//! This module provides the local style block system for CHTL,
//! including inline styles, automatic class/ID assignment,
//! context derivation, and attribute conditional expressions.

use std::collections::HashMap;
use crate::chtl::lexer::SourceLocation;

/// Style block for local styling
#[derive(Debug, Clone, PartialEq)]
pub struct StyleBlock {
    /// Style properties
    pub properties: Vec<(String, CSSValue)>,
    /// CSS class name (auto-generated or explicit)
    pub class_name: Option<String>,
    /// CSS ID (auto-generated or explicit)
    pub id: Option<String>,
    /// Context derivation rules
    pub context_rules: Vec<ContextRule>,
    /// Attribute conditional expressions
    pub attribute_conditions: Vec<AttributeCondition>,
    /// Source location
    pub location: SourceLocation,
}

/// CSS value (can be literal, variable, or expression)
#[derive(Debug, Clone, PartialEq)]
pub enum CSSValue {
    /// Literal value (e.g., "16px", "red")
    Literal(String),
    /// Variable reference (e.g., $primary-color)
    Variable(String),
    /// Expression (e.g., calc(100% - 20px))
    Expression(String),
    /// Function call (e.g., rgb(255, 0, 0))
    Function(String, Vec<String>),
}

/// Context derivation rule
#[derive(Debug, Clone, PartialEq)]
pub struct ContextRule {
    /// Context selector (e.g., "hover", "focus", "active")
    pub selector: String,
    /// Style properties for this context
    pub properties: Vec<(String, CSSValue)>,
    /// Priority level
    pub priority: u32,
}

/// Attribute conditional expression
#[derive(Debug, Clone, PartialEq)]
pub struct AttributeCondition {
    /// Attribute name
    pub attribute: String,
    /// Condition operator (=, !=, ~=, ^=, $=, *=)
    pub operator: AttributeOperator,
    /// Condition value
    pub value: String,
    /// Style properties when condition is met
    pub properties: Vec<(String, CSSValue)>,
}

/// Attribute condition operators
#[derive(Debug, Clone, PartialEq)]
pub enum AttributeOperator {
    /// Equals (=)
    Equals,
    /// Not equals (!=)
    NotEquals,
    /// Contains word (~=)
    ContainsWord,
    /// Starts with (^=)
    StartsWith,
    /// Ends with ($=)
    EndsWith,
    /// Contains substring (*=)
    Contains,
}

/// Style system for managing local styles
#[derive(Debug, Clone)]
pub struct StyleSystem {
    /// Global style blocks
    global_styles: HashMap<String, StyleBlock>,
    /// Context-aware style blocks
    context_styles: HashMap<String, Vec<StyleBlock>>,
    /// Auto-generated class counter
    class_counter: u32,
    /// Auto-generated ID counter
    id_counter: u32,
}

impl StyleBlock {
    /// Create a new style block
    pub fn new() -> Self {
        Self {
            properties: Vec::new(),
            class_name: None,
            id: None,
            context_rules: Vec::new(),
            attribute_conditions: Vec::new(),
            location: SourceLocation::default(),
        }
    }
    
    /// Add a style property
    pub fn add_property(&mut self, name: String, value: CSSValue) {
        self.properties.push((name, value));
    }
    
    /// Add a context rule
    pub fn add_context_rule(&mut self, rule: ContextRule) {
        self.context_rules.push(rule);
    }
    
    /// Add an attribute condition
    pub fn add_attribute_condition(&mut self, condition: AttributeCondition) {
        self.attribute_conditions.push(condition);
    }
    
    /// Set the class name
    pub fn set_class_name(&mut self, class_name: String) {
        self.class_name = Some(class_name);
    }
    
    /// Set the ID
    pub fn set_id(&mut self, id: String) {
        self.id = Some(id);
    }
    
    /// Generate CSS for this style block
    pub fn to_css(&self) -> String {
        let mut css = String::new();
        
        // Generate class selector
        if let Some(class_name) = &self.class_name {
            css.push_str(&format!(".{} {{\n", class_name));
        } else if let Some(id) = &self.id {
            css.push_str(&format!("#{} {{\n", id));
        } else {
            css.push_str("/* Unnamed style block */\n");
        }
        
        // Add properties
        for (name, value) in &self.properties {
            css.push_str(&format!("  {}: {};\n", name, self.value_to_css(value)));
        }
        
        css.push_str("}\n");
        
        // Add context rules
        for rule in &self.context_rules {
            css.push_str(&format!(".{}:{} {{\n", 
                self.class_name.as_deref().unwrap_or("unnamed"), 
                rule.selector
            ));
            for (name, value) in &rule.properties {
                css.push_str(&format!("  {}: {};\n", name, self.value_to_css(value)));
            }
            css.push_str("}\n");
        }
        
        // Add attribute conditions
        for condition in &self.attribute_conditions {
            css.push_str(&format!(".{}[{} {} \"{}\"] {{\n", 
                self.class_name.as_deref().unwrap_or("unnamed"),
                condition.attribute,
                condition.operator.to_css(),
                condition.value
            ));
            for (name, value) in &condition.properties {
                css.push_str(&format!("  {}: {};\n", name, self.value_to_css(value)));
            }
            css.push_str("}\n");
        }
        
        css
    }
    
    /// Convert a style value to CSS
    fn value_to_css(&self, value: &CSSValue) -> String {
        match value {
            CSSValue::Literal(s) => s.clone(),
            CSSValue::Variable(var) => format!("var(--{})", var),
            CSSValue::Expression(expr) => expr.clone(),
            CSSValue::Function(name, args) => {
                format!("{}({})", name, args.join(", "))
            }
        }
    }
}

impl AttributeOperator {
    /// Convert to CSS attribute selector operator
    pub fn to_css(&self) -> &'static str {
        match self {
            AttributeOperator::Equals => "=",
            AttributeOperator::NotEquals => "!=",
            AttributeOperator::ContainsWord => "~=",
            AttributeOperator::StartsWith => "^=",
            AttributeOperator::EndsWith => "$=",
            AttributeOperator::Contains => "*=",
        }
    }
}

impl StyleSystem {
    /// Create a new style system
    pub fn new() -> Self {
        Self {
            global_styles: HashMap::new(),
            context_styles: HashMap::new(),
            class_counter: 0,
            id_counter: 0,
        }
    }
    
    /// Add a style block
    pub fn add_style_block(&mut self, name: String, style_block: StyleBlock) {
        self.global_styles.insert(name, style_block);
    }
    
    /// Generate auto class name
    pub fn generate_class_name(&mut self) -> String {
        self.class_counter += 1;
        format!("chtl-class-{}", self.class_counter)
    }
    
    /// Generate auto ID
    pub fn generate_id(&mut self) -> String {
        self.id_counter += 1;
        format!("chtl-id-{}", self.id_counter)
    }
    
    /// Get all CSS for the style system
    pub fn to_css(&self) -> String {
        let mut css = String::new();
        
        for (_, style_block) in self.global_styles.iter() {
            css.push_str(&style_block.to_css());
            css.push('\n');
        }
        
        css
    }
}

impl Default for StyleBlock {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for StyleSystem {
    fn default() -> Self {
        Self::new()
    }
}