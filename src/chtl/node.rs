//! CHTL AST nodes
//! 
//! This module defines all the AST node types used in CHTL,
//! including element nodes, text nodes, style nodes, etc.

use std::collections::HashMap;
use crate::chtl::context::*;

/// Base trait for all AST nodes
pub trait CHTLNode {
    /// Get the node type
    fn node_type(&self) -> NodeType;
    
    /// Get the source location
    fn location(&self) -> &SourceLocation;
    
    /// Get children nodes
    fn children(&self) -> &[Node];
    
    /// Get mutable children nodes
    fn children_mut(&mut self) -> &mut Vec<Node>;
    
    /// Add a child node
    fn add_child(&mut self, child: Node);
    
    /// Remove a child node
    fn remove_child(&mut self, index: usize) -> Option<Node>;
    
    /// Get node attributes
    fn attributes(&self) -> &HashMap<String, String>;
    
    /// Get mutable node attributes
    fn attributes_mut(&mut self) -> &mut HashMap<String, String>;
    
    /// Set an attribute
    fn set_attribute(&mut self, key: String, value: String);
    
    /// Get an attribute
    fn get_attribute(&self, key: &str) -> Option<&String>;
    
    /// Remove an attribute
    fn remove_attribute(&mut self, key: &str) -> Option<String>;
}

/// Node type enumeration
#[derive(Debug, Clone, PartialEq)]
pub enum NodeType {
    Element,
    Text,
    Comment,
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Import,
    Config,
    Namespace,
}

/// Element node implementation
impl CHTLNode for ElementNode {
    fn node_type(&self) -> NodeType {
        NodeType::Element
    }
    
    fn location(&self) -> &SourceLocation {
        &SourceLocation {
            file: String::new(),
            line: 0,
            column: 0,
        }
    }
    
    fn children(&self) -> &[Node] {
        &self.children
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        &mut self.children
    }
    
    fn add_child(&mut self, child: Node) {
        self.children.push(child);
    }
    
    fn remove_child(&mut self, index: usize) -> Option<Node> {
        if index < self.children.len() {
            Some(self.children.remove(index))
        } else {
            None
        }
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &self.attributes
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        &mut self.attributes
    }
    
    fn set_attribute(&mut self, key: String, value: String) {
        self.attributes.insert(key, value);
    }
    
    fn get_attribute(&self, key: &str) -> Option<&String> {
        self.attributes.get(key)
    }
    
    fn remove_attribute(&mut self, key: &str) -> Option<String> {
        self.attributes.remove(key)
    }
}

/// Text node implementation
impl CHTLNode for TextNode {
    fn node_type(&self) -> NodeType {
        NodeType::Text
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        // Text nodes don't have children
        panic!("Text nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Text nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Text nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Text nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Comment node implementation
impl CHTLNode for CommentNode {
    fn node_type(&self) -> NodeType {
        NodeType::Comment
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Comment nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Comment nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Comment nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Comment nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Style node implementation
impl CHTLNode for StyleNode {
    fn node_type(&self) -> NodeType {
        NodeType::Style
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Style nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Style nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Style nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Style nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Script node implementation
impl CHTLNode for ScriptNode {
    fn node_type(&self) -> NodeType {
        NodeType::Script
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Script nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Script nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Script nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Script nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Template node implementation
impl CHTLNode for TemplateNode {
    fn node_type(&self) -> NodeType {
        NodeType::Template
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Template nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Template nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Template nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Template nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Custom node implementation
impl CHTLNode for CustomNode {
    fn node_type(&self) -> NodeType {
        NodeType::Custom
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Custom nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Custom nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Custom nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Custom nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Origin node implementation
impl CHTLNode for OriginNode {
    fn node_type(&self) -> NodeType {
        NodeType::Origin
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Origin nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Origin nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Origin nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Origin nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Import node implementation
impl CHTLNode for ImportNode {
    fn node_type(&self) -> NodeType {
        NodeType::Import
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Import nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Import nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Import nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Import nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Config node implementation
impl CHTLNode for ConfigNode {
    fn node_type(&self) -> NodeType {
        NodeType::Config
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &[]
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        panic!("Config nodes don't have children")
    }
    
    fn add_child(&mut self, _child: Node) {
        panic!("Config nodes don't have children")
    }
    
    fn remove_child(&mut self, _index: usize) -> Option<Node> {
        None
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Config nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Config nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Namespace node implementation
impl CHTLNode for NamespaceNode {
    fn node_type(&self) -> NodeType {
        NodeType::Namespace
    }
    
    fn location(&self) -> &SourceLocation {
        &self.location
    }
    
    fn children(&self) -> &[Node] {
        &self.content
    }
    
    fn children_mut(&mut self) -> &mut Vec<Node> {
        &mut self.content
    }
    
    fn add_child(&mut self, child: Node) {
        self.content.push(child);
    }
    
    fn remove_child(&mut self, index: usize) -> Option<Node> {
        if index < self.content.len() {
            Some(self.content.remove(index))
        } else {
            None
        }
    }
    
    fn attributes(&self) -> &HashMap<String, String> {
        &HashMap::new()
    }
    
    fn attributes_mut(&mut self) -> &mut HashMap<String, String> {
        panic!("Namespace nodes don't have attributes")
    }
    
    fn set_attribute(&mut self, _key: String, _value: String) {
        panic!("Namespace nodes don't have attributes")
    }
    
    fn get_attribute(&self, _key: &str) -> Option<&String> {
        None
    }
    
    fn remove_attribute(&mut self, _key: &str) -> Option<String> {
        None
    }
}

/// Node visitor trait for AST traversal
pub trait NodeVisitor {
    /// Visit a node
    fn visit_node(&mut self, node: &Node) -> Result<(), Box<dyn std::error::Error>>;
    
    /// Visit an element node
    fn visit_element(&mut self, element: &ElementNode) -> Result<(), Box<dyn std::error::Error>> {
        // Default implementation: visit children
        for child in &element.children {
            self.visit_node(child)?;
        }
        Ok(())
    }
    
    /// Visit a text node
    fn visit_text(&mut self, _text: &TextNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a comment node
    fn visit_comment(&mut self, _comment: &CommentNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a style node
    fn visit_style(&mut self, _style: &StyleNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a script node
    fn visit_script(&mut self, _script: &ScriptNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a template node
    fn visit_template(&mut self, _template: &TemplateNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a custom node
    fn visit_custom(&mut self, _custom: &CustomNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit an origin node
    fn visit_origin(&mut self, _origin: &OriginNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit an import node
    fn visit_import(&mut self, _import: &ImportNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a config node
    fn visit_config(&mut self, _config: &ConfigNode) -> Result<(), Box<dyn std::error::Error>> {
        Ok(())
    }
    
    /// Visit a namespace node
    fn visit_namespace(&mut self, namespace: &NamespaceNode) -> Result<(), Box<dyn std::error::Error>> {
        // Default implementation: visit children
        for child in &namespace.content {
            self.visit_node(child)?;
        }
        Ok(())
    }
}

/// Default node visitor implementation
pub struct DefaultNodeVisitor;

impl NodeVisitor for DefaultNodeVisitor {
    fn visit_node(&mut self, node: &Node) -> Result<(), Box<dyn std::error::Error>> {
        match node {
            Node::Element(element) => self.visit_element(element),
            Node::Text(text) => self.visit_text(text),
            Node::Comment(comment) => self.visit_comment(comment),
            Node::Style(style) => self.visit_style(style),
            Node::Script(script) => self.visit_script(script),
            Node::Template(template) => self.visit_template(template),
            Node::Custom(custom) => self.visit_custom(custom),
            Node::Origin(origin) => self.visit_origin(origin),
            Node::Import(import) => self.visit_import(import),
            Node::Config(config) => self.visit_config(config),
            Node::Namespace(namespace) => self.visit_namespace(namespace),
        }
    }
}

/// Node transformer trait for AST modification
pub trait NodeTransformer {
    /// Transform a node
    fn transform_node(&mut self, node: Node) -> Result<Node, Box<dyn std::error::Error>>;
    
    /// Transform an element node
    fn transform_element(&mut self, mut element: ElementNode) -> Result<Node, Box<dyn std::error::Error>> {
        // Transform children
        let mut new_children = Vec::new();
        for child in element.children {
            new_children.push(self.transform_node(child)?);
        }
        element.children = new_children;
        
        Ok(Node::Element(element))
    }
    
    /// Transform a text node
    fn transform_text(&mut self, text: TextNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Text(text))
    }
    
    /// Transform a comment node
    fn transform_comment(&mut self, comment: CommentNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Comment(comment))
    }
    
    /// Transform a style node
    fn transform_style(&mut self, style: StyleNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Style(style))
    }
    
    /// Transform a script node
    fn transform_script(&mut self, script: ScriptNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Script(script))
    }
    
    /// Transform a template node
    fn transform_template(&mut self, template: TemplateNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Template(template))
    }
    
    /// Transform a custom node
    fn transform_custom(&mut self, custom: CustomNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Custom(custom))
    }
    
    /// Transform an origin node
    fn transform_origin(&mut self, origin: OriginNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Origin(origin))
    }
    
    /// Transform an import node
    fn transform_import(&mut self, import: ImportNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Import(import))
    }
    
    /// Transform a config node
    fn transform_config(&mut self, config: ConfigNode) -> Result<Node, Box<dyn std::error::Error>> {
        Ok(Node::Config(config))
    }
    
    /// Transform a namespace node
    fn transform_namespace(&mut self, mut namespace: NamespaceNode) -> Result<Node, Box<dyn std::error::Error>> {
        // Transform children
        let mut new_content = Vec::new();
        for child in namespace.content {
            new_content.push(self.transform_node(child)?);
        }
        namespace.content = new_content;
        
        Ok(Node::Namespace(namespace))
    }
}

/// Default node transformer implementation
pub struct DefaultNodeTransformer;

impl NodeTransformer for DefaultNodeTransformer {
    fn transform_node(&mut self, node: Node) -> Result<Node, Box<dyn std::error::Error>> {
        match node {
            Node::Element(element) => self.transform_element(element),
            Node::Text(text) => self.transform_text(text),
            Node::Comment(comment) => self.transform_comment(comment),
            Node::Style(style) => self.transform_style(style),
            Node::Script(script) => self.transform_script(script),
            Node::Template(template) => self.transform_template(template),
            Node::Custom(custom) => self.transform_custom(custom),
            Node::Origin(origin) => self.transform_origin(origin),
            Node::Import(import) => self.transform_import(import),
            Node::Config(config) => self.transform_config(config),
            Node::Namespace(namespace) => self.transform_namespace(namespace),
        }
    }
}