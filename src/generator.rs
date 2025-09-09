//! Code generation for CHTL

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use std::collections::HashMap;

/// HTML generator
pub struct HtmlGenerator {
    indent_level: usize,
    indent_string: String,
}

impl HtmlGenerator {
    pub fn new() -> Self {
        Self {
            indent_level: 0,
            indent_string: "  ".to_string(),
        }
    }
    
    pub fn generate(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Add HTML5 doctype if configured
        if let Some(config) = &document.configuration {
            if config.settings.get("HTML5_DOCTYPE").map_or(false, |v| v == "true") {
                output.push_str("<!DOCTYPE html>\n");
            }
        }
        
        // Generate HTML content
        for child in &document.children {
            output.push_str(&self.generate_node(child)?);
        }
        
        Ok(output)
    }
    
    pub fn generate_enum(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Add HTML5 doctype if configured
        if let Some(config) = &document.configuration {
            if config.settings.get("HTML5_DOCTYPE").map_or(false, |v| v == "true") {
                output.push_str("<!DOCTYPE html>\n");
            }
        }
        
        // Generate HTML content
        for child in &document.children {
            output.push_str(&self.generate_enum_node(child)?);
        }
        
        Ok(output)
    }
    
    // remove old dyn-AST helpers; we only use enum-based generation below
    
    fn generate_enum_node(&mut self, node: &AstNode) -> Result<String> {
        match node {
            AstNode::Element(elem) => self.generate_enum_element(elem),
            AstNode::Text(text) => self.generate_enum_text(text),
            AstNode::Style(style) => self.generate_enum_style(style),
            AstNode::Script(script) => self.generate_enum_script(script),
            AstNode::Template(_) => Ok(String::new()), // Templates are not directly output
            AstNode::Custom(_) => Ok(String::new()), // Custom nodes are not directly output
            AstNode::Import(_) => Ok(String::new()), // Import nodes are resolved during compilation
            AstNode::Origin(origin) => self.generate_enum_origin(origin),
            AstNode::Namespace(_) => Ok(String::new()), // Namespace nodes are not directly output
            AstNode::Configuration(_) => Ok(String::new()), // Configuration nodes are not directly output
            AstNode::Constraint(_) => Ok(String::new()), // Constraint nodes are not directly output
            AstNode::ChtlJs(_) => Ok(String::new()), // CHTL JS nodes are not directly output
            _ => Ok(String::new()),
        }
    }
    
    fn generate_enum_element(&mut self, element: &ElementNode) -> Result<String> {
        let mut output = String::new();
        
        // Add indentation
        output.push_str(&self.get_indent());
        
        // Generate opening tag
        output.push('<');
        output.push_str(&element.tag_name);
        
        // Generate attributes
        for (key, value) in &element.attributes {
            output.push(' ');
            output.push_str(key);
            output.push('=');
            output.push('"');
            output.push_str(value);
            output.push('"');
        }
        
        // Generate class and id from style selectors
        for selector in &element.style.as_ref().map_or(vec![], |s| s.selectors.clone()) {
            match selector {
                StyleSelector::Class(name) => {
                    if !element.attributes.contains_key("class") {
                        output.push_str(" class=\"");
                        output.push_str(name);
                        output.push('"');
                    }
                }
                StyleSelector::Id(name) => {
                    if !element.attributes.contains_key("id") {
                        output.push_str(" id=\"");
                        output.push_str(name);
                        output.push('"');
                    }
                }
                _ => {}
            }
        }
        
        // Check if it's a self-closing tag
        let self_closing_tags = ["img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed", "source", "track", "wbr"];
        let is_self_closing = self_closing_tags.contains(&element.tag_name.as_str()) || element.children.is_empty();
        
        if is_self_closing {
            output.push_str(" />\n");
        } else {
            output.push('>');
            
            // Generate children
            if !element.children.is_empty() {
                output.push('\n');
                self.indent_level += 1;
                
                for child in &element.children {
                    output.push_str(&self.generate_enum_node(child)?);
                }
                
                self.indent_level -= 1;
                output.push_str(&self.get_indent());
            }
            
            // Generate closing tag
            output.push('<');
            output.push('/');
            output.push_str(&element.tag_name);
            output.push('>');
            output.push('\n');
        }
        
        Ok(output)
    }
    
    fn generate_enum_text(&mut self, text: &TextNode) -> Result<String> {
        let mut output = String::new();
        output.push_str(&self.get_indent());
        output.push_str(&text.content);
        output.push('\n');
        Ok(output)
    }
    
    fn generate_enum_style(&mut self, style: &StyleNode) -> Result<String> {
        let mut output = String::new();
        output.push_str(&self.get_indent());
        output.push_str("<style>\n");
        
        self.indent_level += 1;
        
        // Generate CSS rules
        for (property, value) in &style.properties {
            output.push_str(&self.get_indent());
            output.push_str(property);
            output.push_str(": ");
            
            match value {
                StyleValue::Simple(v) => output.push_str(v),
                StyleValue::Conditional(conditions) => {
                    // TODO: Generate conditional CSS
                    output.push_str("/* conditional value */");
                }
                StyleValue::Chain(values) => {
                    // TODO: Generate chained values
                    output.push_str("/* chained values */");
                }
            }
            
            output.push_str(";\n");
        }
        
        self.indent_level -= 1;
        output.push_str(&self.get_indent());
        output.push_str("</style>\n");
        
        Ok(output)
    }
    
    fn generate_enum_script(&mut self, script: &ScriptNode) -> Result<String> {
        let mut output = String::new();
        output.push_str(&self.get_indent());
        output.push_str("<script>\n");
        
        self.indent_level += 1;
        output.push_str(&self.get_indent());
        output.push_str(&script.content);
        output.push('\n');
        
        self.indent_level -= 1;
        output.push_str(&self.get_indent());
        output.push_str("</script>\n");
        
        Ok(output)
    }
    
    fn generate_enum_origin(&mut self, origin: &OriginNode) -> Result<String> {
        // Origin nodes output their content directly
        Ok(origin.content.clone())
    }
    
    fn get_indent(&self) -> String {
        self.indent_string.repeat(self.indent_level)
    }
}

/// CSS generator
pub struct CssGenerator {
    indent_level: usize,
    indent_string: String,
}

impl CssGenerator {
    pub fn new() -> Self {
        Self {
            indent_level: 0,
            indent_string: "  ".to_string(),
        }
    }
    
    pub fn generate(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Generate global styles
        for child in &document.children {
            if let Some(style) = self.extract_global_style(child) {
                output.push_str(&self.generate_global_style(&style)?);
            }
        }
        
        Ok(output)
    }
    
    pub fn generate_enum(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Generate global styles
        for child in &document.children {
            if let Some(style) = self.extract_global_style_enum(child) {
                output.push_str(&self.generate_global_style(&style)?);
            }
        }
        
        Ok(output)
    }
    
    fn extract_global_style(&self, node: &dyn AstNode) -> Option<&StyleNode> {
        // TODO: Extract global styles from the document
        None
    }
    
    fn extract_global_style_enum(&self, node: &AstNode) -> Option<&StyleNode> {
        match node {
            AstNode::Style(style) => Some(style),
            _ => None,
        }
    }
    
    fn generate_global_style(&mut self, style: &StyleNode) -> Result<String> {
        let mut output = String::new();
        
        // Generate CSS rules
        for (property, value) in &style.properties {
            output.push_str(property);
            output.push_str(": ");
            
            match value {
                StyleValue::Simple(v) => output.push_str(v),
                StyleValue::Conditional(conditions) => {
                    // TODO: Generate conditional CSS
                    output.push_str("/* conditional value */");
                }
                StyleValue::Chain(values) => {
                    // TODO: Generate chained values
                    output.push_str("/* chained values */");
                }
            }
            
            output.push_str(";\n");
        }
        
        Ok(output)
    }
}

/// JavaScript generator
pub struct JavaScriptGenerator {
    indent_level: usize,
    indent_string: String,
}

impl JavaScriptGenerator {
    pub fn new() -> Self {
        Self {
            indent_level: 0,
            indent_string: "  ".to_string(),
        }
    }
    
    pub fn generate(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Generate JavaScript content
        for child in &document.children {
            if let Some(script) = self.extract_script(child) {
                output.push_str(&self.generate_script_content(&script)?);
            }
        }
        
        Ok(output)
    }
    
    pub fn generate_enum(&mut self, document: &Document) -> Result<String> {
        let mut output = String::new();
        
        // Generate JavaScript content
        for child in &document.children {
            if let Some(script) = self.extract_script_enum(child) {
                output.push_str(&self.generate_script_content(&script)?);
            }
        }
        
        Ok(output)
    }
    
    fn extract_script(&self, node: &dyn AstNode) -> Option<&ScriptNode> {
        // TODO: Extract script nodes from the document
        None
    }
    
    fn extract_script_enum(&self, node: &AstNode) -> Option<&ScriptNode> {
        match node {
            AstNode::Script(script) => Some(script),
            _ => None,
        }
    }
    
    fn generate_script_content(&mut self, script: &ScriptNode) -> Result<String> {
        Ok(script.content.clone())
    }
}
