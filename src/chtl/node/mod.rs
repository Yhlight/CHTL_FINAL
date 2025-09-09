//! CHTL AST Nodes
//! 
//! This module defines the Abstract Syntax Tree nodes for CHTL.

use serde::{Deserialize, Serialize};

/// Root document node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct DocumentNode {
    pub nodes: Vec<AstNode>,
}

/// Main AST node types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstNode {
    Element(ElementNode),
    Text(TextNode),
    Style(StyleNode),
    Script(ScriptNode),
    Template(TemplateNode),
    Custom(CustomNode),
    Origin(OriginNode),
    Import(ImportNode),
    Namespace(NamespaceNode),
    Configuration(ConfigurationNode),
    Use(UseNode),
}

/// HTML Element node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: Vec<Attribute>,
    pub children: Vec<AstNode>,
}

/// Text content node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TextNode {
    pub content: String,
}

/// Style block node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StyleNode {
    pub properties: Vec<StyleProperty>,
}

/// Script block node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScriptNode {
    pub content: String,
}

/// Template node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TemplateNode {
    pub template_type: TemplateType,
    pub name: String,
    pub content: Vec<AstNode>,
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
}

/// Custom types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum CustomType {
    Style,
    Element,
    Var,
}

/// Origin node for raw code embedding
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

/// Import node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ImportNode {
    pub import_type: ImportType,
    pub name: Option<String>,
    pub path: String,
    pub alias: Option<String>,
}

/// Import types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ImportType {
    Html,
    Style,
    JavaScript,
    Chtl,
    Cjmod,
}

/// Namespace node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct NamespaceNode {
    pub name: String,
    pub content: Vec<AstNode>,
}

/// Configuration node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConfigurationNode {
    pub name: Option<String>,
    pub settings: Vec<ConfigSetting>,
}

/// Configuration setting
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ConfigSetting {
    pub key: String,
    pub value: ConfigValue,
}

/// Configuration value types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum ConfigValue {
    String(String),
    Number(f64),
    Boolean(bool),
    Array(Vec<ConfigValue>),
    Object(Vec<ConfigSetting>),
}

/// Use node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct UseNode {
    pub use_type: UseType,
    pub name: String,
}

/// Use types
#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum UseType {
    Html5,
    Config,
}

/// HTML Attribute
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Attribute {
    pub name: String,
    pub value: String,
}

/// CSS Style Property
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StyleProperty {
    pub name: String,
    pub value: String,
}

impl AstNode {
    /// Get the node type as a string
    pub fn node_type(&self) -> &'static str {
        match self {
            AstNode::Element(_) => "Element",
            AstNode::Text(_) => "Text",
            AstNode::Style(_) => "Style",
            AstNode::Script(_) => "Script",
            AstNode::Template(_) => "Template",
            AstNode::Custom(_) => "Custom",
            AstNode::Origin(_) => "Origin",
            AstNode::Import(_) => "Import",
            AstNode::Namespace(_) => "Namespace",
            AstNode::Configuration(_) => "Configuration",
            AstNode::Use(_) => "Use",
        }
    }
    
    /// Check if this is a container node (has children)
    pub fn is_container(&self) -> bool {
        match self {
            AstNode::Element(_) | AstNode::Template(_) | AstNode::Custom(_) | AstNode::Namespace(_) => true,
            _ => false,
        }
    }
    
    /// Get children if this is a container node
    pub fn children(&self) -> Option<&Vec<AstNode>> {
        match self {
            AstNode::Element(node) => Some(&node.children),
            AstNode::Template(node) => Some(&node.content),
            AstNode::Custom(node) => Some(&node.content),
            AstNode::Namespace(node) => Some(&node.content),
            _ => None,
        }
    }
}