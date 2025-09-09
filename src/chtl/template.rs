//! CHTL template system
//! 
//! This module provides the template system for CHTL,
//! including style group templates, element templates,
//! variable group templates, and composition inheritance.

use std::collections::HashMap;
use crate::chtl::lexer::SourceLocation;
use crate::chtl::style::{StyleBlock, CSSValue};
use crate::chtl::node::{Node, ElementNode, TextNode, StyleNode, StyleSelector, StyleProperty, StyleValue, AttributeValue};

/// Template system for managing reusable components
#[derive(Debug, Clone)]
pub struct TemplateSystem {
    /// Style group templates
    style_templates: HashMap<String, StyleGroupTemplate>,
    /// Element templates
    element_templates: HashMap<String, ElementTemplate>,
    /// Variable group templates
    variable_templates: HashMap<String, VariableGroupTemplate>,
    /// Template inheritance chain
    inheritance_chain: HashMap<String, Vec<String>>,
}

/// Style group template
#[derive(Debug, Clone, PartialEq)]
pub struct StyleGroupTemplate {
    /// Template name
    pub name: String,
    /// Base style block
    pub base_style: StyleBlock,
    /// Variable definitions
    pub variables: HashMap<String, CSSValue>,
    /// Template parameters
    pub parameters: Vec<TemplateParameter>,
    /// Source location
    pub location: SourceLocation,
}

/// Element template
#[derive(Debug, Clone, PartialEq)]
pub struct ElementTemplate {
    /// Template name
    pub name: String,
    /// Base element structure
    pub base_element: ElementNode,
    /// Variable definitions
    pub variables: HashMap<String, CSSValue>,
    /// Template parameters
    pub parameters: Vec<TemplateParameter>,
    /// Source location
    pub location: SourceLocation,
}

/// Variable group template
#[derive(Debug, Clone, PartialEq)]
pub struct VariableGroupTemplate {
    /// Template name
    pub name: String,
    /// Variable definitions
    pub variables: HashMap<String, CSSValue>,
    /// Template parameters
    pub parameters: Vec<TemplateParameter>,
    /// Source location
    pub location: SourceLocation,
}

/// Template parameter definition
#[derive(Debug, Clone, PartialEq)]
pub struct TemplateParameter {
    /// Parameter name
    pub name: String,
    /// Parameter type
    pub param_type: ParameterType,
    /// Default value (optional)
    pub default_value: Option<CSSValue>,
    /// Whether parameter is required
    pub required: bool,
}

/// Template parameter types
#[derive(Debug, Clone, PartialEq)]
pub enum ParameterType {
    /// String parameter
    String,
    /// Number parameter
    Number,
    /// Color parameter
    Color,
    /// Size parameter (e.g., "16px", "1em")
    Size,
    /// Boolean parameter
    Boolean,
    /// CSS value parameter
    CSSValue,
}

/// Template instance with applied parameters
#[derive(Debug, Clone, PartialEq)]
pub struct TemplateInstance {
    /// Template name
    pub template_name: String,
    /// Applied parameters
    pub parameters: HashMap<String, CSSValue>,
    /// Resolved content
    pub resolved_content: Node,
}

impl TemplateSystem {
    /// Create a new template system
    pub fn new() -> Self {
        Self {
            style_templates: HashMap::new(),
            element_templates: HashMap::new(),
            variable_templates: HashMap::new(),
            inheritance_chain: HashMap::new(),
        }
    }
    
    /// Add a style group template
    pub fn add_style_template(&mut self, template: StyleGroupTemplate) {
        self.style_templates.insert(template.name.clone(), template);
    }
    
    /// Add an element template
    pub fn add_element_template(&mut self, template: ElementTemplate) {
        self.element_templates.insert(template.name.clone(), template);
    }
    
    /// Add a variable group template
    pub fn add_variable_template(&mut self, template: VariableGroupTemplate) {
        self.variable_templates.insert(template.name.clone(), template);
    }
    
    /// Get a style template
    pub fn get_style_template(&self, name: &str) -> Option<&StyleGroupTemplate> {
        self.style_templates.get(name)
    }
    
    /// Get an element template
    pub fn get_element_template(&self, name: &str) -> Option<&ElementTemplate> {
        self.element_templates.get(name)
    }
    
    /// Get a variable template
    pub fn get_variable_template(&self, name: &str) -> Option<&VariableGroupTemplate> {
        self.variable_templates.get(name)
    }
    
    /// Create a template instance
    pub fn create_instance(&self, template_name: &str, parameters: HashMap<String, CSSValue>) -> Result<TemplateInstance, TemplateError> {
        // Try to find the template
        if let Some(style_template) = self.style_templates.get(template_name) {
            return self.create_style_instance(style_template, parameters);
        }
        
        if let Some(element_template) = self.element_templates.get(template_name) {
            return self.create_element_instance(element_template, parameters);
        }
        
        if let Some(variable_template) = self.variable_templates.get(template_name) {
            return self.create_variable_instance(variable_template, parameters);
        }
        
        Err(TemplateError::TemplateNotFound(template_name.to_string()))
    }
    
    /// Create a style template instance
    fn create_style_instance(&self, template: &StyleGroupTemplate, parameters: HashMap<String, CSSValue>) -> Result<TemplateInstance, TemplateError> {
        // Validate parameters
        self.validate_parameters(&template.parameters, &parameters)?;
        
        // Merge template variables with parameters
        let mut resolved_variables = template.variables.clone();
        for (key, value) in &parameters {
            resolved_variables.insert(key.clone(), value.clone());
        }
        
        // Create resolved style block
        let mut resolved_style = template.base_style.clone();
        self.resolve_style_variables(&mut resolved_style, &resolved_variables);
        
        // Create resolved content node
        let resolved_content = Node::Style(StyleNode {
            selector: StyleSelector::Class(resolved_style.class_name.unwrap_or_else(|| "unnamed".to_string())),
            properties: resolved_style.properties.into_iter().map(|(name, value)| StyleProperty {
                name,
                value: match value {
                    CSSValue::Literal(s) => StyleValue::Literal(s),
                    CSSValue::Variable(s) => StyleValue::Literal(format!("var(--{})", s)),
                    CSSValue::Expression(s) => StyleValue::Literal(s),
                    CSSValue::Function(name, args) => StyleValue::Literal(format!("{}({})", name, args.join(", "))),
                },
                location: SourceLocation::default(),
            }).collect(),
            nested: Vec::new(),
            location: SourceLocation::default(),
        });
        
        Ok(TemplateInstance {
            template_name: template.name.clone(),
            parameters,
            resolved_content,
        })
    }
    
    /// Create an element template instance
    fn create_element_instance(&self, template: &ElementTemplate, parameters: HashMap<String, CSSValue>) -> Result<TemplateInstance, TemplateError> {
        // Validate parameters
        self.validate_parameters(&template.parameters, &parameters)?;
        
        // Merge template variables with parameters
        let mut resolved_variables = template.variables.clone();
        for (key, value) in &parameters {
            resolved_variables.insert(key.clone(), value.clone());
        }
        
        // Create resolved element
        let mut resolved_element = template.base_element.clone();
        self.resolve_element_variables(&mut resolved_element, &resolved_variables);
        
        Ok(TemplateInstance {
            template_name: template.name.clone(),
            parameters,
            resolved_content: Node::Element(resolved_element),
        })
    }
    
    /// Create a variable template instance
    fn create_variable_instance(&self, template: &VariableGroupTemplate, parameters: HashMap<String, CSSValue>) -> Result<TemplateInstance, TemplateError> {
        // Validate parameters
        self.validate_parameters(&template.parameters, &parameters)?;
        
        // Merge template variables with parameters
        let mut resolved_variables = template.variables.clone();
        for (key, value) in &parameters {
            resolved_variables.insert(key.clone(), value.clone());
        }
        
        // Create a text node with resolved variables
        let resolved_text = self.format_variables(&resolved_variables);
        let resolved_content = Node::Text(TextNode::new(resolved_text, crate::chtl::node::TextType::Literal));
        
        Ok(TemplateInstance {
            template_name: template.name.clone(),
            parameters,
            resolved_content,
        })
    }
    
    /// Validate template parameters
    fn validate_parameters(&self, expected: &[TemplateParameter], provided: &HashMap<String, CSSValue>) -> Result<(), TemplateError> {
        for param in expected {
            if param.required && !provided.contains_key(&param.name) {
                return Err(TemplateError::MissingRequiredParameter(param.name.clone()));
            }
        }
        
        for (key, value) in provided {
            if let Some(param) = expected.iter().find(|p| &p.name == key) {
                if !self.validate_parameter_type(value, &param.param_type) {
                    return Err(TemplateError::InvalidParameterType(key.clone(), param.param_type.clone()));
                }
            } else {
                return Err(TemplateError::UnknownParameter(key.clone()));
            }
        }
        
        Ok(())
    }
    
    /// Validate parameter type
    fn validate_parameter_type(&self, value: &CSSValue, expected_type: &ParameterType) -> bool {
        match (value, expected_type) {
            (CSSValue::Literal(s), ParameterType::String) => true,
            (CSSValue::Literal(s), ParameterType::Number) => s.parse::<f64>().is_ok(),
            (CSSValue::Literal(s), ParameterType::Color) => self.is_color(s),
            (CSSValue::Literal(s), ParameterType::Size) => self.is_size(s),
            (CSSValue::Literal(s), ParameterType::Boolean) => s == "true" || s == "false",
            (CSSValue::Literal(_), ParameterType::CSSValue) => true,
            (CSSValue::Variable(_), _) => true,
            (CSSValue::Expression(_), _) => true,
            (CSSValue::Function(_, _), _) => true,
        }
    }
    
    /// Check if a string is a valid color
    fn is_color(&self, s: &str) -> bool {
        s.starts_with('#') || s.starts_with("rgb") || s.starts_with("hsl") || 
        ["red", "blue", "green", "black", "white", "transparent"].contains(&s)
    }
    
    /// Check if a string is a valid size
    fn is_size(&self, s: &str) -> bool {
        s.ends_with("px") || s.ends_with("em") || s.ends_with("rem") || 
        s.ends_with("%") || s.ends_with("vh") || s.ends_with("vw")
    }
    
    /// Resolve style variables
    fn resolve_style_variables(&self, style: &mut StyleBlock, variables: &HashMap<String, CSSValue>) {
        for (_, value) in &mut style.properties {
            *value = self.resolve_css_value(value, variables);
        }
        
        for rule in &mut style.context_rules {
            for (_, value) in &mut rule.properties {
                *value = self.resolve_css_value(value, variables);
            }
        }
        
        for condition in &mut style.attribute_conditions {
            for (_, value) in &mut condition.properties {
                *value = self.resolve_css_value(value, variables);
            }
        }
    }
    
    /// Resolve element variables
    fn resolve_element_variables(&self, element: &mut ElementNode, variables: &HashMap<String, CSSValue>) {
        // Resolve element attributes
        for (_, value) in element.attributes.iter_mut() {
            *value = AttributeValue::String(self.resolve_string_variables(&value.to_string(), variables));
        }
        
        // Recursively resolve child elements
        for child in &mut element.children {
            match child {
                Node::Element(child_element) => {
                    self.resolve_element_variables(child_element, variables);
                }
                Node::Text(text_node) => {
                    text_node.content = self.resolve_string_variables(&text_node.content, variables);
                }
                _ => {}
            }
        }
    }
    
    /// Resolve CSS value variables
    fn resolve_css_value(&self, value: &CSSValue, variables: &HashMap<String, CSSValue>) -> CSSValue {
        match value {
            CSSValue::Variable(var_name) => {
                variables.get(var_name).cloned().unwrap_or(value.clone())
            }
            CSSValue::Function(name, args) => {
                let resolved_args: Vec<String> = args.iter()
                    .map(|arg| self.resolve_string_variables(arg, variables))
                    .collect();
                CSSValue::Function(name.clone(), resolved_args)
            }
            _ => value.clone(),
        }
    }
    
    /// Resolve string variables
    fn resolve_string_variables(&self, s: &str, variables: &HashMap<String, CSSValue>) -> String {
        let mut result = s.to_string();
        
        for (var_name, var_value) in variables {
            let placeholder = format!("${{{}}}", var_name);
            let value_str = match var_value {
                CSSValue::Literal(s) => s.clone(),
                CSSValue::Variable(s) => s.clone(),
                CSSValue::Expression(s) => s.clone(),
                CSSValue::Function(name, args) => format!("{}({})", name, args.join(", ")),
            };
            result = result.replace(&placeholder, &value_str);
        }
        
        result
    }
    
    /// Format variables as text
    fn format_variables(&self, variables: &HashMap<String, CSSValue>) -> String {
        let mut result = String::new();
        
        for (name, value) in variables {
            let value_str = match value {
                CSSValue::Literal(s) => s.clone(),
                CSSValue::Variable(s) => s.clone(),
                CSSValue::Expression(s) => s.clone(),
                CSSValue::Function(name, args) => format!("{}({})", name, args.join(", ")),
            };
            result.push_str(&format!("--{}: {};\n", name, value_str));
        }
        
        result
    }
}

/// Template error types
#[derive(Debug, thiserror::Error)]
pub enum TemplateError {
    #[error("Template not found: {0}")]
    TemplateNotFound(String),
    
    #[error("Missing required parameter: {0}")]
    MissingRequiredParameter(String),
    
    #[error("Unknown parameter: {0}")]
    UnknownParameter(String),
    
    #[error("Invalid parameter type for '{0}': expected {1:?}")]
    InvalidParameterType(String, ParameterType),
    
    #[error("Template compilation error: {0}")]
    CompilationError(String),
}

impl Default for TemplateSystem {
    fn default() -> Self {
        Self::new()
    }
}