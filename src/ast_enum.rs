//! Enum-based AST nodes for better type safety

use crate::error::Result;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// Main AST node enum
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstNode {
    Document(Document),
    Element(ElementNode),
    Text(TextNode),
    Style(StyleNode),
    Script(ScriptNode),
    Template(TemplateNode),
    Custom(CustomNode),
    Import(ImportNode),
    Origin(OriginNode),
    Namespace(NamespaceNode),
    Configuration(Configuration),
    Constraint(ConstraintNode),
    ChtlJs(ChtlJsNode),
}

impl AstNode {
    pub fn node_type(&self) -> &str {
        match self {
            AstNode::Document(_) => "Document",
            AstNode::Element(_) => "Element",
            AstNode::Text(_) => "Text",
            AstNode::Style(_) => "Style",
            AstNode::Script(_) => "Script",
            AstNode::Template(_) => "Template",
            AstNode::Custom(_) => "Custom",
            AstNode::Import(_) => "Import",
            AstNode::Origin(_) => "Origin",
            AstNode::Namespace(_) => "Namespace",
            AstNode::Configuration(_) => "Configuration",
            AstNode::Constraint(_) => "Constraint",
            AstNode::ChtlJs(_) => "ChtlJs",
        }
    }
    
    pub fn children(&self) -> Vec<&AstNode> {
        match self {
            AstNode::Document(doc) => doc.children.iter().collect(),
            AstNode::Element(elem) => elem.children.iter().collect(),
            AstNode::Template(tmpl) => tmpl.content.iter().collect(),
            AstNode::Custom(custom) => custom.content.iter().collect(),
            AstNode::Namespace(ns) => ns.content.iter().collect(),
            _ => vec![],
        }
    }
    
    pub fn as_element(&self) -> Option<&ElementNode> {
        match self {
            AstNode::Element(elem) => Some(elem),
            _ => None,
        }
    }
    
    pub fn as_text(&self) -> Option<&TextNode> {
        match self {
            AstNode::Text(text) => Some(text),
            _ => None,
        }
    }
    
    pub fn as_style(&self) -> Option<&StyleNode> {
        match self {
            AstNode::Style(style) => Some(style),
            _ => None,
        }
    }
    
    pub fn as_script(&self) -> Option<&ScriptNode> {
        match self {
            AstNode::Script(script) => Some(script),
            _ => None,
        }
    }
    
    pub fn as_template(&self) -> Option<&TemplateNode> {
        match self {
            AstNode::Template(tmpl) => Some(tmpl),
            _ => None,
        }
    }
    
    pub fn as_custom(&self) -> Option<&CustomNode> {
        match self {
            AstNode::Custom(custom) => Some(custom),
            _ => None,
        }
    }
    
    pub fn as_import(&self) -> Option<&ImportNode> {
        match self {
            AstNode::Import(import) => Some(import),
            _ => None,
        }
    }
    
    pub fn as_origin(&self) -> Option<&OriginNode> {
        match self {
            AstNode::Origin(origin) => Some(origin),
            _ => None,
        }
    }
    
    pub fn as_namespace(&self) -> Option<&NamespaceNode> {
        match self {
            AstNode::Namespace(ns) => Some(ns),
            _ => None,
        }
    }
    
    pub fn as_configuration(&self) -> Option<&Configuration> {
        match self {
            AstNode::Configuration(config) => Some(config),
            _ => None,
        }
    }
}

/// Main document node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Document {
    pub children: Vec<AstNode>,
    pub configuration: Option<Configuration>,
    pub namespace: Option<NamespaceNode>,
}

/// HTML element node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: HashMap<String, String>,
    pub children: Vec<AstNode>,
    pub style: Option<StyleNode>,
    pub script: Option<ScriptNode>,
}

/// Text node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TextNode {
    pub content: String,
    pub is_literal: bool, // Whether it's an unquoted literal
}

/// Style node (local style block)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StyleNode {
    pub properties: HashMap<String, StyleValue>,
    pub selectors: Vec<StyleSelector>,
    pub conditions: Vec<StyleCondition>,
}

/// Script node (local script block)
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptNode {
    pub content: String,
    pub is_chtl_js: bool,
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
    pub content: Vec<AstNode>,
    pub parameters: Vec<String>,
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
    pub content: Vec<AstNode>,
    pub specializations: Vec<Specialization>,
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
    InsertElement(InsertPosition, AstNode),
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
    pub content: Vec<AstNode>,
    pub parent: Option<String>,
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