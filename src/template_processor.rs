//! Advanced template processing for CHTL

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use crate::style_processor::StyleExpression;
use std::collections::HashMap;

/// Template processor for handling complex template features
pub struct TemplateProcessor {
    templates: HashMap<String, TemplateDefinition>,
    custom_definitions: HashMap<String, CustomDefinition>,
    variable_groups: HashMap<String, VariableGroup>,
}

/// Template definition with full metadata
#[derive(Debug, Clone)]
pub struct TemplateDefinition {
    pub name: String,
    pub template_type: TemplateType,
    pub content: Vec<AstNode>,
    pub parameters: Vec<TemplateParameter>,
    pub dependencies: Vec<String>,
    pub metadata: TemplateMetadata,
}

/// Template parameter
#[derive(Debug, Clone)]
pub struct TemplateParameter {
    pub name: String,
    pub parameter_type: ParameterType,
    pub default_value: Option<String>,
    pub is_required: bool,
    pub description: Option<String>,
}

/// Parameter types
#[derive(Debug, Clone, PartialEq)]
pub enum ParameterType {
    String,
    Number,
    Boolean,
    Color,
    Length,
    Percentage,
    Custom(String),
}

/// Template metadata
#[derive(Debug, Clone)]
pub struct TemplateMetadata {
    pub author: Option<String>,
    pub version: String,
    pub description: Option<String>,
    pub tags: Vec<String>,
    pub created_at: String,
    pub updated_at: String,
}

/// Custom definition with full metadata
#[derive(Debug, Clone)]
pub struct CustomDefinition {
    pub name: String,
    pub custom_type: CustomType,
    pub content: Vec<AstNode>,
    pub specializations: Vec<Specialization>,
    pub parameters: Vec<TemplateParameter>,
    pub dependencies: Vec<String>,
    pub metadata: TemplateMetadata,
}

/// Variable group definition
#[derive(Debug, Clone)]
pub struct VariableGroup {
    pub name: String,
    pub variables: HashMap<String, VariableDefinition>,
    pub metadata: TemplateMetadata,
}

/// Variable definition
#[derive(Debug, Clone)]
pub struct VariableDefinition {
    pub name: String,
    pub value: String,
    pub variable_type: VariableType,
    pub is_required: bool,
    pub description: Option<String>,
}

/// Variable types
#[derive(Debug, Clone, PartialEq)]
pub enum VariableType {
    String,
    Number,
    Color,
    Length,
    Percentage,
    Expression,
    Custom(String),
}

impl TemplateProcessor {
    pub fn new() -> Self {
        Self {
            templates: HashMap::new(),
            custom_definitions: HashMap::new(),
            variable_groups: HashMap::new(),
        }
    }
    
    /// Register a template
    pub fn register_template(&mut self, template: TemplateNode) -> Result<()> {
        let definition = self.process_template_node(template)?;
        self.templates.insert(definition.name.clone(), definition);
        Ok(())
    }
    
    /// Register a custom definition
    pub fn register_custom(&mut self, custom: CustomNode) -> Result<()> {
        let definition = self.process_custom_node(custom)?;
        self.custom_definitions.insert(definition.name.clone(), definition);
        Ok(())
    }
    
    /// Register a variable group
    pub fn register_variable_group(&mut self, name: String, variables: HashMap<String, String>) -> Result<()> {
        let mut var_definitions = HashMap::new();
        for (var_name, value) in variables {
            let var_def = VariableDefinition {
                name: var_name.clone(),
                value: value.clone(),
                variable_type: self.infer_variable_type(&value),
                is_required: true,
                description: None,
            };
            var_definitions.insert(var_name, var_def);
        }
        
        let group = VariableGroup {
            name: name.clone(),
            variables: var_definitions,
            metadata: TemplateMetadata {
                author: None,
                version: "1.0.0".to_string(),
                description: None,
                tags: vec![],
                created_at: chrono::Utc::now().to_rfc3339(),
                updated_at: chrono::Utc::now().to_rfc3339(),
            },
        };
        
        self.variable_groups.insert(name, group);
        Ok(())
    }
    
    /// Process template node into definition
    fn process_template_node(&self, template: TemplateNode) -> Result<TemplateDefinition> {
        let parameters = self.extract_parameters(&template.content)?;
        let dependencies = self.extract_dependencies(&template.content)?;
        
        Ok(TemplateDefinition {
            name: template.name.clone(),
            template_type: template.template_type.clone(),
            content: template.content,
            parameters,
            dependencies,
            metadata: TemplateMetadata {
                author: None,
                version: "1.0.0".to_string(),
                description: None,
                tags: vec![],
                created_at: chrono::Utc::now().to_rfc3339(),
                updated_at: chrono::Utc::now().to_rfc3339(),
            },
        })
    }
    
    /// Process custom node into definition
    fn process_custom_node(&self, custom: CustomNode) -> Result<CustomDefinition> {
        let parameters = self.extract_parameters(&custom.content)?;
        let dependencies = self.extract_dependencies(&custom.content)?;
        
        Ok(CustomDefinition {
            name: custom.name.clone(),
            custom_type: custom.custom_type.clone(),
            content: custom.content,
            specializations: custom.specializations,
            parameters,
            dependencies,
            metadata: TemplateMetadata {
                author: None,
                version: "1.0.0".to_string(),
                description: None,
                tags: vec![],
                created_at: chrono::Utc::now().to_rfc3339(),
                updated_at: chrono::Utc::now().to_rfc3339(),
            },
        })
    }
    
    /// Extract parameters from content
    fn extract_parameters(&self, content: &[AstNode]) -> Result<Vec<TemplateParameter>> {
        let mut parameters = Vec::new();
        
        for node in content {
            self.extract_parameters_from_node(node, &mut parameters)?;
        }
        
        Ok(parameters)
    }
    
    /// Extract parameters from a single node
    fn extract_parameters_from_node(&self, node: &AstNode, parameters: &mut Vec<TemplateParameter>) -> Result<()> {
        match node {
            AstNode::Element(element) => {
                // Check for parameter attributes
                for (attr_name, attr_value) in &element.attributes {
                    if attr_name.starts_with("param-") {
                        let param_name = attr_name.strip_prefix("param-").unwrap();
                        let param_type = self.infer_parameter_type(attr_value);
                        let is_required = element.attributes.contains_key(&format!("param-{}-required", param_name));
                        let default_value = element.attributes.get(&format!("param-{}-default", param_name)).cloned();
                        let description = element.attributes.get(&format!("param-{}-description", param_name)).cloned();
                        
                        parameters.push(TemplateParameter {
                            name: param_name.to_string(),
                            parameter_type: param_type,
                            default_value,
                            is_required,
                            description,
                        });
                    }
                }
                
                // Recursively process children
                for child in &element.children {
                    self.extract_parameters_from_node(child, parameters)?;
                }
            }
            _ => {
                // Recursively process children
                for child in node.children() {
                    self.extract_parameters_from_node(child, parameters)?;
                }
            }
        }
        
        Ok(())
    }
    
    /// Extract dependencies from content
    fn extract_dependencies(&self, content: &[AstNode]) -> Result<Vec<String>> {
        let mut dependencies = Vec::new();
        
        for node in content {
            self.extract_dependencies_from_node(node, &mut dependencies)?;
        }
        
        Ok(dependencies)
    }
    
    /// Extract dependencies from a single node
    fn extract_dependencies_from_node(&self, node: &AstNode, dependencies: &mut Vec<String>) -> Result<()> {
        match node {
            AstNode::Template(template) => {
                dependencies.push(template.name.clone());
            }
            AstNode::Custom(custom) => {
                dependencies.push(custom.name.clone());
            }
            AstNode::Import(import) => {
                dependencies.push(import.path.clone());
            }
            _ => {
                // Recursively process children
                for child in node.children() {
                    self.extract_dependencies_from_node(child, dependencies)?;
                }
            }
        }
        
        Ok(())
    }
    
    /// Infer parameter type from value
    fn infer_parameter_type(&self, value: &str) -> ParameterType {
        if value.starts_with('#') || value.starts_with("rgb") || value.starts_with("hsl") {
            ParameterType::Color
        } else if value.ends_with("px") || value.ends_with("em") || value.ends_with("rem") || 
                  value.ends_with("%") || value.ends_with("vh") || value.ends_with("vw") {
            ParameterType::Length
        } else if value.ends_with('%') {
            ParameterType::Percentage
        } else if value == "true" || value == "false" {
            ParameterType::Boolean
        } else if value.parse::<f64>().is_ok() {
            ParameterType::Number
        } else {
            ParameterType::String
        }
    }
    
    /// Infer variable type from value
    fn infer_variable_type(&self, value: &str) -> VariableType {
        if value.starts_with('#') || value.starts_with("rgb") || value.starts_with("hsl") {
            VariableType::Color
        } else if value.ends_with("px") || value.ends_with("em") || value.ends_with("rem") || 
                  value.ends_with("%") || value.ends_with("vh") || value.ends_with("vw") {
            VariableType::Length
        } else if value.ends_with('%') {
            VariableType::Percentage
        } else if value.contains('(') && value.contains(')') {
            VariableType::Expression
        } else if value.parse::<f64>().is_ok() {
            VariableType::Number
        } else {
            VariableType::String
        }
    }
    
    /// Resolve template with parameters
    pub fn resolve_template(&self, name: &str, parameters: HashMap<String, String>) -> Result<Vec<AstNode>> {
        let template = self.templates.get(name)
            .ok_or_else(|| ChtlError::semantic(&format!("Template '{}' not found", name)))?;
        
        self.resolve_template_with_params(template, parameters)
    }
    
    /// Resolve template with parameters
    fn resolve_template_with_params(&self, template: &TemplateDefinition, parameters: HashMap<String, String>) -> Result<Vec<AstNode>> {
        let mut resolved_content = Vec::new();
        
        for node in &template.content {
            let resolved_node = self.resolve_node(node, &parameters)?;
            resolved_content.push(resolved_node);
        }
        
        Ok(resolved_content)
    }
    
    /// Resolve a single node with parameters
    fn resolve_node(&self, node: &AstNode, parameters: &HashMap<String, String>) -> Result<AstNode> {
        match node {
            AstNode::Element(element) => {
                let mut resolved_attributes = HashMap::new();
                for (attr_name, attr_value) in &element.attributes {
                    let resolved_value = self.resolve_parameter_value(attr_value, parameters)?;
                    resolved_attributes.insert(attr_name.clone(), resolved_value);
                }
                
                let mut resolved_children = Vec::new();
                for child in &element.children {
                    let resolved_child = self.resolve_node(child, parameters)?;
                    resolved_children.push(resolved_child);
                }
                
                let resolved_style = if let Some(style) = &element.style {
                    Some(self.resolve_style(style, parameters)?)
                } else {
                    None
                };
                
                let resolved_script = if let Some(script) = &element.script {
                    Some(self.resolve_script(script, parameters)?)
                } else {
                    None
                };
                
                Ok(AstNode::Element(ElementNode {
                    tag_name: element.tag_name.clone(),
                    attributes: resolved_attributes,
                    children: resolved_children,
                    style: resolved_style,
                    script: resolved_script,
                }))
            }
            AstNode::Text(text) => {
                let resolved_content = self.resolve_parameter_value(&text.content, parameters)?;
                Ok(AstNode::Text(TextNode {
                    content: resolved_content,
                    is_literal: text.is_literal,
                }))
            }
            AstNode::Template(template) => {
                let resolved_content = self.resolve_template(&template.name, parameters.clone())?;
                Ok(AstNode::Template(TemplateNode {
                    template_type: template.template_type.clone(),
                    name: template.name.clone(),
                    content: resolved_content,
                    parameters: template.parameters.clone(),
                }))
            }
            AstNode::Custom(custom) => {
                let resolved_content = self.resolve_custom(custom, parameters)?;
                Ok(resolved_content)
            }
            _ => Ok(node.clone()),
        }
    }
    
    /// Resolve custom node
    fn resolve_custom(&self, custom: &CustomNode, parameters: &HashMap<String, String>) -> Result<AstNode> {
        let definition = self.custom_definitions.get(&custom.name)
            .ok_or_else(|| ChtlError::semantic(&format!("Custom definition '{}' not found", custom.name)))?;
        
        let mut resolved_content = Vec::new();
        for node in &definition.content {
            let resolved_node = self.resolve_node(node, parameters)?;
            resolved_content.push(resolved_node);
        }
        
        // Apply specializations
        let mut specialized_content = resolved_content;
        for specialization in &custom.specializations {
            specialized_content = self.apply_specialization(specialized_content, specialization)?;
        }
        
        Ok(AstNode::Custom(CustomNode {
            custom_type: custom.custom_type.clone(),
            name: custom.name.clone(),
            content: specialized_content,
            specializations: custom.specializations.clone(),
        }))
    }
    
    /// Apply specialization to content
    fn apply_specialization(&self, content: Vec<AstNode>, specialization: &Specialization) -> Result<Vec<AstNode>> {
        match specialization {
            Specialization::DeleteProperty(prop_name) => {
                self.delete_property_from_content(content, prop_name)
            }
            Specialization::DeleteInheritance(inheritance_name) => {
                self.delete_inheritance_from_content(content, inheritance_name)
            }
            Specialization::AddProperty(prop_name, prop_value) => {
                self.add_property_to_content(content, prop_name, prop_value)
            }
            Specialization::InsertElement(position, element) => {
                self.insert_element_to_content(content, position, element)
            }
            Specialization::DeleteElement(element_name) => {
                self.delete_element_from_content(content, element_name)
            }
        }
    }
    
    /// Delete property from content
    fn delete_property_from_content(&self, mut content: Vec<AstNode>, prop_name: &str) -> Result<Vec<AstNode>> {
        for node in &mut content {
            self.delete_property_from_node(node, prop_name)?;
        }
        Ok(content)
    }
    
    /// Delete property from a single node
    fn delete_property_from_node(&self, node: &mut AstNode, prop_name: &str) -> Result<()> {
        match node {
            AstNode::Element(element) => {
                element.attributes.remove(prop_name);
                if let Some(style) = &mut element.style {
                    style.properties.remove(prop_name);
                }
                for child in &mut element.children {
                    self.delete_property_from_node(child, prop_name)?;
                }
            }
            _ => {
                for child in node.children_mut() {
                    self.delete_property_from_node(child, prop_name)?;
                }
            }
        }
        Ok(())
    }
    
    /// Delete inheritance from content
    fn delete_inheritance_from_content(&self, mut content: Vec<AstNode>, inheritance_name: &str) -> Result<Vec<AstNode>> {
        for node in &mut content {
            self.delete_inheritance_from_node(node, inheritance_name)?;
        }
        Ok(content)
    }
    
    /// Delete inheritance from a single node
    fn delete_inheritance_from_node(&self, node: &mut AstNode, inheritance_name: &str) -> Result<()> {
        match node {
            AstNode::Element(element) => {
                element.children.retain(|child| {
                    match child {
                        AstNode::Template(template) => template.name != inheritance_name,
                        AstNode::Custom(custom) => custom.name != inheritance_name,
                        _ => true,
                    }
                });
                for child in &mut element.children {
                    self.delete_inheritance_from_node(child, inheritance_name)?;
                }
            }
            _ => {
                for child in node.children_mut() {
                    self.delete_inheritance_from_node(child, inheritance_name)?;
                }
            }
        }
        Ok(())
    }
    
    /// Add property to content
    fn add_property_to_content(&self, mut content: Vec<AstNode>, prop_name: &str, prop_value: &str) -> Result<Vec<AstNode>> {
        for node in &mut content {
            self.add_property_to_node(node, prop_name, prop_value)?;
        }
        Ok(content)
    }
    
    /// Add property to a single node
    fn add_property_to_node(&self, node: &mut AstNode, prop_name: &str, prop_value: &str) -> Result<()> {
        match node {
            AstNode::Element(element) => {
                element.attributes.insert(prop_name.to_string(), prop_value.to_string());
                for child in &mut element.children {
                    self.add_property_to_node(child, prop_name, prop_value)?;
                }
            }
            _ => {
                for child in node.children_mut() {
                    self.add_property_to_node(child, prop_name, prop_value)?;
                }
            }
        }
        Ok(())
    }
    
    /// Insert element to content
    fn insert_element_to_content(&self, mut content: Vec<AstNode>, position: &InsertPosition, element: &AstNode) -> Result<Vec<AstNode>> {
        match position {
            InsertPosition::After(target_name) => {
                self.insert_element_after(&mut content, target_name, element)?;
            }
            InsertPosition::Before(target_name) => {
                self.insert_element_before(&mut content, target_name, element)?;
            }
            InsertPosition::Replace(target_name) => {
                self.replace_element(&mut content, target_name, element)?;
            }
            InsertPosition::AtTop => {
                content.insert(0, element.clone());
            }
            InsertPosition::AtBottom => {
                content.push(element.clone());
            }
        }
        Ok(content)
    }
    
    /// Insert element after target
    fn insert_element_after(&self, content: &mut Vec<AstNode>, target_name: &str, element: &AstNode) -> Result<()> {
        for i in 0..content.len() {
            if self.is_target_element(&content[i], target_name) {
                content.insert(i + 1, element.clone());
                return Ok(());
            }
            self.insert_element_after(&mut content[i].children_mut(), target_name, element)?;
        }
        Ok(())
    }
    
    /// Insert element before target
    fn insert_element_before(&self, content: &mut Vec<AstNode>, target_name: &str, element: &AstNode) -> Result<()> {
        for i in 0..content.len() {
            if self.is_target_element(&content[i], target_name) {
                content.insert(i, element.clone());
                return Ok(());
            }
            self.insert_element_before(&mut content[i].children_mut(), target_name, element)?;
        }
        Ok(())
    }
    
    /// Replace element
    fn replace_element(&self, content: &mut Vec<AstNode>, target_name: &str, element: &AstNode) -> Result<()> {
        for i in 0..content.len() {
            if self.is_target_element(&content[i], target_name) {
                content[i] = element.clone();
                return Ok(());
            }
            self.replace_element(&mut content[i].children_mut(), target_name, element)?;
        }
        Ok(())
    }
    
    /// Delete element from content
    fn delete_element_from_content(&self, mut content: Vec<AstNode>, element_name: &str) -> Result<Vec<AstNode>> {
        content.retain(|node| {
            if self.is_target_element(node, element_name) {
                false
            } else {
                self.delete_element_from_children(node, element_name);
                true
            }
        });
        Ok(content)
    }
    
    /// Delete element from children
    fn delete_element_from_children(&self, node: &mut AstNode, element_name: &str) {
        match node {
            AstNode::Element(element) => {
                element.children.retain(|child| {
                    if self.is_target_element(child, element_name) {
                        false
                    } else {
                        self.delete_element_from_children(child, element_name);
                        true
                    }
                });
            }
            _ => {
                for child in node.children_mut() {
                    self.delete_element_from_children(child, element_name);
                }
            }
        }
    }
    
    /// Check if node is target element
    fn is_target_element(&self, node: &AstNode, target_name: &str) -> bool {
        match node {
            AstNode::Element(element) => element.tag_name == target_name,
            AstNode::Template(template) => template.name == target_name,
            AstNode::Custom(custom) => custom.name == target_name,
            _ => false,
        }
    }
    
    /// Resolve parameter value
    fn resolve_parameter_value(&self, value: &str, parameters: &HashMap<String, String>) -> Result<String> {
        let mut result = value.to_string();
        
        // Replace parameter placeholders
        for (param_name, param_value) in parameters {
            let placeholder = format!("{{{{{}}}}}", param_name);
            result = result.replace(&placeholder, param_value);
        }
        
        // Replace variable references
        for (var_name, var_group) in &self.variable_groups {
            for (var_key, var_def) in &var_group.variables {
                let placeholder = format!("{}({})", var_name, var_key);
                result = result.replace(&placeholder, &var_def.value);
            }
        }
        
        Ok(result)
    }
    
    /// Resolve style
    fn resolve_style(&self, style: &StyleNode, parameters: &HashMap<String, String>) -> Result<StyleNode> {
        let mut resolved_properties = HashMap::new();
        for (prop_name, prop_value) in &style.properties {
            let resolved_value = self.resolve_style_value(prop_value, parameters)?;
            resolved_properties.insert(prop_name.clone(), resolved_value);
        }
        
        Ok(StyleNode {
            properties: resolved_properties,
            selectors: style.selectors.clone(),
            conditions: style.conditions.clone(),
        })
    }
    
    /// Resolve style value
    fn resolve_style_value(&self, value: &StyleValue, parameters: &HashMap<String, String>) -> Result<StyleValue> {
        match value {
            StyleValue::Simple(s) => {
                let resolved = self.resolve_parameter_value(s, parameters)?;
                Ok(StyleValue::Simple(resolved))
            }
            StyleValue::Conditional(conditions) => {
                let mut resolved_conditions = Vec::new();
                for condition in conditions {
                    let resolved_condition = StyleCondition {
                        condition: self.resolve_parameter_value(&condition.condition, parameters)?,
                        value: self.resolve_parameter_value(&condition.value, parameters)?,
                        is_optional: condition.is_optional,
                        operator: condition.operator.clone(),
                    };
                    resolved_conditions.push(resolved_condition);
                }
                Ok(StyleValue::Conditional(resolved_conditions))
            }
            StyleValue::Chain(values) => {
                let mut resolved_values = Vec::new();
                for value in values {
                    let resolved = self.resolve_style_value(value, parameters)?;
                    resolved_values.push(resolved);
                }
                Ok(StyleValue::Chain(resolved_values))
            }
            StyleValue::Expression(expr) => {
                let resolved_expr = StyleExpression {
                    expression: self.resolve_parameter_value(&expr.expression, parameters)?,
                    variables: expr.variables.clone(),
                    functions: expr.functions.clone(),
                };
                Ok(StyleValue::Expression(resolved_expr))
            }
        }
    }
    
    /// Resolve script
    fn resolve_script(&self, script: &ScriptNode, parameters: &HashMap<String, String>) -> Result<ScriptNode> {
        let resolved_content = self.resolve_parameter_value(&script.content, parameters)?;
        Ok(ScriptNode {
            content: resolved_content,
            is_chtl_js: script.is_chtl_js,
        })
    }
    
    /// Get template definition
    pub fn get_template(&self, name: &str) -> Option<&TemplateDefinition> {
        self.templates.get(name)
    }
    
    /// Get custom definition
    pub fn get_custom(&self, name: &str) -> Option<&CustomDefinition> {
        self.custom_definitions.get(name)
    }
    
    /// Get variable group
    pub fn get_variable_group(&self, name: &str) -> Option<&VariableGroup> {
        self.variable_groups.get(name)
    }
    
    /// List all templates
    pub fn list_templates(&self) -> Vec<&TemplateDefinition> {
        self.templates.values().collect()
    }
    
    /// List all custom definitions
    pub fn list_custom_definitions(&self) -> Vec<&CustomDefinition> {
        self.custom_definitions.values().collect()
    }
    
    /// List all variable groups
    pub fn list_variable_groups(&self) -> Vec<&VariableGroup> {
        self.variable_groups.values().collect()
    }
}

// Helper trait for mutable children access
trait ChildrenMut {
    fn children_mut(&mut self) -> &mut Vec<AstNode>;
}

impl ChildrenMut for AstNode {
    fn children_mut(&mut self) -> &mut Vec<AstNode> {
        match self {
            AstNode::Document(doc) => &mut doc.children,
            AstNode::Element(elem) => &mut elem.children,
            AstNode::Template(tmpl) => &mut tmpl.content,
            AstNode::Custom(custom) => &mut custom.content,
            AstNode::Namespace(ns) => &mut ns.content,
            _ => {
                // For nodes without children, return a static empty vector
                static mut EMPTY: Vec<AstNode> = Vec::new();
                unsafe { &mut EMPTY }
            }
        }
    }
}