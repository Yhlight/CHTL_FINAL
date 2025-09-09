//! Parser for CHTL

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use crate::lexer::{Lexer, Token, TokenType};
use std::collections::HashMap;

/// CHTL parser
pub struct Parser {
    tokens: Vec<Token>,
    current: usize,
    source: String,
}

impl Parser {
    /// Create a new parser
    pub fn new(tokens: Vec<Token>, source: String) -> Self {
        Self {
            tokens,
            current: 0,
            source,
        }
    }
    
    /// Parse the tokens into an AST
    pub fn parse(&mut self) -> Result<Document> {
        let mut children = Vec::new();
        let mut configuration = None;
        let mut namespace = None;
        
        while !self.is_at_end() {
            match self.peek().token_type {
                TokenType::Configuration => {
                    configuration = Some(self.parse_configuration()?);
                }
                TokenType::Namespace => {
                    namespace = Some(self.parse_namespace()?);
                }
                _ => {
                    if let Some(node) = self.parse_node()? {
                        children.push(node);
                    }
                }
            }
        }
        
        Ok(Document {
            children,
            configuration,
            namespace,
        })
    }
    
    fn parse_node(&mut self) -> Result<Option<AstNode>> {
        match self.peek().token_type {
            TokenType::Template => self.parse_template(),
            TokenType::Custom => self.parse_custom(),
            TokenType::Import => self.parse_import(),
            TokenType::Origin => self.parse_origin(),
            TokenType::Identifier(_) => self.parse_element(),
            TokenType::Text => self.parse_text_node(),
            TokenType::LineComment | TokenType::BlockComment | TokenType::GeneratorComment => {
                self.advance();
                Ok(None)
            }
            TokenType::Whitespace | TokenType::Newline => {
                self.advance();
                Ok(None)
            }
            _ => Ok(None),
        }
    }
    
    fn parse_element(&mut self) -> Result<Option<AstNode>> {
        let tag_token = self.advance();
        let tag_name = match &tag_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(ChtlError::parse("Expected element identifier")),
        };
        
        let mut attributes = HashMap::new();
        let mut children = Vec::new();
        let mut style = None;
        let mut script = None;
        
        // Parse attributes and content
        if self.check(TokenType::LeftBrace) {
            self.advance(); // consume {
            
            while !self.check(TokenType::RightBrace) && !self.is_at_end() {
                match self.peek().token_type {
                    TokenType::Text => {
                        if let Some(text_node) = self.parse_text_node()? {
                            children.push(text_node);
                        }
                    }
                    TokenType::Style => {
                        style = Some(self.parse_style()?);
                    }
                    TokenType::Script => {
                        script = Some(self.parse_script()?);
                    }
                    TokenType::Identifier(_) => {
                        if let Some(child_element) = self.parse_element()? {
                            children.push(child_element);
                        }
                    }
                    TokenType::Colon | TokenType::Equal => {
                        // Parse attribute
                        if let Some((key, value)) = self.parse_attribute()? {
                            attributes.insert(key, value);
                        }
                    }
                    _ => {
                        self.advance();
                    }
                }
            }
            
            if !self.check(TokenType::RightBrace) {
                return Err(ChtlError::parse("Expected '}' after element"));
            }
            self.advance(); // consume }
        }
        
        Ok(Some(AstNode::Element(ElementNode {
            tag_name,
            attributes,
            children,
            style,
            script,
        })))
    }
    
    fn parse_text_node(&mut self) -> Result<Option<AstNode>> {
        let text_token = self.advance();
        let content = match &text_token.token_type {
            TokenType::Text => "".to_string(),
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            _ => return Ok(None),
        };
        
        let mut is_literal = false;
        
        // Check if it's a text block
        if self.check(TokenType::LeftBrace) {
            self.advance(); // consume {
            
            let mut text_content = String::new();
            while !self.check(TokenType::RightBrace) && !self.is_at_end() {
                let token = self.advance();
                match &token.token_type {
                    TokenType::String(s) => text_content.push_str(s),
                    TokenType::Identifier(s) => {
                        text_content.push_str(s);
                        is_literal = true;
                    }
                    TokenType::Whitespace => text_content.push(' '),
                    TokenType::Newline => text_content.push('\n'),
                    _ => {}
                }
            }
            
            if !self.check(TokenType::RightBrace) {
                return Err(ChtlError::parse("Expected '}' after text block"));
            }
            self.advance(); // consume }
            
            Ok(Some(AstNode::Text(TextNode {
                content: text_content,
                is_literal,
            })))
        } else {
            Ok(Some(AstNode::Text(TextNode {
                content,
                is_literal: matches!(text_token.token_type, TokenType::Identifier(_)),
            })))
        }
    }
    
    fn parse_attribute(&mut self) -> Result<Option<(String, String)>> {
        let key_token = self.advance();
        let key = match &key_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Ok(None),
        };
        
        // Skip : or =
        if self.check(TokenType::Colon) || self.check(TokenType::Equal) {
            self.advance();
        }
        
        let value_token = self.advance();
        let value = match &value_token.token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            TokenType::Number(s) => s.clone(),
            _ => return Ok(None),
        };
        
        // Skip semicolon if present
        if self.check(TokenType::Semicolon) {
            self.advance();
        }
        
        Ok(Some((key, value)))
    }
    
    fn parse_style(&mut self) -> Result<StyleNode> {
        self.advance(); // consume 'style'
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after style"));
        }
        self.advance(); // consume {
        
        let mut properties = HashMap::new();
        let mut selectors = Vec::new();
        let mut conditions = Vec::new();
        
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            match self.peek().token_type {
                TokenType::Identifier(_) => {
                    if let Some((key, value)) = self.parse_style_property()? {
                        properties.insert(key, StyleValue::Simple(value));
                    }
                }
                TokenType::Dot => {
                    if let Some(selector) = self.parse_class_selector()? {
                        selectors.push(selector);
                    }
                }
                TokenType::Colon if self.peek_next().map_or(false, |t| matches!(t.token_type, TokenType::Colon)) => {
                    if let Some(selector) = self.parse_pseudo_selector()? {
                        selectors.push(selector);
                    }
                }
                _ => {
                    self.advance();
                }
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after style block"));
        }
        self.advance(); // consume }
        
        Ok(StyleNode {
            properties,
            selectors,
            conditions,
        })
    }
    
    fn parse_style_property(&mut self) -> Result<Option<(String, String)>> {
        let key_token = self.advance();
        let key = match &key_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Ok(None),
        };
        
        // Skip :
        if self.check(TokenType::Colon) {
            self.advance();
        }
        
        let value_token = self.advance();
        let value = match &value_token.token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            TokenType::Number(s) => s.clone(),
            _ => return Ok(None),
        };
        
        // Skip semicolon if present
        if self.check(TokenType::Semicolon) {
            self.advance();
        }
        
        Ok(Some((key, value)))
    }
    
    fn parse_class_selector(&mut self) -> Result<Option<StyleSelector>> {
        self.advance(); // consume .
        
        let name_token = self.advance();
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Ok(None),
        };
        
        Ok(Some(StyleSelector::Class(name)))
    }
    
    fn parse_pseudo_selector(&mut self) -> Result<Option<StyleSelector>> {
        self.advance(); // consume :
        self.advance(); // consume second :
        
        let name_token = self.advance();
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Ok(None),
        };
        
        Ok(Some(StyleSelector::PseudoElement(name)))
    }
    
    fn parse_script(&mut self) -> Result<ScriptNode> {
        self.advance(); // consume 'script'
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after script"));
        }
        self.advance(); // consume {
        
        let mut content = String::new();
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            let token = self.advance();
            match &token.token_type {
                TokenType::String(s) => content.push_str(s),
                TokenType::Identifier(s) => content.push_str(s),
                TokenType::Whitespace => content.push(' '),
                TokenType::Newline => content.push('\n'),
                _ => {}
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after script block"));
        }
        self.advance(); // consume }
        
        Ok(ScriptNode {
            content,
            is_chtl_js: true, // TODO: Determine if it's CHTL JS
        })
    }
    
    fn parse_template(&mut self) -> Result<Option<AstNode>> {
        self.advance(); // consume [Template]
        
        let template_type = match self.peek().token_type {
            TokenType::AtStyle => {
                self.advance();
                TemplateType::Style
            }
            TokenType::AtElement => {
                self.advance();
                TemplateType::Element
            }
            TokenType::AtVar => {
                self.advance();
                TemplateType::Var
            }
            _ => return Err(ChtlError::parse("Expected @Style, @Element, or @Var after [Template]")),
        };
        
        let name_token = self.advance();
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(ChtlError::parse("Expected template name")),
        };
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after template name"));
        }
        self.advance(); // consume {
        
        let mut content = Vec::new();
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            if let Some(node) = self.parse_node()? {
                content.push(node);
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after template"));
        }
        self.advance(); // consume }
        
        Ok(Some(AstNode::Template(TemplateNode {
            template_type,
            name,
            content,
            parameters: Vec::new(),
        })))
    }
    
    fn parse_custom(&mut self) -> Result<Option<AstNode>> {
        self.advance(); // consume [Custom]
        
        let custom_type = match self.peek().token_type {
            TokenType::AtStyle => {
                self.advance();
                CustomType::Style
            }
            TokenType::AtElement => {
                self.advance();
                CustomType::Element
            }
            TokenType::AtVar => {
                self.advance();
                CustomType::Var
            }
            _ => return Err(ChtlError::parse("Expected @Style, @Element, or @Var after [Custom]")),
        };
        
        let name_token = self.advance();
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(ChtlError::parse("Expected custom name")),
        };
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after custom name"));
        }
        self.advance(); // consume {
        
        let mut content = Vec::new();
        let mut specializations = Vec::new();
        
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            if let Some(node) = self.parse_node()? {
                content.push(node);
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after custom"));
        }
        self.advance(); // consume }
        
        Ok(Some(AstNode::Custom(CustomNode {
            custom_type,
            name,
            content,
            specializations,
        })))
    }
    
    fn parse_import(&mut self) -> Result<Option<AstNode>> {
        self.advance(); // consume [Import]
        
        let import_type = match self.peek().token_type {
            TokenType::AtHtml => {
                self.advance();
                ImportType::Html
            }
            TokenType::AtStyle => {
                self.advance();
                ImportType::Style
            }
            TokenType::AtJavaScript => {
                self.advance();
                ImportType::JavaScript
            }
            TokenType::AtChtl => {
                self.advance();
                ImportType::Chtl
            }
            TokenType::AtCJmod => {
                self.advance();
                ImportType::CJmod
            }
            TokenType::AtConfig => {
                self.advance();
                ImportType::Config
            }
            _ => return Err(ChtlError::parse("Expected import type after [Import]")),
        };
        
        if !self.check(TokenType::From) {
            return Err(ChtlError::parse("Expected 'from' after import type"));
        }
        self.advance(); // consume 'from'
        
        let path_token = self.advance();
        let path = match &path_token.token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            _ => return Err(ChtlError::parse("Expected import path")),
        };
        
        let mut alias = None;
        if self.check(TokenType::As) {
            self.advance(); // consume 'as'
            let alias_token = self.advance();
            alias = match &alias_token.token_type {
                TokenType::Identifier(name) => Some(name.clone()),
                _ => return Err(ChtlError::parse("Expected alias name")),
            };
        }
        
        Ok(Some(AstNode::Import(ImportNode {
            import_type,
            path,
            alias,
            target: None,
        })))
    }
    
    fn parse_origin(&mut self) -> Result<Option<AstNode>> {
        self.advance(); // consume [Origin]
        
        let origin_type = match self.peek().token_type {
            TokenType::AtHtml => {
                self.advance();
                OriginType::Html
            }
            TokenType::AtStyle => {
                self.advance();
                OriginType::Style
            }
            TokenType::AtJavaScript => {
                self.advance();
                OriginType::JavaScript
            }
            TokenType::Identifier(name) => {
                self.advance();
                OriginType::Custom(name.clone())
            }
            _ => return Err(ChtlError::parse("Expected origin type after [Origin]")),
        };
        
        let mut name = None;
        if matches!(self.peek().token_type, TokenType::Identifier(_)) {
            let name_token = self.advance();
            name = match &name_token.token_type {
                TokenType::Identifier(n) => Some(n.clone()),
                _ => None,
            };
        }
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after origin"));
        }
        self.advance(); // consume {
        
        let mut content = String::new();
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            let token = self.advance();
            match &token.token_type {
                TokenType::String(s) => content.push_str(s),
                TokenType::Identifier(s) => content.push_str(s),
                TokenType::Whitespace => content.push(' '),
                TokenType::Newline => content.push('\n'),
                _ => {}
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after origin"));
        }
        self.advance(); // consume }
        
        Ok(Some(AstNode::Origin(OriginNode {
            origin_type,
            name,
            content,
        })))
    }
    
    fn parse_configuration(&mut self) -> Result<Configuration> {
        self.advance(); // consume [Configuration]
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after [Configuration]"));
        }
        self.advance(); // consume {
        
        let mut settings = HashMap::new();
        let mut name_config = None;
        let mut origin_types = Vec::new();
        
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            match self.peek().token_type {
                TokenType::Identifier(_) => {
                    if let Some((key, value)) = self.parse_setting()? {
                        settings.insert(key, value);
                    }
                }
                TokenType::Name => {
                    name_config = Some(self.parse_name_configuration()?);
                }
                _ => {
                    self.advance();
                }
            }
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after configuration"));
        }
        self.advance(); // consume }
        
        Ok(Configuration {
            settings,
            name_config,
            origin_types,
        })
    }
    
    fn parse_setting(&mut self) -> Result<Option<(String, String)>> {
        let key_token = self.advance();
        let key = match &key_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Ok(None),
        };
        
        // Skip =
        if self.check(TokenType::Equal) {
            self.advance();
        }
        
        let value_token = self.advance();
        let value = match &value_token.token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Identifier(s) => s.clone(),
            TokenType::Number(s) => s.clone(),
            _ => return Ok(None),
        };
        
        // Skip semicolon if present
        if self.check(TokenType::Semicolon) {
            self.advance();
        }
        
        Ok(Some((key, value)))
    }
    
    fn parse_name_configuration(&mut self) -> Result<NameConfiguration> {
        // Skip [Name] token
        self.advance();
        
        if !self.check(TokenType::LeftBrace) {
            return Err(ChtlError::parse("Expected '{' after [Name]"));
        }
        self.advance(); // consume {
        
        // TODO: Parse actual name configuration
        while !self.check(TokenType::RightBrace) && !self.is_at_end() {
            self.advance();
        }
        
        if !self.check(TokenType::RightBrace) {
            return Err(ChtlError::parse("Expected '}' after name configuration"));
        }
        self.advance(); // consume }
        
        // Return default configuration
        Ok(NameConfiguration {
            custom_style: vec!["@Style".to_string()],
            custom_element: "@Element".to_string(),
            custom_var: "@Var".to_string(),
            template_style: "@Style".to_string(),
            template_element: "@Element".to_string(),
            template_var: "@Var".to_string(),
            origin_html: "@Html".to_string(),
            origin_style: "@Style".to_string(),
            origin_javascript: "@JavaScript".to_string(),
            import_html: "@Html".to_string(),
            import_style: "@Style".to_string(),
            import_javascript: "@JavaScript".to_string(),
            import_chtl: "@Chtl".to_string(),
            import_cjmod: "@CJmod".to_string(),
            keyword_inherit: "inherit".to_string(),
            keyword_delete: "delete".to_string(),
            keyword_insert: "insert".to_string(),
            keyword_after: "after".to_string(),
            keyword_before: "before".to_string(),
            keyword_replace: "replace".to_string(),
            keyword_at_top: "at top".to_string(),
            keyword_at_bottom: "at bottom".to_string(),
            keyword_from: "from".to_string(),
            keyword_as: "as".to_string(),
            keyword_except: "except".to_string(),
            keyword_use: "use".to_string(),
            keyword_html5: "html5".to_string(),
            keyword_text: "text".to_string(),
            keyword_style: "style".to_string(),
            keyword_script: "script".to_string(),
            keyword_custom: "[Custom]".to_string(),
            keyword_template: "[Template]".to_string(),
            keyword_origin: "[Origin]".to_string(),
            keyword_import: "[Import]".to_string(),
            keyword_namespace: "[Namespace]".to_string(),
        })
    }
    
    fn parse_namespace(&mut self) -> Result<NamespaceNode> {
        self.advance(); // consume [Namespace]
        
        let name_token = self.advance();
        let name = match &name_token.token_type {
            TokenType::Identifier(name) => name.clone(),
            _ => return Err(ChtlError::parse("Expected namespace name")),
        };
        
        let mut content = Vec::new();
        
        if self.check(TokenType::LeftBrace) {
            self.advance(); // consume {
            
            while !self.check(TokenType::RightBrace) && !self.is_at_end() {
                if let Some(node) = self.parse_node()? {
                    content.push(node);
                }
            }
            
            if !self.check(TokenType::RightBrace) {
                return Err(ChtlError::parse("Expected '}' after namespace"));
            }
            self.advance(); // consume }
        }
        
        Ok(NamespaceNode {
            name,
            content,
            parent: None,
        })
    }
    
    // Helper methods
    fn is_at_end(&self) -> bool {
        self.current >= self.tokens.len() - 1
    }
    
    fn peek(&self) -> &Token {
        &self.tokens[self.current]
    }
    
    fn peek_next(&self) -> Option<&Token> {
        if self.current + 1 < self.tokens.len() {
            Some(&self.tokens[self.current + 1])
        } else {
            None
        }
    }
    
    fn advance(&mut self) -> &Token {
        if !self.is_at_end() {
            self.current += 1;
        }
        self.previous()
    }
    
    fn previous(&self) -> &Token {
        &self.tokens[self.current - 1]
    }
    
    fn check(&self, token_type: TokenType) -> bool {
        if self.is_at_end() {
            false
        } else {
            std::mem::discriminant(&self.peek().token_type) == std::mem::discriminant(&token_type)
        }
    }
}