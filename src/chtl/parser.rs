//! CHTL parser
//! 
//! This module provides the syntax analyzer for CHTL source code,
//! converting tokens into an Abstract Syntax Tree (AST).

use crate::chtl::lexer::{CHTLLexer, Token, TokenType};
use crate::chtl::node::*;
use crate::chtl::context::CHTLContext;

/// CHTL parser for converting tokens to AST
#[derive(Debug, Clone)]
pub struct CHTLParser {
    /// Tokens to parse
    tokens: Vec<Token>,
    /// Current token index
    position: usize,
    /// Parser context
    context: CHTLContext,
}

/// Parser error
#[derive(Debug, thiserror::Error)]
pub enum ParserError {
    #[error("Unexpected token '{0}' at line {1}, column {2}")]
    UnexpectedToken(String, usize, usize),
    
    #[error("Expected token '{0}' but found '{1}' at line {2}, column {3}")]
    ExpectedToken(String, String, usize, usize),
    
    #[error("Unexpected end of file")]
    UnexpectedEOF,
    
    #[error("Invalid syntax: {0}")]
    InvalidSyntax(String),
    
    #[error("Lexical error: {0}")]
    Lexical(#[from] crate::chtl::lexer::LexerError),
}

impl CHTLParser {
    /// Create a new parser with source code
    pub fn new(source: String) -> Result<Self, ParserError> {
        let mut lexer = CHTLLexer::new(source);
        let tokens = lexer.tokenize()?;
        
        Ok(Self {
            tokens,
            position: 0,
            context: CHTLContext::new(),
        })
    }
    
    /// Parse the tokens into an AST
    pub fn parse(&mut self) -> Result<Document, ParserError> {
        let mut document = Document::new();
        
        while !self.is_at_end() {
            let node = self.parse_node()?;
            document.add_child(node);
        }
        
        Ok(document)
    }
    
    /// Parse a single node
    fn parse_node(&mut self) -> Result<Node, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::Template => self.parse_template(),
            TokenType::Custom => self.parse_custom(),
            TokenType::Import => self.parse_import(),
            TokenType::Namespace => self.parse_namespace(),
            TokenType::Configuration => self.parse_configuration(),
            TokenType::Origin => self.parse_origin(),
            TokenType::SingleLineComment(content) => {
                self.advance();
                Ok(Node::Comment(CommentNode::new(
                    content,
                    CommentType::SingleLine,
                )))
            }
            TokenType::MultiLineComment(content) => {
                self.advance();
                Ok(Node::Comment(CommentNode::new(
                    content,
                    CommentType::MultiLine,
                )))
            }
            TokenType::GeneratorComment(content) => {
                self.advance();
                Ok(Node::Comment(CommentNode::new(
                    content,
                    CommentType::Generator,
                )))
            }
            TokenType::LeftBracket => self.parse_element(),
            TokenType::String(content) => {
                self.advance();
                Ok(Node::Text(TextNode::new(content, TextType::Literal)))
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse a template definition
    fn parse_template(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Template]
        
        let template_type = self.parse_template_type()?;
        self.expect_token(&TokenType::At)?;
        self.advance(); // Skip @
        
        let name = self.expect_identifier()?;
        
        self.expect_token(&TokenType::LeftBrace)?;
        self.advance(); // Skip {
        
        let mut template = TemplateNode::new(template_type, name);
        
        // Parse template content
        while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
            let node = self.parse_template_content()?;
            template.add_content(node);
        }
        
        self.expect_token(&TokenType::RightBrace)?;
        self.advance(); // Skip }
        
        Ok(Node::Template(template))
    }
    
    /// Parse template type
    fn parse_template_type(&mut self) -> Result<TemplateType, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::Style => {
                self.advance();
                Ok(TemplateType::Style)
            }
            TokenType::Element => {
                self.advance();
                Ok(TemplateType::Element)
            }
            TokenType::Variable => {
                self.advance();
                Ok(TemplateType::Variable)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse template content
    fn parse_template_content(&mut self) -> Result<Node, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::LeftBracket => self.parse_element(),
            TokenType::String(_) => self.parse_text(),
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse a custom definition
    fn parse_custom(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Custom]
        
        let custom_type = self.parse_custom_type()?;
        self.expect_token(&TokenType::At)?;
        self.advance(); // Skip @
        
        let name = self.expect_identifier()?;
        
        let mut custom = CustomNode::new(custom_type, name);
        
        // Check for base template
        if self.check_token(&TokenType::Colon) {
            self.advance(); // Skip :
            let base_template = self.expect_identifier()?;
            custom.set_base_template(base_template);
        }
        
        self.expect_token(&TokenType::LeftBrace)?;
        self.advance(); // Skip {
        
        // Parse custom modifications
        while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
            let modification = self.parse_custom_modification()?;
            custom.add_modification(modification);
        }
        
        self.expect_token(&TokenType::RightBrace)?;
        self.advance(); // Skip }
        
        Ok(Node::Custom(custom))
    }
    
    /// Parse custom type
    fn parse_custom_type(&mut self) -> Result<CustomType, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::Style => {
                self.advance();
                Ok(CustomType::Style)
            }
            TokenType::Element => {
                self.advance();
                Ok(CustomType::Element)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse custom modification
    fn parse_custom_modification(&mut self) -> Result<CustomModification, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::Identifier(name) => {
                self.advance();
                if self.check_token(&TokenType::Colon) {
                    self.advance(); // Skip :
                    let value = self.expect_string()?;
                    Ok(CustomModification::AddProperty(name, value))
                } else {
                    let token = &self.tokens[self.position];
                    Err(ParserError::UnexpectedToken(
                        token.value.clone(),
                        token.location.line,
                        token.location.column,
                    ))
                }
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse an import statement
    fn parse_import(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Import]
        
        let import_type = self.parse_import_type()?;
        let path = self.expect_string()?;
        
        let mut import = ImportNode::new(import_type, path);
        
        // Check for import items
        if self.check_token(&TokenType::LeftBrace) {
            self.advance(); // Skip {
            let mut items = Vec::new();
            
            while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
                let item = self.parse_import_item()?;
                items.push(item);
                
                if self.check_token(&TokenType::Comma) {
                    self.advance(); // Skip ,
                }
            }
            
            self.expect_token(&TokenType::RightBrace)?;
            self.advance(); // Skip }
            import.set_items(items);
        }
        
        Ok(Node::Import(import))
    }
    
    /// Parse import type
    fn parse_import_type(&mut self) -> Result<ImportType, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::CHTL => {
                self.advance();
                Ok(ImportType::CHTL)
            }
            TokenType::HTML => {
                self.advance();
                Ok(ImportType::HTML)
            }
            TokenType::CSS => {
                self.advance();
                Ok(ImportType::CSS)
            }
            TokenType::JavaScript => {
                self.advance();
                Ok(ImportType::JavaScript)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse import item
    fn parse_import_item(&mut self) -> Result<ImportItem, ParserError> {
        let name = self.expect_identifier()?;
        let alias = if self.check_token(&TokenType::Colon) {
            self.advance(); // Skip :
            Some(self.expect_identifier()?)
        } else {
            None
        };
        
        Ok(ImportItem { name, alias })
    }
    
    /// Parse a namespace declaration
    fn parse_namespace(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Namespace]
        
        let name = self.expect_identifier()?;
        let mut namespace = NamespaceNode::new(name);
        
        self.expect_token(&TokenType::LeftBrace)?;
        self.advance(); // Skip {
        
        while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
            let node = self.parse_node()?;
            namespace.add_content(node);
        }
        
        self.expect_token(&TokenType::RightBrace)?;
        self.advance(); // Skip }
        
        Ok(Node::Namespace(namespace))
    }
    
    /// Parse a configuration block
    fn parse_configuration(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Configuration]
        
        let name = if self.check_token(&TokenType::Identifier("".to_string())) {
            Some(self.expect_identifier()?)
        } else {
            None
        };
        
        let mut config = ConfigurationNode::new(name);
        
        self.expect_token(&TokenType::LeftBrace)?;
        self.advance(); // Skip {
        
        while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
            let key = self.expect_identifier()?;
            self.expect_token(&TokenType::Colon)?;
            self.advance(); // Skip :
            let value = self.expect_string()?;
            self.expect_token(&TokenType::Semicolon)?;
            self.advance(); // Skip ;
            
            config.set_property(key, value);
        }
        
        self.expect_token(&TokenType::RightBrace)?;
        self.advance(); // Skip }
        
        Ok(Node::Configuration(config))
    }
    
    /// Parse an origin embedding
    fn parse_origin(&mut self) -> Result<Node, ParserError> {
        self.advance(); // Skip [Origin]
        
        let origin_type = self.parse_origin_type()?;
        let name = if self.check_token(&TokenType::Identifier("".to_string())) {
            Some(self.expect_identifier()?)
        } else {
            None
        };
        
        self.expect_token(&TokenType::LeftBrace)?;
        self.advance(); // Skip {
        
        let content = self.parse_origin_content()?;
        
        self.expect_token(&TokenType::RightBrace)?;
        self.advance(); // Skip }
        
        let mut origin = OriginNode::new(origin_type, content);
        if let Some(name) = name {
            origin.set_name(name);
        }
        
        Ok(Node::Origin(origin))
    }
    
    /// Parse origin type
    fn parse_origin_type(&mut self) -> Result<OriginType, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::HTML => {
                self.advance();
                Ok(OriginType::HTML)
            }
            TokenType::CSS => {
                self.advance();
                Ok(OriginType::CSS)
            }
            TokenType::JavaScript => {
                self.advance();
                Ok(OriginType::JavaScript)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse origin content
    fn parse_origin_content(&mut self) -> Result<String, ParserError> {
        let mut content = String::new();
        
        while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
            let token = &self.tokens[self.position];
            content.push_str(&token.value);
            self.advance();
        }
        
        Ok(content)
    }
    
    /// Parse an HTML element
    fn parse_element(&mut self) -> Result<Node, ParserError> {
        self.expect_token(&TokenType::LeftBracket)?;
        self.advance(); // Skip [
        
        let tag = self.expect_identifier()?;
        let mut element = ElementNode::new(tag);
        
        // Parse attributes
        while !self.is_at_end() && !self.check_token(&TokenType::RightBracket) {
            match &self.tokens[self.position].token_type {
                TokenType::Identifier(_) => {
                    let attr_name = self.expect_identifier()?;
                    self.expect_token(&TokenType::Colon)?;
                    self.advance(); // Skip :
                    let attr_value = self.parse_attribute_value()?;
                    element.set_attribute(attr_name, attr_value);
                }
                TokenType::String(_) => {
                    // Parse text content
                    let text = self.parse_text()?;
                    element.add_child(text);
                }
                _ => {
                    self.advance();
                }
            }
        }
        
        self.expect_token(&TokenType::RightBracket)?;
        self.advance(); // Skip ]
        
        // Parse children
        if self.check_token(&TokenType::LeftBrace) {
            self.advance(); // Skip {
            
            while !self.is_at_end() && !self.check_token(&TokenType::RightBrace) {
                let child = self.parse_node()?;
                element.add_child(child);
            }
            
            self.expect_token(&TokenType::RightBrace)?;
            self.advance(); // Skip }
        }
        
        Ok(Node::Element(element))
    }
    
    /// Parse text content
    fn parse_text(&mut self) -> Result<Node, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::String(content) => {
                self.advance();
                Ok(Node::Text(TextNode::new(
                    content,
                    TextType::DoubleQuoted,
                )))
            }
            TokenType::Identifier(content) => {
                self.advance();
                Ok(Node::Text(TextNode::new(
                    content,
                    TextType::Literal,
                )))
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Parse attribute value
    fn parse_attribute_value(&mut self) -> Result<AttributeValue, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::String(value) => {
                self.advance();
                Ok(AttributeValue::String(value))
            }
            TokenType::Number(value) => {
                self.advance();
                Ok(AttributeValue::Number(value.parse().unwrap_or(0.0)))
            }
            TokenType::Identifier(value) => {
                self.advance();
                if value == "true" {
                    Ok(AttributeValue::Boolean(true))
                } else if value == "false" {
                    Ok(AttributeValue::Boolean(false))
                } else {
                    Ok(AttributeValue::String(value))
                }
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Check if at end of tokens
    fn is_at_end(&self) -> bool {
        self.position >= self.tokens.len()
    }
    
    /// Check if current token matches type
    fn check_token(&self, token_type: &TokenType) -> bool {
        if self.is_at_end() {
            false
        } else {
            std::mem::discriminant(&self.tokens[self.position].token_type) == 
            std::mem::discriminant(token_type)
        }
    }
    
    /// Advance to next token
    fn advance(&mut self) {
        if !self.is_at_end() {
            self.position += 1;
        }
    }
    
    /// Expect a specific token type
    fn expect_token(&self, token_type: &TokenType) -> Result<(), ParserError> {
        if self.check_token(token_type) {
            Ok(())
        } else {
            let token = &self.tokens[self.position];
            Err(ParserError::ExpectedToken(
                format!("{:?}", token_type),
                token.value.clone(),
                token.location.line,
                token.location.column,
            ))
        }
    }
    
    /// Expect an identifier
    fn expect_identifier(&mut self) -> Result<String, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::Identifier(name) => {
                self.advance();
                Ok(name)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
    
    /// Expect a string
    fn expect_string(&mut self) -> Result<String, ParserError> {
        if self.is_at_end() {
            return Err(ParserError::UnexpectedEOF);
        }
        
        let token_type = self.tokens[self.position].token_type.clone();
        
        match token_type {
            TokenType::String(value) => {
                self.advance();
                Ok(value)
            }
            _ => {
                let token = &self.tokens[self.position];
                Err(ParserError::UnexpectedToken(
                    token.value.clone(),
                    token.location.line,
                    token.location.column,
                ))
            }
        }
    }
}