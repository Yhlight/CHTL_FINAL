//! CHTL code generator
//! 
//! This module provides code generation for CHTL,
//! converting AST nodes into HTML, CSS, and JavaScript code.

use std::collections::HashMap;
use crate::chtl::node::*;
use crate::chtl::style::{StyleBlock, CSSValue};
use crate::chtl::template::TemplateSystem;

/// Code generation result
#[derive(Debug, Clone, PartialEq)]
pub struct GeneratedCode {
    /// Generated HTML code
    pub html: String,
    /// Generated CSS code
    pub css: String,
    /// Generated JavaScript code
    pub javascript: String,
    /// Generated source map (optional)
    pub source_map: Option<String>,
}

/// Code generator for CHTL
#[derive(Debug, Clone)]
pub struct CodeGenerator {
    /// Template system for resolving templates
    template_system: TemplateSystem,
    /// Style system for managing styles
    style_system: crate::chtl::style::StyleSystem,
    /// Generation options
    options: GenerationOptions,
    /// Generated CSS class counter
    css_class_counter: u32,
    /// Generated ID counter
    css_id_counter: u32,
}

/// Code generation options
#[derive(Debug, Clone, PartialEq)]
pub struct GenerationOptions {
    /// Whether to minify output
    pub minify: bool,
    /// Whether to include source maps
    pub include_source_maps: bool,
    /// Whether to include comments
    pub include_comments: bool,
    /// CSS output format
    pub css_format: CSSFormat,
    /// HTML output format
    pub html_format: HTMLFormat,
}

/// CSS output format
#[derive(Debug, Clone, PartialEq)]
pub enum CSSFormat {
    /// Standard CSS
    Standard,
    /// SCSS/Sass
    SCSS,
    /// Less
    Less,
}

/// HTML output format
#[derive(Debug, Clone, PartialEq)]
pub enum HTMLFormat {
    /// Standard HTML
    Standard,
    /// XHTML
    XHTML,
    /// HTML5
    HTML5,
}

impl CodeGenerator {
    /// Create a new code generator
    pub fn new() -> Self {
        Self {
            template_system: TemplateSystem::new(),
            style_system: crate::chtl::style::StyleSystem::new(),
            options: GenerationOptions::default(),
            css_class_counter: 0,
            css_id_counter: 0,
        }
    }
    
    /// Create a new code generator with options
    pub fn with_options(options: GenerationOptions) -> Self {
        Self {
            template_system: TemplateSystem::new(),
            style_system: crate::chtl::style::StyleSystem::new(),
            options,
            css_class_counter: 0,
            css_id_counter: 0,
        }
    }
    
    /// Generate code from a document AST
    pub fn generate(&mut self, document: &Document) -> Result<GeneratedCode, GenerationError> {
        let mut html = String::new();
        let mut css = String::new();
        let mut javascript = String::new();
        
        // Generate HTML
        html = self.generate_html(document)?;
        
        // Generate CSS
        css = self.generate_css(document)?;
        
        // Generate JavaScript
        javascript = self.generate_javascript(document)?;
        
        Ok(GeneratedCode {
            html,
            css,
            javascript,
            source_map: None, // TODO: Implement source map generation
        })
    }
    
    /// Generate HTML from document
    fn generate_html(&mut self, document: &Document) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        // Add DOCTYPE if HTML5 format
        if self.options.html_format == HTMLFormat::HTML5 {
            html.push_str("<!DOCTYPE html>\n");
        }
        
        // Generate HTML for each child node
        for child in &document.children {
            html.push_str(&self.generate_node_html(child)?);
            html.push('\n');
        }
        
        Ok(html)
    }
    
    /// Generate CSS from document
    fn generate_css(&mut self, document: &Document) -> Result<String, GenerationError> {
        let mut css = String::new();
        
        // Generate CSS for each child node
        for child in &document.children {
            css.push_str(&self.generate_node_css(child)?);
        }
        
        // Add global styles from style system
        css.push_str(&self.style_system.to_css());
        
        Ok(css)
    }
    
    /// Generate JavaScript from document
    fn generate_javascript(&mut self, document: &Document) -> Result<String, GenerationError> {
        let mut javascript = String::new();
        
        // Generate JavaScript for each child node
        for child in &document.children {
            javascript.push_str(&self.generate_node_javascript(child)?);
        }
        
        Ok(javascript)
    }
    
    /// Generate HTML for a single node
    fn generate_node_html(&mut self, node: &Node) -> Result<String, GenerationError> {
        match node {
            Node::Element(element) => self.generate_element_html(element),
            Node::Text(text) => self.generate_text_html(text),
            Node::Style(style) => self.generate_style_html(style),
            Node::Template(template) => self.generate_template_html(template),
            Node::Custom(custom) => self.generate_custom_html(custom),
            Node::Import(import) => self.generate_import_html(import),
            Node::Namespace(namespace) => self.generate_namespace_html(namespace),
            Node::Configuration(config) => self.generate_configuration_html(config),
            Node::Origin(origin) => self.generate_origin_html(origin),
            Node::Comment(comment) => self.generate_comment_html(comment),
            Node::Script(script) => self.generate_script_html(script),
        }
    }
    
    /// Generate CSS for a single node
    fn generate_node_css(&mut self, node: &Node) -> Result<String, GenerationError> {
        match node {
            Node::Element(element) => self.generate_element_css(element),
            Node::Style(style) => self.generate_style_css(style),
            Node::Template(template) => self.generate_template_css(template),
            Node::Custom(custom) => self.generate_custom_css(custom),
            _ => Ok(String::new()),
        }
    }
    
    /// Generate JavaScript for a single node
    fn generate_node_javascript(&mut self, node: &Node) -> Result<String, GenerationError> {
        match node {
            Node::Element(element) => self.generate_element_javascript(element),
            Node::Script(script) => self.generate_script_javascript(script),
            Node::Template(template) => self.generate_template_javascript(template),
            Node::Custom(custom) => self.generate_custom_javascript(custom),
            _ => Ok(String::new()),
        }
    }
    
    /// Generate HTML for an element node
    fn generate_element_html(&mut self, element: &ElementNode) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        // Generate opening tag
        html.push('<');
        html.push_str(&element.tag);
        
        // Generate attributes
        for (name, value) in &element.attributes {
            html.push(' ');
            html.push_str(name);
            html.push('=');
            html.push('"');
            html.push_str(&value.to_string());
            html.push('"');
        }
        
        // Handle self-closing tags
        if element.self_closing {
            if self.options.html_format == HTMLFormat::XHTML {
                html.push_str(" />");
            } else {
                html.push_str(">");
            }
            return Ok(html);
        }
        
        html.push('>');
        
        // Generate children
        for child in &element.children {
            html.push_str(&self.generate_node_html(child)?);
        }
        
        // Generate closing tag
        html.push_str("</");
        html.push_str(&element.tag);
        html.push('>');
        
        Ok(html)
    }
    
    /// Generate HTML for a text node
    fn generate_text_html(&self, text: &TextNode) -> Result<String, GenerationError> {
        Ok(self.escape_html(&text.content))
    }
    
    /// Generate HTML for a style node
    fn generate_style_html(&self, _style: &StyleNode) -> Result<String, GenerationError> {
        // Style nodes don't generate HTML directly
        Ok(String::new())
    }
    
    /// Generate HTML for a template node
    fn generate_template_html(&mut self, template: &TemplateNode) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        // Generate HTML for template content
        for child in &template.content {
            html.push_str(&self.generate_node_html(child)?);
        }
        
        Ok(html)
    }
    
    /// Generate HTML for a custom node
    fn generate_custom_html(&mut self, custom: &CustomNode) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        // Generate HTML for custom modifications
        for modification in &custom.modifications {
            match modification {
                CustomModification::AddElement(element) => {
                    html.push_str(&self.generate_element_html(element)?);
                }
                CustomModification::AddStyle(_style) => {
                    // Styles don't generate HTML directly
                }
                CustomModification::AddScript(script) => {
                    html.push_str(&self.generate_script_html(script)?);
                }
            }
        }
        
        Ok(html)
    }
    
    /// Generate HTML for an import node
    fn generate_import_html(&self, _import: &ImportNode) -> Result<String, GenerationError> {
        // Import nodes don't generate HTML directly
        Ok(String::new())
    }
    
    /// Generate HTML for a namespace node
    fn generate_namespace_html(&mut self, namespace: &NamespaceNode) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        // Generate HTML for namespace content
        for child in &namespace.content {
            html.push_str(&self.generate_node_html(child)?);
        }
        
        Ok(html)
    }
    
    /// Generate HTML for a configuration node
    fn generate_configuration_html(&self, _config: &ConfigurationNode) -> Result<String, GenerationError> {
        // Configuration nodes don't generate HTML directly
        Ok(String::new())
    }
    
    /// Generate HTML for an origin node
    fn generate_origin_html(&self, origin: &OriginNode) -> Result<String, GenerationError> {
        match origin.origin_type {
            OriginType::HTML => Ok(origin.content.clone()),
            _ => Ok(String::new()),
        }
    }
    
    /// Generate HTML for a comment node
    fn generate_comment_html(&self, comment: &CommentNode) -> Result<String, GenerationError> {
        if self.options.include_comments {
            Ok(format!("<!-- {} -->", comment.content))
        } else {
            Ok(String::new())
        }
    }
    
    /// Generate HTML for a script node
    fn generate_script_html(&self, script: &ScriptNode) -> Result<String, GenerationError> {
        let mut html = String::new();
        
        html.push_str("<script");
        
        // Add script attributes
        for (name, value) in &script.attributes {
            html.push(' ');
            html.push_str(name);
            html.push('=');
            html.push('"');
            html.push_str(&value.to_string());
            html.push('"');
        }
        
        html.push_str(">");
        html.push_str(&script.content);
        html.push_str("</script>");
        
        Ok(html)
    }
    
    /// Generate CSS for an element node
    fn generate_element_css(&mut self, element: &ElementNode) -> Result<String, GenerationError> {
        let mut css = String::new();
        
        // Generate CSS for element children
        for child in &element.children {
            css.push_str(&self.generate_node_css(child)?);
        }
        
        Ok(css)
    }
    
    /// Generate CSS for a style node
    fn generate_style_css(&self, style: &StyleNode) -> Result<String, GenerationError> {
        let mut css = String::new();
        
        // Generate selector
        css.push_str(&self.generate_selector(&style.selector)?);
        css.push_str(" {\n");
        
        // Generate properties
        for property in &style.properties {
            css.push_str("  ");
            css.push_str(&property.name);
            css.push_str(": ");
            css.push_str(&property.value.to_string());
            css.push_str(";\n");
        }
        
        css.push_str("}\n");
        
        // Generate nested styles
        for nested in &style.nested {
            css.push_str(&self.generate_style_css(nested)?);
        }
        
        Ok(css)
    }
    
    /// Generate CSS for a template node
    fn generate_template_css(&mut self, template: &TemplateNode) -> Result<String, GenerationError> {
        let mut css = String::new();
        
        // Generate CSS for template content
        for child in &template.content {
            css.push_str(&self.generate_node_css(child)?);
        }
        
        Ok(css)
    }
    
    /// Generate CSS for a custom node
    fn generate_custom_css(&mut self, custom: &CustomNode) -> Result<String, GenerationError> {
        let mut css = String::new();
        
        // Generate CSS for custom modifications
        for modification in &custom.modifications {
            match modification {
                CustomModification::AddStyle(style) => {
                    css.push_str(&self.generate_style_css(style)?);
                }
                _ => {}
            }
        }
        
        Ok(css)
    }
    
    /// Generate JavaScript for an element node
    fn generate_element_javascript(&mut self, element: &ElementNode) -> Result<String, GenerationError> {
        let mut javascript = String::new();
        
        // Generate JavaScript for element children
        for child in &element.children {
            javascript.push_str(&self.generate_node_javascript(child)?);
        }
        
        Ok(javascript)
    }
    
    /// Generate JavaScript for a script node
    fn generate_script_javascript(&self, script: &ScriptNode) -> Result<String, GenerationError> {
        Ok(script.content.clone())
    }
    
    /// Generate JavaScript for a template node
    fn generate_template_javascript(&mut self, template: &TemplateNode) -> Result<String, GenerationError> {
        let mut javascript = String::new();
        
        // Generate JavaScript for template content
        for child in &template.content {
            javascript.push_str(&self.generate_node_javascript(child)?);
        }
        
        Ok(javascript)
    }
    
    /// Generate JavaScript for a custom node
    fn generate_custom_javascript(&mut self, custom: &CustomNode) -> Result<String, GenerationError> {
        let mut javascript = String::new();
        
        // Generate JavaScript for custom modifications
        for modification in &custom.modifications {
            match modification {
                CustomModification::AddScript(script) => {
                    javascript.push_str(&self.generate_script_javascript(script)?);
                }
                _ => {}
            }
        }
        
        Ok(javascript)
    }
    
    /// Generate CSS selector
    fn generate_selector(&self, selector: &StyleSelector) -> Result<String, GenerationError> {
        match selector {
            StyleSelector::Class(name) => Ok(format!(".{}", name)),
            StyleSelector::Id(name) => Ok(format!("#{}", name)),
            StyleSelector::Element(name) => Ok(name.clone()),
            StyleSelector::Attribute(attr) => Ok(format!("[{}]", attr)),
            StyleSelector::PseudoClass(name) => Ok(format!(":{}", name)),
            StyleSelector::PseudoElement(name) => Ok(format!("::{}", name)),
            StyleSelector::Universal => Ok("*".to_string()),
        }
    }
    
    /// Escape HTML special characters
    fn escape_html(&self, text: &str) -> String {
        text.chars()
            .map(|c| match c {
                '<' => "&lt;",
                '>' => "&gt;",
                '&' => "&amp;",
                '"' => "&quot;",
                '\'' => "&#39;",
                _ => &c.to_string(),
            })
            .collect::<Vec<&str>>()
            .join("")
    }
    
    /// Generate a unique CSS class name
    fn generate_css_class(&mut self) -> String {
        self.css_class_counter += 1;
        format!("chtl-class-{}", self.css_class_counter)
    }
    
    /// Generate a unique CSS ID
    fn generate_css_id(&mut self) -> String {
        self.css_id_counter += 1;
        format!("chtl-id-{}", self.css_id_counter)
    }
}

/// Code generation error
#[derive(Debug, thiserror::Error)]
pub enum GenerationError {
    #[error("Template resolution error: {0}")]
    TemplateResolution(String),
    
    #[error("Style generation error: {0}")]
    StyleGeneration(String),
    
    #[error("JavaScript generation error: {0}")]
    JavaScriptGeneration(String),
    
    #[error("HTML generation error: {0}")]
    HTMLGeneration(String),
    
    #[error("Invalid node type: {0}")]
    InvalidNodeType(String),
}

impl Default for CodeGenerator {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for GenerationOptions {
    fn default() -> Self {
        Self {
            minify: false,
            include_source_maps: false,
            include_comments: true,
            css_format: CSSFormat::Standard,
            html_format: HTMLFormat::HTML5,
        }
    }
}