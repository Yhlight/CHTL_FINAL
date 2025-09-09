//! CHTL Parser - Parses tokens into Abstract Syntax Tree
//! 
//! This module handles the syntactic analysis of CHTL tokens,
//! converting them into a structured Abstract Syntax Tree.

use anyhow::Result;
use std::collections::VecDeque;

use crate::chtl::lexer::{Token, TokenType};
use crate::chtl::node::*;

/// CHTL Parser
pub struct ChtlParser {
    tokens: VecDeque<Token>,
    current: usize,
}

impl ChtlParser {
    /// Create a new CHTL parser
    pub fn new() -> Self {
        Self {
            tokens: VecDeque::new(),
            current: 0,
        }
    }
    
    /// Parse tokens into an AST
    pub fn parse(&self, tokens: VecDeque<Token>) -> Result<DocumentNode> {
        let mut parser = Self {
            tokens,
            current: 0,
        };
        
        parser.parse_document()
    }
    
    fn parse_document(&mut self) -> Result<DocumentNode> {
        let mut nodes = Vec::new();
        
        while !self.is_at_end() {
            if let Some(node) = self.parse_node()? {
                nodes.push(node);
            }
        }
        
        Ok(DocumentNode { nodes })
    }
    
    fn parse_node(&mut self) -> Result<Option<AstNode>> {
        let token = self.peek()?;
        
        match &token.token_type {
            TokenType::Text => self.parse_text_node(),
            TokenType::Style => self.parse_style_node(),
            TokenType::Script => self.parse_script_node(),
            TokenType::Template => self.parse_template_node(),
            TokenType::Custom => self.parse_custom_node(),
            TokenType::Origin => self.parse_origin_node(),
            TokenType::Import => self.parse_import_node(),
            TokenType::Namespace => self.parse_namespace_node(),
            TokenType::Configuration => self.parse_configuration_node(),
            TokenType::Use => self.parse_use_node(),
            TokenType::LeftBracket => self.parse_declaration_node(),
            TokenType::Identifier(_) => self.parse_element_node(),
            TokenType::Newline => {
                let _ = self.advance();
                Ok(None)
            }
            TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                let _ = self.advance();
                Ok(None)
            }
            _ => {
                let _ = self.advance();
                Ok(None)
            }
        }
    }
    
    fn parse_text_node(&mut self) -> Result<Option<AstNode>> {
        self.consume(TokenType::Text)?;
        
        // Skip newlines before expecting LeftBrace
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        self.consume(TokenType::LeftBrace)?;
        
        let content = self.parse_text_content()?;
        
        self.consume(TokenType::RightBrace)?;
        
        Ok(Some(AstNode::Text(TextNode { content })))
    }
    
    fn parse_text_content(&mut self) -> Result<String> {
        let mut content = String::new();
        
        while !self.is_at_end() {
            let token = self.peek()?;
            match &token.token_type {
                TokenType::RightBrace => break,
                TokenType::Newline => {
                    // Include newlines in text content as spaces
                    content.push(' ');
                    self.advance()?;
                }
                TokenType::String(s) => {
                    content.push_str(s);
                    self.advance()?;
                }
                TokenType::Identifier(s) => {
                    if !content.is_empty() && !content.ends_with(' ') {
                        content.push(' ');
                    }
                    content.push_str(s);
                    self.advance()?;
                }
                TokenType::Number(n) => {
                    content.push_str(n);
                    self.advance()?;
                }
                _ => {
                    content.push_str(&token.value);
                    self.advance()?;
                }
            }
        }
        
        Ok(content.trim().to_string())
    }
    
    fn parse_style_node(&mut self) -> Result<Option<AstNode>> {
        self.consume(TokenType::Style)?;
        
        // Skip newlines before expecting LeftBrace
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        self.consume(TokenType::LeftBrace)?;
        
        let mut properties = Vec::new();
        
        while !self.is_at_end() {
            let token = self.peek()?.clone();
            match &token.token_type {
                TokenType::RightBrace => break,
                TokenType::Newline => {
                    // Skip newlines within style body
                    self.advance()?;
                }
                TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                    // Skip comments within style body
                    self.advance()?;
                }
                TokenType::Identifier(name) => {
                    self.advance();
                    self.consume(TokenType::Colon)?;
                    
                    let value = self.parse_style_value()?;
                    properties.push(StyleProperty {
                        name: name.clone(),
                        value,
                    });
                    
                    if self.peek()?.token_type == TokenType::Semicolon {
                        self.advance();
                    }
                }
                _ => {
                    self.advance();
                }
            }
        }
        
        self.consume(TokenType::RightBrace)?;
        
        Ok(Some(AstNode::Style(StyleNode { properties })))
    }
    
    fn parse_style_value(&mut self) -> Result<String> {
        let mut value = String::new();
        
        while !self.is_at_end() {
            let token = self.peek()?;
            match &token.token_type {
                TokenType::Semicolon | TokenType::RightBrace | TokenType::Newline => break,
                TokenType::String(s) => {
                    value.push_str(s);
                    self.advance()?;
                }
                TokenType::Identifier(s) => {
                    if !value.is_empty() && !value.ends_with(' ') {
                        value.push(' ');
                    }
                    value.push_str(s);
                    self.advance()?;
                }
                TokenType::Number(n) => {
                    if !value.is_empty() && !value.ends_with(' ') {
                        value.push(' ');
                    }
                    value.push_str(n);
                    self.advance()?;
                }
                _ => {
                    if !value.is_empty() && !value.ends_with(' ') && !token.value.starts_with(' ') {
                        value.push(' ');
                    }
                    value.push_str(&token.value);
                    self.advance()?;
                }
            }
        }
        
        Ok(value.trim().to_string())
    }
    
    fn parse_script_node(&mut self) -> Result<Option<AstNode>> {
        self.consume(TokenType::Script)?;
        self.consume(TokenType::LeftBrace)?;
        
        let content = self.parse_script_content()?;
        
        self.consume(TokenType::RightBrace)?;
        
        Ok(Some(AstNode::Script(ScriptNode { content })))
    }
    
    fn parse_script_content(&mut self) -> Result<String> {
        let mut content = String::new();
        
        while !self.is_at_end() {
            let token = self.peek()?;
            match &token.token_type {
                TokenType::RightBrace => break,
                _ => {
                    content.push_str(&token.value);
                    self.advance();
                }
            }
        }
        
        Ok(content)
    }
    
    fn parse_element_node(&mut self) -> Result<Option<AstNode>> {
        let token = self.advance()?;
        let tag_name = match &token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(anyhow::anyhow!("Expected element name")),
        };
        
        let mut attributes = Vec::new();
        
        // Parse attributes before the opening brace
        while !self.is_at_end() {
            let token = self.peek()?.clone();
            match &token.token_type {
                TokenType::LeftBrace => break,
                TokenType::Newline => {
                    // Skip newlines
                    self.advance()?;
                }
                TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                    // Skip comments
                    self.advance()?;
                }
                TokenType::Identifier(name) => {
                    // This is an attribute
                    self.advance();
                    
                    // Check if there's an equals sign
                    if self.peek()?.token_type == TokenType::Equals {
                        self.advance()?; // consume the equals sign
                    }
                    
                    let value = self.parse_attribute_value()?;
                    attributes.push(Attribute {
                        name: name.clone(),
                        value,
                    });
                    
                    if self.peek()?.token_type == TokenType::Semicolon {
                        self.advance()?;
                    }
                }
                _ => {
                    return Err(anyhow::anyhow!("Expected LeftBrace or attribute, got {:?}", token.token_type));
                }
            }
        }
        
        self.consume(TokenType::LeftBrace)?;
        
        let mut children = Vec::new();
        
        while !self.is_at_end() {
            let token = self.peek()?.clone();
            match &token.token_type {
                TokenType::RightBrace => break,
                TokenType::Newline => {
                    // Skip newlines within element body
                    self.advance()?;
                }
                TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                    // Skip comments within element body
                    self.advance()?;
                }
                _ => {
                    // This is a child element
                    if let Some(child) = self.parse_node()? {
                        children.push(child);
                    }
                }
            }
        }
        
        self.consume(TokenType::RightBrace)?;
        
        Ok(Some(AstNode::Element(ElementNode {
            tag_name,
            attributes,
            children,
        })))
    }
    
    fn parse_attribute_value(&mut self) -> Result<String> {
        let mut value = String::new();
        
        while !self.is_at_end() {
            let token = self.peek()?;
            match &token.token_type {
                TokenType::Semicolon | TokenType::RightBrace => break,
                TokenType::String(s) => {
                    value.push_str(s);
                    self.advance();
                }
                TokenType::Identifier(s) => {
                    value.push_str(s);
                    self.advance();
                }
                TokenType::Number(n) => {
                    value.push_str(n);
                    self.advance();
                }
                _ => {
                    value.push_str(&token.value);
                    self.advance();
                }
            }
        }
        
        Ok(value)
    }
    
    fn parse_template_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement template parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_custom_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement custom parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_origin_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement origin parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_import_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement import parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_namespace_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement namespace parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_configuration_node(&mut self) -> Result<Option<AstNode>> {
        // TODO: Implement configuration parsing
        self.advance();
        Ok(None)
    }
    
    fn parse_use_node(&mut self) -> Result<Option<AstNode>> {
        self.consume(TokenType::Use)?;
        
        let token = self.advance()?;
        let name = match &token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(anyhow::anyhow!("Expected identifier after use")),
        };
        
        self.consume(TokenType::Semicolon)?;
        
        let use_type = match name.as_str() {
            "html5" => UseType::Html5,
            _ => UseType::Config,
        };
        
        Ok(Some(AstNode::Use(UseNode {
            use_type,
            name,
        })))
    }
    
    fn parse_declaration_node(&mut self) -> Result<Option<AstNode>> {
        self.consume(TokenType::LeftBracket)?;
        
        let keyword_token = self.advance()?;
        let keyword = match &keyword_token.token_type {
            TokenType::Template => "Template",
            TokenType::Custom => "Custom",
            TokenType::Origin => "Origin",
            TokenType::Import => "Import",
            TokenType::Namespace => "Namespace",
            TokenType::Configuration => "Configuration",
            TokenType::Identifier(name) => name,
            _ => return Err(anyhow::anyhow!("Expected declaration keyword after '['")),
        };
        
        self.consume(TokenType::RightBracket)?;
        
        match keyword {
            "Template" => self.parse_template_declaration(),
            "Custom" => self.parse_custom_declaration(),
            "Origin" => self.parse_origin_declaration(),
            "Import" => self.parse_import_declaration(),
            "Namespace" => self.parse_namespace_declaration(),
            "Configuration" => self.parse_configuration_declaration(),
            _ => Err(anyhow::anyhow!("Unknown declaration keyword: {}", keyword)),
        }
    }
    
    fn parse_template_declaration(&mut self) -> Result<Option<AstNode>> {
        // Skip newlines after [Template]
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse @Style or @Element
        self.consume(TokenType::At)?;
        
        let type_token = self.advance()?;
        let template_type = match &type_token.token_type {
            TokenType::Style => "Style",
            TokenType::Identifier(name) if name == "Style" => "Style",
            TokenType::Identifier(name) if name == "Element" => "Element",
            TokenType::Identifier(name) if name == "Var" => "Var",
            _ => return Err(anyhow::anyhow!("Expected @Style, @Element, or @Var after [Template], got {:?}", type_token.token_type)),
        };
        
        let name_token = self.advance()?;
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(anyhow::anyhow!("Expected template name")),
        };
        
        // Skip newlines before expecting LeftBrace
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        self.consume(TokenType::LeftBrace)?;
        
        // Parse template body based on type
        let body = match template_type {
            "Style" => {
                let mut properties = Vec::new();
                
                while !self.is_at_end() {
                    let token = self.peek()?.clone();
                    match &token.token_type {
                        TokenType::RightBrace => break,
                        TokenType::Newline => {
                            self.advance()?;
                        }
                        TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                            self.advance()?;
                        }
                        TokenType::Identifier(prop_name) => {
                            self.advance()?;
                            self.consume(TokenType::Colon)?;
                            
                            let value = self.parse_style_value()?;
                            properties.push(StyleProperty {
                                name: prop_name.clone(),
                                value,
                            });
                            
                            // Optional semicolon
                            if !self.is_at_end() && self.peek()?.token_type == TokenType::Semicolon {
                                self.advance()?;
                            }
                        }
                        _ => {
                            self.advance()?;
                        }
                    }
                }
                
                format!("Style properties: {:?}", properties)
            }
            "Element" => {
                // For now, just parse the element body as a string
                let mut body_content = String::new();
                let mut brace_count = 1;
                
                while !self.is_at_end() && brace_count > 0 {
                    let token = self.advance()?;
                    match &token.token_type {
                        TokenType::LeftBrace => {
                            brace_count += 1;
                            body_content.push_str("{ ");
                        }
                        TokenType::RightBrace => {
                            brace_count -= 1;
                            if brace_count > 0 {
                                body_content.push_str("} ");
                            }
                        }
                        TokenType::Identifier(name) => {
                            body_content.push_str(name);
                            body_content.push(' ');
                        }
                        _ => {
                            body_content.push_str(&token.value);
                            body_content.push(' ');
                        }
                    }
                }
                
                body_content.trim().to_string()
            }
            _ => String::new(),
        };
        
        self.consume(TokenType::RightBrace)?;
        
        Ok(Some(AstNode::Template(TemplateNode {
            name,
            template_type: match template_type {
                "Style" => TemplateType::Style,
                "Element" => TemplateType::Element,
                "Var" => TemplateType::Var,
                _ => TemplateType::Element,
            },
            content: Vec::new(), // For now, we'll put the parsed content here later
        })))
    }
    
    fn parse_custom_declaration(&mut self) -> Result<Option<AstNode>> {
        // Placeholder for Custom declarations
        Ok(Some(AstNode::Custom(CustomNode {
            name: "Custom".to_string(),
            custom_type: CustomType::Element,
            content: Vec::new(),
        })))
    }
    
    fn parse_origin_declaration(&mut self) -> Result<Option<AstNode>> {
        // Placeholder for Origin declarations
        Ok(Some(AstNode::Origin(OriginNode {
            origin_type: OriginType::Html,
            name: None,
            content: "Origin content".to_string(),
        })))
    }
    
    fn parse_import_declaration(&mut self) -> Result<Option<AstNode>> {
        // Skip newlines after [Import]
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse import type (@Module, @Style, @Script, etc.)
        self.consume(TokenType::At)?;
        
        let type_token = self.advance()?;
        let import_type = match &type_token.token_type {
            TokenType::Identifier(name) if name == "Module" => ImportType::Module,
            TokenType::Identifier(name) if name == "Style" => ImportType::Style,
            TokenType::Identifier(name) if name == "Script" => ImportType::Script,
            TokenType::Identifier(name) if name == "Config" => ImportType::Config,
            _ => return Err(anyhow::anyhow!("Expected @Module, @Style, @Script, or @Config after [Import], got {:?}", type_token.token_type)),
        };
        
        // Parse module path
        let path_token = self.advance()?;
        let path = match &path_token.token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            _ => return Err(anyhow::anyhow!("Expected module path after import type, got {:?}", path_token.token_type)),
        };
        
        // Parse optional alias
        let mut alias = None;
        if !self.is_at_end() && self.peek()?.token_type == TokenType::As {
            self.advance()?; // consume 'as'
            let alias_token = self.advance()?;
            alias = match &alias_token.token_type {
                TokenType::Identifier(s) => Some(s.clone()),
                _ => return Err(anyhow::anyhow!("Expected alias identifier after 'as', got {:?}", alias_token.token_type)),
            };
        }
        
        Ok(Some(AstNode::Import(ImportNode {
            import_type,
            name: None,
            path,
            alias,
        })))
    }
    
    fn parse_namespace_declaration(&mut self) -> Result<Option<AstNode>> {
        // Skip newlines after [Namespace]
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse namespace name
        let name_token = self.advance()?;
        let name = match &name_token.token_type {
            TokenType::Identifier(s) => s.clone(),
            _ => return Err(anyhow::anyhow!("Expected namespace name after [Namespace], got {:?}", name_token.token_type)),
        };
        
        // Parse namespace content (elements, styles, etc.)
        let mut content = Vec::new();
        
        // Skip newlines before content
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse content until end of namespace
        while !self.is_at_end() {
            if let Some(node) = self.parse_node()? {
                content.push(node);
            } else {
                break;
            }
        }
        
        Ok(Some(AstNode::Namespace(NamespaceNode {
            name,
            content,
        })))
    }
    
    fn parse_configuration_declaration(&mut self) -> Result<Option<AstNode>> {
        // Skip newlines after [Configuration]
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse optional configuration name
        let mut name = None;
        if !self.is_at_end() && matches!(self.peek()?.token_type, TokenType::Identifier(_)) {
            let name_token = self.advance()?;
            name = match &name_token.token_type {
                TokenType::Identifier(s) => Some(s.clone()),
                _ => None,
            };
        }
        
        // Parse configuration settings
        let mut settings = Vec::new();
        
        // Skip newlines before settings
        while !self.is_at_end() && self.peek()?.token_type == TokenType::Newline {
            self.advance()?;
        }
        
        // Parse settings until end of configuration
        while !self.is_at_end() {
            if self.peek()?.token_type == TokenType::LeftBracket {
                // Check if this is the end of configuration
                let bracket_token = self.advance()?;
                if let Ok(next_token) = self.peek() {
                    if matches!(&next_token.token_type, TokenType::Identifier(s) if s == "Configuration" || s == "End") {
                        self.advance()?; // consume the identifier
                        if self.peek()?.token_type == TokenType::RightBracket {
                            self.advance()?; // consume the closing bracket
                            break;
                        }
                    }
                }
                // If not end of configuration, continue parsing
            }
            
            // Parse setting key-value pairs
            if let Ok(key_token) = self.advance() {
                if let TokenType::Identifier(key) = &key_token.token_type {
                    // Skip newlines and colons
                    while !self.is_at_end() && matches!(self.peek()?.token_type, TokenType::Newline | TokenType::Colon) {
                        self.advance()?;
                    }
                    
                    // Parse value
                    let value_token = self.advance()?;
                    let value = match &value_token.token_type {
                        TokenType::String(s) => ConfigValue::String(s.clone()),
                        TokenType::Identifier(s) => ConfigValue::String(s.clone()),
                        TokenType::Number(s) => ConfigValue::String(s.clone()),
                        _ => return Err(anyhow::anyhow!("Expected configuration value, got {:?}", value_token.token_type)),
                    };
                    
                    settings.push(ConfigSetting {
                        key: key.clone(),
                        value,
                    });
                }
            } else {
                break;
            }
        }
        
        Ok(Some(AstNode::Configuration(ConfigurationNode {
            name,
            settings,
        })))
    }
    
    fn is_at_end(&self) -> bool {
        self.current >= self.tokens.len()
    }
    
    fn peek(&self) -> Result<&Token> {
        self.tokens.get(self.current)
            .ok_or_else(|| anyhow::anyhow!("Unexpected end of tokens"))
    }
    
    fn peek_next(&self) -> Result<&Token> {
        self.tokens.get(self.current + 1)
            .ok_or_else(|| anyhow::anyhow!("Unexpected end of tokens"))
    }
    
    fn advance(&mut self) -> Result<Token> {
        let token = self.peek()?.clone();
        self.current += 1;
        Ok(token)
    }
    
    fn consume(&mut self, expected: TokenType) -> Result<()> {
        let token = self.advance()?;
        if std::mem::discriminant(&token.token_type) == std::mem::discriminant(&expected) {
            Ok(())
        } else {
            Err(anyhow::anyhow!("Expected {:?}, got {:?}", expected, token.token_type))
        }
    }
}