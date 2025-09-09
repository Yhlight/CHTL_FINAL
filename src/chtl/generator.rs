//! CHTL code generator
//! 
//! This module provides the code generation functionality for CHTL,
//! including HTML, CSS, and JavaScript generation.

use std::collections::HashMap;
use crate::chtl::context::*;
use crate::chtl::node::*;

/// CHTL code generator
#[derive(Debug, Clone)]
pub struct CHTLGenerator {
    /// Generation context
    context: CHTLContext,
    /// Generated HTML
    html: String,
    /// Generated CSS
    css: String,
    /// Generated JavaScript
    javascript: String,
    /// CSS class counter
    class_counter: usize,
    /// ID counter
    id_counter: usize,
    /// Global styles
    global_styles: Vec<GlobalStyle>,
    /// Global scripts
    global_scripts: Vec<GlobalScript>,
}

/// Global style definition
#[derive(Debug, Clone)]
pub struct GlobalStyle {
    pub selector: String,
    pub properties: Vec<StyleProperty>,
    pub media_queries: Vec<MediaQuery>,
    pub pseudo_classes: Vec<PseudoClass>,
    pub pseudo_elements: Vec<PseudoElement>,
}

/// Media query
#[derive(Debug, Clone)]
pub struct MediaQuery {
    pub condition: String,
    pub styles: Vec<GlobalStyle>,
}

/// Pseudo class
#[derive(Debug, Clone)]
pub struct PseudoClass {
    pub name: String,
    pub styles: Vec<GlobalStyle>,
}

/// Pseudo element
#[derive(Debug, Clone)]
pub struct PseudoElement {
    pub name: String,
    pub styles: Vec<GlobalStyle>,
}

/// Global script definition
#[derive(Debug, Clone)]
pub struct GlobalScript {
    pub content: String,
    pub script_type: ScriptType,
    pub priority: usize,
}

/// Generation error
#[derive(Debug, Clone)]
pub struct GenerationError {
    pub message: String,
    pub location: SourceLocation,
    pub suggestion: Option<String>,
}

impl CHTLGenerator {
    /// Create a new CHTL generator
    pub fn new(context: CHTLContext) -> Self {
        Self {
            context,
            html: String::new(),
            css: String::new(),
            javascript: String::new(),
            class_counter: 0,
            id_counter: 0,
            global_styles: Vec::new(),
            global_scripts: Vec::new(),
        }
    }
    
    /// Generate code from AST
    pub fn generate(&mut self, ast: &Node) -> Result<CompilationResult, GenerationError> {
        self.html.clear();
        self.css.clear();
        self.javascript.clear();
        self.global_styles.clear();
        self.global_scripts.clear();
        
        // Generate HTML5 doctype if configured
        if self.context.config.debug_mode {
            self.html.push_str("<!DOCTYPE html>\n");
        }
        
        // Generate code from AST
        self.generate_node(ast)?;
        
        // Generate global styles
        self.generate_global_styles()?;
        
        // Generate global scripts
        self.generate_global_scripts()?;
        
        Ok(CompilationResult {
            html: self.html.clone(),
            css: self.css.clone(),
            javascript: self.javascript.clone(),
        })
    }
    
    /// Generate code from a node
    fn generate_node(&mut self, node: &Node) -> Result<(), GenerationError> {
        match node {
            Node::Element(element) => self.generate_element(element)?,
            Node::Text(text) => self.generate_text(text)?,
            Node::Comment(comment) => self.generate_comment(comment)?,
            Node::Style(style) => self.generate_style(style)?,
            Node::Script(script) => self.generate_script(script)?,
            Node::Template(template) => self.generate_template(template)?,
            Node::Custom(custom) => self.generate_custom(custom)?,
            Node::Origin(origin) => self.generate_origin(origin)?,
            Node::Import(import) => self.generate_import(import)?,
            Node::Config(config) => self.generate_config(config)?,
            Node::Namespace(namespace) => self.generate_namespace(namespace)?,
        }
        Ok(())
    }
    
    /// Generate HTML element
    fn generate_element(&mut self, element: &ElementNode) -> Result<(), GenerationError> {
        // Handle special elements
        match element.tag_name.as_str() {
            "html" => {
                self.html.push_str("<html");
                self.generate_attributes(&element.attributes)?;
                self.html.push_str(">\n");
                
                // Generate children
                for child in &element.children {
                    self.generate_node(child)?;
                }
                
                self.html.push_str("</html>\n");
            }
            "head" => {
                self.html.push_str("<head");
                self.generate_attributes(&element.attributes)?;
                self.html.push_str(">\n");
                
                // Generate children
                for child in &element.children {
                    self.generate_node(child)?;
                }
                
                self.html.push_str("</head>\n");
            }
            "body" => {
                self.html.push_str("<body");
                self.generate_attributes(&element.attributes)?;
                self.html.push_str(">\n");
                
                // Generate children
                for child in &element.children {
                    self.generate_node(child)?;
                }
                
                self.html.push_str("</body>\n");
            }
            _ => {
                // Check if it's a self-closing tag
                let self_closing_tags = ["img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed", "source", "track", "wbr"];
                
                if self_closing_tags.contains(&element.tag_name.as_str()) {
                    self.html.push_str(&format!("<{}", element.tag_name));
                    self.generate_attributes(&element.attributes)?;
                    self.html.push_str(" />\n");
                } else {
                    self.html.push_str(&format!("<{}", element.tag_name));
                    self.generate_attributes(&element.attributes)?;
                    self.html.push_str(">");
                    
                    // Generate children
                    for child in &element.children {
                        self.generate_node(child)?;
                    }
                    
                    self.html.push_str(&format!("</{}>\n", element.tag_name));
                }
            }
        }
        
        // Generate local style if present
        if let Some(style) = &element.style {
            self.generate_local_style(element, style)?;
        }
        
        // Generate local script if present
        if let Some(script) = &element.script {
            self.generate_local_script(element, script)?;
        }
        
        Ok(())
    }
    
    /// Generate text content
    fn generate_text(&mut self, text: &TextNode) -> Result<(), GenerationError> {
        // Escape HTML special characters
        let escaped = self.escape_html(&text.content);
        self.html.push_str(&escaped);
        Ok(())
    }
    
    /// Generate comment
    fn generate_comment(&mut self, comment: &CommentNode) -> Result<(), GenerationError> {
        match comment.comment_type {
            CommentType::SingleLine => {
                // Single line comments are not generated to HTML
            }
            CommentType::MultiLine => {
                // Multi line comments are not generated to HTML
            }
            CommentType::Generator => {
                // Generator comments are generated to HTML
                self.html.push_str(&format!("<!-- {} -->\n", comment.content));
            }
        }
        Ok(())
    }
    
    /// Generate style block
    fn generate_style(&mut self, style: &StyleNode) -> Result<(), GenerationError> {
        // Add to global styles
        for property in &style.content.properties {
            // TODO: Handle style selectors and conditions
            self.global_styles.push(GlobalStyle {
                selector: "global".to_string(),
                properties: vec![property.clone()],
                media_queries: Vec::new(),
                pseudo_classes: Vec::new(),
                pseudo_elements: Vec::new(),
            });
        }
        Ok(())
    }
    
    /// Generate script block
    fn generate_script(&mut self, script: &ScriptNode) -> Result<(), GenerationError> {
        // Add to global scripts
        self.global_scripts.push(GlobalScript {
            content: script.content.content.clone(),
            script_type: script.content.script_type.clone(),
            priority: 0,
        });
        Ok(())
    }
    
    /// Generate template
    fn generate_template(&mut self, template: &TemplateNode) -> Result<(), GenerationError> {
        // Templates are not directly generated, they are used by other elements
        // Store template in context for later use
        self.context.templates.insert(template.name.clone(), TemplateDefinition {
            name: template.name.clone(),
            template_type: template.template_type.clone(),
            content: template.content.clone(),
            parameters: Vec::new(),
        });
        Ok(())
    }
    
    /// Generate custom definition
    fn generate_custom(&mut self, custom: &CustomNode) -> Result<(), GenerationError> {
        // Custom definitions are not directly generated, they are used by other elements
        // Store custom in context for later use
        self.context.customs.insert(custom.name.clone(), CustomDefinition {
            name: custom.name.clone(),
            custom_type: custom.custom_type.clone(),
            content: custom.content.clone(),
            specializations: Vec::new(),
        });
        Ok(())
    }
    
    /// Generate origin content
    fn generate_origin(&mut self, origin: &OriginNode) -> Result<(), GenerationError> {
        match origin.origin_type {
            OriginType::HTML => {
                self.html.push_str(&origin.content);
            }
            OriginType::CSS => {
                self.css.push_str(&origin.content);
            }
            OriginType::JavaScript => {
                self.javascript.push_str(&origin.content);
            }
            OriginType::Custom(_) => {
                // Custom origin types are handled based on configuration
                self.html.push_str(&origin.content);
            }
        }
        Ok(())
    }
    
    /// Generate import statement
    fn generate_import(&mut self, import: &ImportNode) -> Result<(), GenerationError> {
        // Imports are handled during compilation, not generation
        // This is a placeholder for future implementation
        Ok(())
    }
    
    /// Generate configuration
    fn generate_config(&mut self, config: &ConfigNode) -> Result<(), GenerationError> {
        // Configuration is handled during compilation, not generation
        // This is a placeholder for future implementation
        Ok(())
    }
    
    /// Generate namespace
    fn generate_namespace(&mut self, namespace: &NamespaceNode) -> Result<(), GenerationError> {
        // Generate namespace content
        for child in &namespace.content {
            self.generate_node(child)?;
        }
        Ok(())
    }
    
    /// Generate local style for an element
    fn generate_local_style(&mut self, element: &ElementNode, style: &StyleBlock) -> Result<(), GenerationError> {
        // Generate CSS class or ID if needed
        let selector = self.generate_selector(element, style)?;
        
        // Add to global styles
        self.global_styles.push(GlobalStyle {
            selector,
            properties: style.properties.clone(),
            media_queries: Vec::new(),
            pseudo_classes: Vec::new(),
            pseudo_elements: Vec::new(),
        });
        
        Ok(())
    }
    
    /// Generate local script for an element
    fn generate_local_script(&mut self, element: &ElementNode, script: &ScriptBlock) -> Result<(), GenerationError> {
        // Add to global scripts with element context
        self.global_scripts.push(GlobalScript {
            content: script.content.clone(),
            script_type: script.script_type.clone(),
            priority: 1, // Local scripts have higher priority
        });
        Ok(())
    }
    
    /// Generate CSS selector for an element
    fn generate_selector(&mut self, element: &ElementNode, style: &StyleBlock) -> Result<String, GenerationError> {
        // Check if element has class or ID
        if let Some(class) = element.get_attribute("class") {
            return Ok(format!(".{}", class));
        }
        
        if let Some(id) = element.get_attribute("id") {
            return Ok(format!("#{}", id));
        }
        
        // Generate automatic class or ID based on style selectors
        for selector in &style.selectors {
            match selector.selector_type {
                SelectorType::Class => {
                    // Add class to element
                    element.set_attribute("class".to_string(), selector.value.clone());
                    return Ok(format!(".{}", selector.value));
                }
                SelectorType::Id => {
                    // Add ID to element
                    element.set_attribute("id".to_string(), selector.value.clone());
                    return Ok(format!("#{}", selector.value));
                }
                _ => {}
            }
        }
        
        // Generate automatic class
        let class_name = format!("chtl-class-{}", self.class_counter);
        self.class_counter += 1;
        element.set_attribute("class".to_string(), class_name.clone());
        Ok(format!(".{}", class_name))
    }
    
    /// Generate attributes
    fn generate_attributes(&mut self, attributes: &HashMap<String, String>) -> Result<(), GenerationError> {
        for (key, value) in attributes {
            self.html.push_str(&format!(" {}=\"{}\"", key, self.escape_html(value)));
        }
        Ok(())
    }
    
    /// Generate global styles
    fn generate_global_styles(&mut self) -> Result<(), GenerationError> {
        if !self.global_styles.is_empty() {
            self.css.push_str("<style>\n");
            
            for style in &self.global_styles {
                self.css.push_str(&format!("{} {{\n", style.selector));
                
                for property in &style.properties {
                    self.css.push_str(&format!("    {}: {};\n", property.name, property.value));
                }
                
                self.css.push_str("}\n\n");
            }
            
            self.css.push_str("</style>\n");
        }
        Ok(())
    }
    
    /// Generate global scripts
    fn generate_global_scripts(&mut self) -> Result<(), GenerationError> {
        if !self.global_scripts.is_empty() {
            self.javascript.push_str("<script>\n");
            
            // Sort scripts by priority
            let mut sorted_scripts = self.global_scripts.clone();
            sorted_scripts.sort_by_key(|s| s.priority);
            
            for script in sorted_scripts {
                self.javascript.push_str(&script.content);
                self.javascript.push_str("\n");
            }
            
            self.javascript.push_str("</script>\n");
        }
        Ok(())
    }
    
    /// Escape HTML special characters
    fn escape_html(&self, text: &str) -> String {
        text.replace('&', "&amp;")
            .replace('<', "&lt;")
            .replace('>', "&gt;")
            .replace('"', "&quot;")
            .replace('\'', "&#x27;")
    }
    
    /// Get the context
    pub fn get_context(&self) -> &CHTLContext {
        &self.context
    }
    
    /// Get mutable context
    pub fn get_context_mut(&mut self) -> &mut CHTLContext {
        &mut self.context
    }
}

impl Default for CHTLGenerator {
    fn default() -> Self {
        Self::new(CHTLContext::new())
    }
}