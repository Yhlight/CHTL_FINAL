//! CHTL Code Generator
//! 
//! This module handles the generation of HTML, CSS, and JavaScript
//! from the CHTL Abstract Syntax Tree.

use anyhow::Result;
use crate::chtl::node::*;

/// CHTL Code Generator
pub struct ChtlGenerator {
    html_output: String,
    css_output: String,
    js_output: String,
}

impl ChtlGenerator {
    /// Create a new CHTL generator
    pub fn new() -> Self {
        Self {
            html_output: String::new(),
            css_output: String::new(),
            js_output: String::new(),
        }
    }
    
    /// Generate HTML from AST
    pub fn generate(&self, ast: Vec<AstNode>) -> Result<String> {
        let mut generator = Self::new();
        generator.generate_document(ast)?;
        
        // Combine HTML, CSS, and JS
        let mut result = generator.html_output.clone();
        
        if !generator.css_output.is_empty() {
            result.push_str(&format!("<style>\n{}\n</style>", generator.css_output));
        }
        
        if !generator.js_output.is_empty() {
            result.push_str(&format!("<script>\n{}\n</script>", generator.js_output));
        }
        
        Ok(result)
    }
    
    fn generate_document(&mut self, nodes: Vec<AstNode>) -> Result<()> {
        for node in nodes {
            self.generate_node(node)?;
        }
        Ok(())
    }
    
    fn generate_node(&mut self, node: AstNode) -> Result<()> {
        match node {
            AstNode::Element(element) => self.generate_element(element)?,
            AstNode::Text(text) => self.generate_text(text)?,
            AstNode::Style(style) => self.generate_style(style)?,
            AstNode::Script(script) => self.generate_script(script)?,
            AstNode::Template(template) => self.generate_template(template)?,
            AstNode::Custom(custom) => self.generate_custom(custom)?,
            AstNode::Origin(origin) => self.generate_origin(origin)?,
            AstNode::Import(import) => self.generate_import(import)?,
            AstNode::Namespace(namespace) => self.generate_namespace(namespace)?,
            AstNode::Configuration(config) => self.generate_configuration(config)?,
            AstNode::Use(use_node) => self.generate_use(use_node)?,
        }
        Ok(())
    }
    
    fn generate_element(&mut self, element: ElementNode) -> Result<()> {
        // Check if this is a self-closing tag
        let self_closing_tags = ["img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed", "source", "track", "wbr"];
        let is_self_closing = self_closing_tags.contains(&element.tag_name.as_str());
        
        // Generate opening tag
        self.html_output.push('<');
        self.html_output.push_str(&element.tag_name);
        
        // Generate attributes
        for attr in element.attributes {
            self.html_output.push(' ');
            self.html_output.push_str(&attr.name);
            self.html_output.push('=');
            self.html_output.push('"');
            self.html_output.push_str(&attr.value);
            self.html_output.push('"');
        }
        
        if is_self_closing {
            self.html_output.push_str(" />");
        } else {
            self.html_output.push('>');
            
            // Generate children
            for child in element.children {
                self.generate_node(child)?;
            }
            
            // Generate closing tag
            self.html_output.push_str("</");
            self.html_output.push_str(&element.tag_name);
            self.html_output.push('>');
        }
        
        Ok(())
    }
    
    fn generate_text(&mut self, text: TextNode) -> Result<()> {
        // Escape HTML special characters
        let escaped = text.content
            .replace('&', "&amp;")
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&#39;");
        
        self.html_output.push_str(&escaped);
        Ok(())
    }
    
    fn generate_style(&mut self, style: StyleNode) -> Result<()> {
        // Generate CSS properties
        for property in style.properties {
            self.css_output.push_str(&property.name);
            self.css_output.push(':');
            self.css_output.push(' ');
            self.css_output.push_str(&property.value);
            self.css_output.push(';');
            self.css_output.push('\n');
        }
        Ok(())
    }
    
    fn generate_script(&mut self, script: ScriptNode) -> Result<()> {
        self.js_output.push_str(&script.content);
        self.js_output.push('\n');
        Ok(())
    }
    
    fn generate_template(&mut self, template: TemplateNode) -> Result<()> {
        // TODO: Implement template generation
        // Templates are not directly output to HTML, they are used for code reuse
        Ok(())
    }
    
    fn generate_custom(&mut self, custom: CustomNode) -> Result<()> {
        // TODO: Implement custom generation
        // Custom nodes are expanded based on their definitions
        Ok(())
    }
    
    fn generate_origin(&mut self, origin: OriginNode) -> Result<()> {
        match origin.origin_type {
            OriginType::Html => {
                self.html_output.push_str(&origin.content);
            }
            OriginType::Style => {
                self.css_output.push_str(&origin.content);
            }
            OriginType::JavaScript => {
                self.js_output.push_str(&origin.content);
            }
            OriginType::Custom(_) => {
                // Custom origin types are handled based on their type
                self.html_output.push_str(&origin.content);
            }
        }
        Ok(())
    }
    
    fn generate_import(&mut self, import: ImportNode) -> Result<()> {
        match import.import_type {
            ImportType::Module => {
                // Generate module import
                if let Some(alias) = &import.alias {
                    self.html_output.push_str(&format!("<!-- Import module: {} as {} -->\n", import.path, alias));
                } else {
                    self.html_output.push_str(&format!("<!-- Import module: {} -->\n", import.path));
                }
            }
            ImportType::Style => {
                // Generate style import
                self.css_output.push_str(&format!("/* Import style: {} */\n", import.path));
            }
            ImportType::Script => {
                // Generate script import
                self.js_output.push_str(&format!("/* Import script: {} */\n", import.path));
            }
            ImportType::Config => {
                // Generate config import
                self.html_output.push_str(&format!("<!-- Import config: {} -->\n", import.path));
            }
            _ => {
                // Default import handling
                self.html_output.push_str(&format!("<!-- Import: {} -->\n", import.path));
            }
        }
        Ok(())
    }
    
    fn generate_namespace(&mut self, namespace: NamespaceNode) -> Result<()> {
        // Generate namespace comment
        self.html_output.push_str(&format!("<!-- Namespace: {} -->\n", namespace.name));
        
        // Generate namespace content
        for node in namespace.content {
            self.generate_node(node)?;
        }
        
        self.html_output.push_str(&format!("<!-- End namespace: {} -->\n", namespace.name));
        Ok(())
    }
    
    fn generate_configuration(&mut self, config: ConfigurationNode) -> Result<()> {
        // Generate configuration comment
        if let Some(name) = &config.name {
            self.html_output.push_str(&format!("<!-- Configuration: {} -->\n", name));
        } else {
            self.html_output.push_str("<!-- Configuration -->\n");
        }
        
        // Generate configuration settings as comments
        for setting in &config.settings {
            self.html_output.push_str(&format!("<!-- {}: {:?} -->\n", setting.key, setting.value));
        }
        
        Ok(())
    }
    
    fn generate_use(&mut self, use_node: UseNode) -> Result<()> {
        match use_node.use_type {
            UseType::Html5 => {
                // Add HTML5 doctype
                self.html_output = format!("<!DOCTYPE html>\n{}", self.html_output);
            }
            UseType::Config => {
                // TODO: Apply configuration
                println!("Using config");
            }
        }
        Ok(())
    }
}