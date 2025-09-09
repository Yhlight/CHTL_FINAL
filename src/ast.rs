//! Abstract Syntax Tree nodes for CHTL

use crate::error::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Base trait for all AST nodes
pub trait AstNode: std::fmt::Debug {
    fn node_type(&self) -> &str;
    fn children(&self) -> Vec<&dyn AstNode>;
}

/// Main document node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Document {
    pub children: Vec<Box<dyn AstNode>>,
    pub configuration: Option<Configuration>,
    pub namespace: Option<Namespace>,
}

impl AstNode for Document {
    fn node_type(&self) -> &str {
        "Document"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        self.children.iter().map(|c| c.as_ref()).collect()
    }
}

/// HTML element node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: HashMap<String, String>,
    pub children: Vec<Box<dyn AstNode>>,
    pub style: Option<StyleNode>,
    pub script: Option<ScriptNode>,
}

impl AstNode for ElementNode {
    fn node_type(&self) -> &str {
        "Element"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        let mut children: Vec<&dyn AstNode> = self.children.iter().map(|c| c.as_ref()).collect();
        if let Some(style) = &self.style {
            children.push(style);
        }
        if let Some(script) = &self.script {
            children.push(script);
        }
        children
    }
}

/// Text node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TextNode {
    pub content: String,
    pub is_literal: bool, // Whether it's an unquoted literal
}

impl AstNode for TextNode {
    fn node_type(&self) -> &str {
        "Text"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Style node (local style block)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StyleNode {
    pub properties: HashMap<String, StyleValue>,
    pub selectors: Vec<StyleSelector>,
    pub conditions: Vec<StyleCondition>,
}

impl AstNode for StyleNode {
    fn node_type(&self) -> &str {
        "Style"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Script node (local script block)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptNode {
    pub content: String,
    pub is_chtl_js: bool,
}

impl AstNode for ScriptNode {
    fn node_type(&self) -> &str {
        "Script"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Style value with support for conditions
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum StyleValue {
    Simple(String),
    Conditional(Vec<StyleCondition>),
    Chain(Vec<StyleValue>),
}

/// Style condition for conditional values
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StyleCondition {
    pub condition: String,
    pub value: String,
    pub is_optional: bool,
}

/// Style selector
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum StyleSelector {
    Class(String),
    Id(String),
    Tag(String),
    PseudoClass(String),
    PseudoElement(String),
    Contextual(String), // & selector
}

/// Template node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TemplateNode {
    pub template_type: TemplateType,
    pub name: String,
    pub content: Vec<Box<dyn AstNode>>,
    pub parameters: Vec<String>,
}

impl AstNode for TemplateNode {
    fn node_type(&self) -> &str {
        "Template"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        self.content.iter().map(|c| c.as_ref()).collect()
    }
}

/// Template types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum TemplateType {
    Style,
    Element,
    Var,
}

/// Custom node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CustomNode {
    pub custom_type: CustomType,
    pub name: String,
    pub content: Vec<Box<dyn AstNode>>,
    pub specializations: Vec<Specialization>,
}

impl AstNode for CustomNode {
    fn node_type(&self) -> &str {
        "Custom"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        self.content.iter().map(|c| c.as_ref()).collect()
    }
}

/// Custom types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum CustomType {
    Style,
    Element,
    Var,
}

/// Custom specialization
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum Specialization {
    DeleteProperty(String),
    DeleteInheritance(String),
    AddProperty(String, String),
    InsertElement(InsertPosition, Box<dyn AstNode>),
    DeleteElement(String),
}

/// Insert position for elements
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum InsertPosition {
    After(String),
    Before(String),
    Replace(String),
    AtTop,
    AtBottom,
}

/// Import node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ImportNode {
    pub import_type: ImportType,
    pub path: String,
    pub alias: Option<String>,
    pub target: Option<String>,
}

impl AstNode for ImportNode {
    fn node_type(&self) -> &str {
        "Import"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Import types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ImportType {
    Html,
    Style,
    JavaScript,
    Chtl,
    CJmod,
    Config,
}

/// Namespace node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NamespaceNode {
    pub name: String,
    pub content: Vec<Box<dyn AstNode>>,
    pub parent: Option<String>,
}

impl AstNode for NamespaceNode {
    fn node_type(&self) -> &str {
        "Namespace"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        self.content.iter().map(|c| c.as_ref()).collect()
    }
}

/// Configuration node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Configuration {
    pub settings: HashMap<String, String>,
    pub name_config: Option<NameConfiguration>,
    pub origin_types: Vec<String>,
}

/// Name configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NameConfiguration {
    pub custom_style: Vec<String>,
    pub custom_element: String,
    pub custom_var: String,
    pub template_style: String,
    pub template_element: String,
    pub template_var: String,
    pub origin_html: String,
    pub origin_style: String,
    pub origin_javascript: String,
    pub import_html: String,
    pub import_style: String,
    pub import_javascript: String,
    pub import_chtl: String,
    pub import_cjmod: String,
    pub keyword_inherit: String,
    pub keyword_delete: String,
    pub keyword_insert: String,
    pub keyword_after: String,
    pub keyword_before: String,
    pub keyword_replace: String,
    pub keyword_at_top: String,
    pub keyword_at_bottom: String,
    pub keyword_from: String,
    pub keyword_as: String,
    pub keyword_except: String,
    pub keyword_use: String,
    pub keyword_html5: String,
    pub keyword_text: String,
    pub keyword_style: String,
    pub keyword_script: String,
    pub keyword_custom: String,
    pub keyword_template: String,
    pub keyword_origin: String,
    pub keyword_import: String,
    pub keyword_namespace: String,
}

/// Origin node (raw embedding)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct OriginNode {
    pub origin_type: OriginType,
    pub name: Option<String>,
    pub content: String,
}

impl AstNode for OriginNode {
    fn node_type(&self) -> &str {
        "Origin"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Origin types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum OriginType {
    Html,
    Style,
    JavaScript,
    Custom(String),
}

/// Constraint node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConstraintNode {
    pub constraint_type: ConstraintType,
    pub targets: Vec<String>,
}

impl AstNode for ConstraintNode {
    fn node_type(&self) -> &str {
        "Constraint"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// Constraint types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConstraintType {
    Exact(Vec<String>),
    Type(String),
    Global(String),
}

/// CHTL JS specific nodes
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ChtlJsNode {
    pub js_type: ChtlJsType,
    pub content: String,
    pub parameters: HashMap<String, String>,
}

impl AstNode for ChtlJsNode {
    fn node_type(&self) -> &str {
        "ChtlJs"
    }
    
    fn children(&self) -> Vec<&dyn AstNode> {
        vec![]
    }
}

/// CHTL JS types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ChtlJsType {
    FileLoader,
    Listen,
    Delegate,
    Animate,
    VirtualObject,
    Router,
    UtilThen,
}

/// Module information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ModuleInfo {
    pub name: String,
    pub version: String,
    pub description: String,
    pub author: String,
    pub license: String,
    pub dependencies: String,
    pub category: String,
    pub min_chtl_version: String,
    pub max_chtl_version: String,
}

/// Module export
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ModuleExport {
    pub custom_styles: Vec<String>,
    pub custom_elements: Vec<String>,
    pub custom_vars: Vec<String>,
    pub template_styles: Vec<String>,
    pub template_elements: Vec<String>,
    pub template_vars: Vec<String>,
    pub origin_htmls: Vec<String>,
    pub origin_styles: Vec<String>,
    pub origin_javascripts: Vec<String>,
    pub origin_customs: Vec<String>,
    pub configurations: Vec<String>,
}