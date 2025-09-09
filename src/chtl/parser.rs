//! CHTL parser
//! 
//! This module provides the syntax analysis functionality for CHTL,
//! including AST construction and semantic analysis.

use std::collections::VecDeque;
use crate::chtl::context::*;
use crate::chtl::lexer::*;

/// CHTL parser
#[derive(Debug, Clone)]
pub struct CHTLParser {
    /// Tokens to parse
    tokens: Vec<Token>,
    /// Current token index
    position: usize,
    /// Context for compilation
    context: CHTLContext,
    /// AST root node
    ast: Option<Node>,
}

/// Parser error
#[derive(Debug, Clone)]
pub struct ParseError {
    pub message: String,
    pub location: SourceLocation,
    pub suggestion: Option<String>,
}

impl CHTLParser {
    /// Create a new CHTL parser
    pub fn new(tokens: Vec<Token>, context: CHTLContext) -> Self {
        Self {
            tokens,
            position: 0,
            context,
            ast: None,
        }
    }
    
    /// Parse the tokens into an AST
    pub fn parse(&mut self) -> Result<Node, ParseError> {
        self.position = 0;
        self.ast = None;
        
        let root = self.parse_document()?;
        self.ast = Some(root.clone());
        
        Ok(root)
    }
    
    /// Parse a document (root level)
    fn parse_document(&mut self) -> Result<Node, ParseError> {
        let mut children = Vec::new();
        
        while !self.is_at_end() {
            if let Some(node) = self.parse_node()? {
                children.push(node);
            }
        }
        
        // Create a root element node
        Ok(Node::Element(ElementNode {
            tag_name: "document".to_string(),
            attributes: std::collections::HashMap::new(),
            children,
            style: None,
            script: None,
        }))
    }
    
    /// Parse a single node
    fn parse_node(&mut self) -> Result<Option<Node>, ParseError> {
        self.skip_whitespace_and_comments();
        
        if self.is_at_end() {
            return Ok(None);
        }
        
        let current_token = self.current_token();
        
        match &current_token.token_type {
            // Template definitions
            TokenType::Template => {
                self.parse_template_definition()
            }
            // Custom definitions
            TokenType::Custom => {
                self.parse_custom_definition()
            }
            // Origin definitions
            TokenType::Origin => {
                self.parse_origin_definition()
            }
            // Import statements
            TokenType::Import => {
                self.parse_import_statement()
            }
            // Namespace definitions
            TokenType::Namespace => {
                self.parse_namespace_definition()
            }
            // Configuration definitions
            TokenType::Configuration => {
                self.parse_configuration_definition()
            }
            // Use statements
            TokenType::Use => {
                self.parse_use_statement()
            }
            // HTML elements
            TokenType::Identifier(name) => {
                self.parse_element(name.clone())
            }
            // Comments
            TokenType::SingleLineComment(_) | TokenType::MultiLineComment(_) | TokenType::GeneratorComment(_) => {
                self.parse_comment()
            }
            _ => {
                Err(ParseError {
                    message: format!("Unexpected token: {:?}", current_token.token_type),
                    location: current_token.location.clone(),
                    suggestion: Some("Expected a valid CHTL construct".to_string()),
                })
            }
        }
    }
    
    /// Parse a template definition
    fn parse_template_definition(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Template]
        self.advance();
        
        // Parse template type (@Style, @Element, @Var)
        let template_type = self.parse_template_type()?;
        
        // Parse template name
        let name = self.parse_identifier()?;
        
        // Parse template content
        let content = self.parse_template_content(&template_type)?;
        
        Ok(Some(Node::Template(TemplateNode {
            template_type,
            name,
            content,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse template type
    fn parse_template_type(&mut self) -> Result<TemplateType, ParseError> {
        if self.current_token().token_type == TokenType::AtSymbol {
            self.advance();
            let type_name = self.parse_identifier()?;
            
            match type_name.as_str() {
                "Style" => Ok(TemplateType::Style),
                "Element" => Ok(TemplateType::Element),
                "Var" => Ok(TemplateType::Variable),
                _ => Err(ParseError {
                    message: format!("Unknown template type: {}", type_name),
                    location: self.current_token().location.clone(),
                    suggestion: Some("Expected @Style, @Element, or @Var".to_string()),
                }),
            }
        } else {
            Err(ParseError {
                message: "Expected @ symbol for template type".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Add @ before template type".to_string()),
            })
        }
    }
    
    /// Parse template content
    fn parse_template_content(&mut self, template_type: &TemplateType) -> Result<TemplateContent, ParseError> {
        self.expect_token(TokenType::LeftBrace)?;
        
        match template_type {
            TemplateType::Style => {
                let properties = self.parse_style_properties()?;
                Ok(TemplateContent::Style(properties))
            }
            TemplateType::Element => {
                let elements = self.parse_element_list()?;
                Ok(TemplateContent::Element(elements))
            }
            TemplateType::Variable => {
                let variables = self.parse_variable_definitions()?;
                Ok(TemplateContent::Variable(variables))
            }
        }
    }
    
    /// Parse a custom definition
    fn parse_custom_definition(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Custom]
        self.advance();
        
        // Parse custom type
        let custom_type = self.parse_custom_type()?;
        
        // Parse custom name
        let name = self.parse_identifier()?;
        
        // Parse custom content
        let content = self.parse_custom_content(&custom_type)?;
        
        Ok(Some(Node::Custom(CustomNode {
            custom_type,
            name,
            content,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse custom type
    fn parse_custom_type(&mut self) -> Result<CustomType, ParseError> {
        if self.current_token().token_type == TokenType::AtSymbol {
            self.advance();
            let type_name = self.parse_identifier()?;
            
            match type_name.as_str() {
                "Style" => Ok(CustomType::Style),
                "Element" => Ok(CustomType::Element),
                "Var" => Ok(CustomType::Variable),
                _ => Err(ParseError {
                    message: format!("Unknown custom type: {}", type_name),
                    location: self.current_token().location.clone(),
                    suggestion: Some("Expected @Style, @Element, or @Var".to_string()),
                }),
            }
        } else {
            Err(ParseError {
                message: "Expected @ symbol for custom type".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Add @ before custom type".to_string()),
            })
        }
    }
    
    /// Parse custom content
    fn parse_custom_content(&mut self, custom_type: &CustomType) -> Result<CustomContent, ParseError> {
        self.expect_token(TokenType::LeftBrace)?;
        
        match custom_type {
            CustomType::Style => {
                let properties = self.parse_style_properties()?;
                Ok(CustomContent::Style(properties))
            }
            CustomType::Element => {
                let elements = self.parse_element_list()?;
                Ok(CustomContent::Element(elements))
            }
            CustomType::Variable => {
                let variables = self.parse_variable_definitions()?;
                Ok(CustomContent::Variable(variables))
            }
        }
    }
    
    /// Parse an origin definition
    fn parse_origin_definition(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Origin]
        self.advance();
        
        // Parse origin type
        let origin_type = self.parse_origin_type()?;
        
        // Parse optional origin name
        let name = if self.current_token().token_type == TokenType::Identifier(_) {
            Some(self.parse_identifier()?)
        } else {
            None
        };
        
        // Parse origin content
        let content = self.parse_origin_content()?;
        
        Ok(Some(Node::Origin(OriginNode {
            origin_type,
            name,
            content,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse origin type
    fn parse_origin_type(&mut self) -> Result<OriginType, ParseError> {
        if self.current_token().token_type == TokenType::AtSymbol {
            self.advance();
            let type_name = self.parse_identifier()?;
            
            match type_name.as_str() {
                "Html" => Ok(OriginType::HTML),
                "Style" => Ok(OriginType::CSS),
                "JavaScript" => Ok(OriginType::JavaScript),
                custom => Ok(OriginType::Custom(custom.to_string())),
            }
        } else {
            Err(ParseError {
                message: "Expected @ symbol for origin type".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Add @ before origin type".to_string()),
            })
        }
    }
    
    /// Parse origin content
    fn parse_origin_content(&mut self) -> Result<String, ParseError> {
        self.expect_token(TokenType::LeftBrace)?;
        
        let mut content = String::new();
        let mut brace_count = 1;
        
        while !self.is_at_end() && brace_count > 0 {
            let token = self.current_token();
            
            match token.token_type {
                TokenType::LeftBrace => {
                    brace_count += 1;
                    content.push('{');
                }
                TokenType::RightBrace => {
                    brace_count -= 1;
                    if brace_count > 0 {
                        content.push('}');
                    }
                }
                _ => {
                    content.push_str(&token.value);
                }
            }
            
            self.advance();
        }
        
        if brace_count > 0 {
            Err(ParseError {
                message: "Unclosed origin content block".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Add closing } for origin content".to_string()),
            })
        } else {
            Ok(content)
        }
    }
    
    /// Parse an import statement
    fn parse_import_statement(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Import]
        self.advance();
        
        // Parse import type
        let import_type = self.parse_import_type()?;
        
        // Parse optional import name
        let name = if self.current_token().token_type == TokenType::Identifier(_) {
            Some(self.parse_identifier()?)
        } else {
            None
        };
        
        // Parse from keyword
        self.expect_keyword("from")?;
        
        // Parse import path
        let path = self.parse_import_path()?;
        
        // Parse optional alias
        let alias = if self.current_token().token_type == TokenType::Identifier("as".to_string()) {
            self.advance(); // Consume "as"
            Some(self.parse_identifier()?)
        } else {
            None
        };
        
        Ok(Some(Node::Import(ImportNode {
            import_type,
            name,
            path,
            alias,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse import type
    fn parse_import_type(&mut self) -> Result<ImportType, ParseError> {
        if self.current_token().token_type == TokenType::AtSymbol {
            self.advance();
            let type_name = self.parse_identifier()?;
            
            match type_name.as_str() {
                "Html" => Ok(ImportType::HTML),
                "Style" => Ok(ImportType::CSS),
                "JavaScript" => Ok(ImportType::JavaScript),
                "Chtl" => Ok(ImportType::CHTL),
                "CJmod" => Ok(ImportType::CJMOD),
                _ => Err(ParseError {
                    message: format!("Unknown import type: {}", type_name),
                    location: self.current_token().location.clone(),
                    suggestion: Some("Expected @Html, @Style, @JavaScript, @Chtl, or @CJmod".to_string()),
                }),
            }
        } else {
            Err(ParseError {
                message: "Expected @ symbol for import type".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Add @ before import type".to_string()),
            })
        }
    }
    
    /// Parse import path
    fn parse_import_path(&mut self) -> Result<String, ParseError> {
        match &self.current_token().token_type {
            TokenType::String(path) => {
                let path = path.clone();
                self.advance();
                Ok(path)
            }
            TokenType::Identifier(path) => {
                let path = path.clone();
                self.advance();
                Ok(path)
            }
            _ => Err(ParseError {
                message: "Expected import path".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Provide a valid file path or module name".to_string()),
            }),
        }
    }
    
    /// Parse a namespace definition
    fn parse_namespace_definition(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Namespace]
        self.advance();
        
        // Parse namespace name
        let name = self.parse_identifier()?;
        
        // Parse namespace content
        let content = if self.current_token().token_type == TokenType::LeftBrace {
            self.parse_namespace_content()?
        } else {
            Vec::new()
        };
        
        Ok(Some(Node::Namespace(NamespaceNode {
            name,
            content,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse namespace content
    fn parse_namespace_content(&mut self) -> Result<Vec<Node>, ParseError> {
        self.expect_token(TokenType::LeftBrace)?;
        
        let mut content = Vec::new();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
            if let Some(node) = self.parse_node()? {
                content.push(node);
            }
        }
        
        self.expect_token(TokenType::RightBrace)?;
        
        Ok(content)
    }
    
    /// Parse a configuration definition
    fn parse_configuration_definition(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume [Configuration]
        self.advance();
        
        // Parse optional configuration name
        let config_name = if self.current_token().token_type == TokenType::AtSymbol {
            self.advance();
            Some(self.parse_identifier()?)
        } else {
            None
        };
        
        // Parse configuration content
        let configuration = self.parse_configuration_content()?;
        
        Ok(Some(Node::Config(ConfigNode {
            config_name,
            configuration,
            location: self.current_token().location.clone(),
        })))
    }
    
    /// Parse configuration content
    fn parse_configuration_content(&mut self) -> Result<Configuration, ParseError> {
        self.expect_token(TokenType::LeftBrace)?;
        
        let mut config = Configuration::default();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
            let key = self.parse_identifier()?;
            self.expect_token(TokenType::Equal)?;
            let value = self.parse_configuration_value()?;
            self.expect_token(TokenType::Semicolon)?;
            
            // Set configuration value based on key
            match key.as_str() {
                "INDEX_INITIAL_COUNT" => {
                    if let Ok(count) = value.parse::<i32>() {
                        config.index_initial_count = count;
                    }
                }
                "DEBUG_MODE" => {
                    config.debug_mode = value == "true";
                }
                "DISABLE_NAME_GROUP" => {
                    config.disable_name_group = value == "true";
                }
                "DISABLE_STYLE_AUTO_ADD_CLASS" => {
                    config.disable_style_auto_add_class = value == "true";
                }
                "DISABLE_STYLE_AUTO_ADD_ID" => {
                    config.disable_style_auto_add_id = value == "true";
                }
                "DISABLE_DEFAULT_NAMESPACE" => {
                    config.disable_default_namespace = value == "true";
                }
                "DISABLE_CUSTOM_ORIGIN_TYPE" => {
                    config.disable_custom_origin_type = value == "true";
                }
                "DISABLE_SCRIPT_AUTO_ADD_CLASS" => {
                    config.disable_script_auto_add_class = value == "true";
                }
                "DISABLE_SCRIPT_AUTO_ADD_ID" => {
                    config.disable_script_auto_add_id = value == "true";
                }
                _ => {
                    // Handle other configuration keys
                }
            }
        }
        
        self.expect_token(TokenType::RightBrace)?;
        
        Ok(config)
    }
    
    /// Parse configuration value
    fn parse_configuration_value(&mut self) -> Result<String, ParseError> {
        match &self.current_token().token_type {
            TokenType::String(value) => {
                let value = value.clone();
                self.advance();
                Ok(value)
            }
            TokenType::Number(value) => {
                let value = value.to_string();
                self.advance();
                Ok(value)
            }
            TokenType::Identifier(value) => {
                let value = value.clone();
                self.advance();
                Ok(value)
            }
            _ => Err(ParseError {
                message: "Expected configuration value".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Provide a string, number, or identifier".to_string()),
            }),
        }
    }
    
    /// Parse a use statement
    fn parse_use_statement(&mut self) -> Result<Option<Node>, ParseError> {
        // Consume "use"
        self.advance();
        
        // Parse use target
        let target = self.parse_identifier()?;
        
        // Handle different use types
        match target.as_str() {
            "html5" => {
                // HTML5 declaration
                Ok(None) // This is handled by the generator
            }
            _ => {
                // Configuration use
                Ok(None) // This is handled by the context
            }
        }
    }
    
    /// Parse an element
    fn parse_element(&mut self, tag_name: String) -> Result<Option<Node>, ParseError> {
        let mut attributes = std::collections::HashMap::new();
        let mut children = Vec::new();
        let mut style = None;
        let mut script = None;
        
        // Parse attributes
        while !self.is_at_end() && self.current_token().token_type != TokenType::LeftBrace {
            let attr_name = self.parse_identifier()?;
            self.expect_token(TokenType::Colon)?;
            let attr_value = self.parse_attribute_value()?;
            self.expect_token(TokenType::Semicolon)?;
            
            attributes.insert(attr_name, attr_value);
        }
        
        // Parse element content
        if self.current_token().token_type == TokenType::LeftBrace {
            self.advance(); // Consume {
            
            while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
                if let Some(node) = self.parse_node()? {
                    children.push(node);
                }
            }
            
            self.expect_token(TokenType::RightBrace)?;
        }
        
        Ok(Some(Node::Element(ElementNode {
            tag_name,
            attributes,
            children,
            style,
            script,
        })))
    }
    
    /// Parse attribute value
    fn parse_attribute_value(&mut self) -> Result<String, ParseError> {
        match &self.current_token().token_type {
            TokenType::String(value) => {
                let value = value.clone();
                self.advance();
                Ok(value)
            }
            TokenType::Identifier(value) => {
                let value = value.clone();
                self.advance();
                Ok(value)
            }
            TokenType::UnquotedLiteral(value) => {
                let value = value.clone();
                self.advance();
                Ok(value)
            }
            _ => Err(ParseError {
                message: "Expected attribute value".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Provide a string, identifier, or unquoted literal".to_string()),
            }),
        }
    }
    
    /// Parse a comment
    fn parse_comment(&mut self) -> Result<Option<Node>, ParseError> {
        let token = self.current_token();
        let comment_type = match token.token_type {
            TokenType::SingleLineComment(_) => CommentType::SingleLine,
            TokenType::MultiLineComment(_) => CommentType::MultiLine,
            TokenType::GeneratorComment(_) => CommentType::Generator,
            _ => return Err(ParseError {
                message: "Expected comment token".to_string(),
                location: token.location.clone(),
                suggestion: None,
            }),
        };
        
        let content = match &token.token_type {
            TokenType::SingleLineComment(content) => content.clone(),
            TokenType::MultiLineComment(content) => content.clone(),
            TokenType::GeneratorComment(content) => content.clone(),
            _ => String::new(),
        };
        
        self.advance();
        
        Ok(Some(Node::Comment(CommentNode {
            content,
            comment_type,
            location: token.location.clone(),
        })))
    }
    
    /// Parse style properties
    fn parse_style_properties(&mut self) -> Result<Vec<StyleProperty>, ParseError> {
        let mut properties = Vec::new();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
            let name = self.parse_identifier()?;
            self.expect_token(TokenType::Colon)?;
            let value = self.parse_style_value()?;
            self.expect_token(TokenType::Semicolon)?;
            
            properties.push(StyleProperty {
                name,
                value,
                important: false, // TODO: Handle !important
            });
        }
        
        Ok(properties)
    }
    
    /// Parse style value
    fn parse_style_value(&mut self) -> Result<String, ParseError> {
        let mut value = String::new();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::Semicolon {
            value.push_str(&self.current_token().value);
            self.advance();
        }
        
        Ok(value.trim().to_string())
    }
    
    /// Parse element list
    fn parse_element_list(&mut self) -> Result<Vec<ElementNode>, ParseError> {
        let mut elements = Vec::new();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
            if let Some(Node::Element(element)) = self.parse_node()? {
                elements.push(element);
            }
        }
        
        Ok(elements)
    }
    
    /// Parse variable definitions
    fn parse_variable_definitions(&mut self) -> Result<Vec<VariableDefinition>, ParseError> {
        let mut variables = Vec::new();
        
        while !self.is_at_end() && self.current_token().token_type != TokenType::RightBrace {
            let name = self.parse_identifier()?;
            self.expect_token(TokenType::Colon)?;
            let value = self.parse_attribute_value()?;
            self.expect_token(TokenType::Semicolon)?;
            
            variables.push(VariableDefinition {
                name,
                value,
                variable_type: VariableType::String, // TODO: Infer type
            });
        }
        
        Ok(variables)
    }
    
    /// Parse identifier
    fn parse_identifier(&mut self) -> Result<String, ParseError> {
        match &self.current_token().token_type {
            TokenType::Identifier(name) => {
                let name = name.clone();
                self.advance();
                Ok(name)
            }
            _ => Err(ParseError {
                message: "Expected identifier".to_string(),
                location: self.current_token().location.clone(),
                suggestion: Some("Provide a valid identifier".to_string()),
            }),
        }
    }
    
    /// Expect a specific token type
    fn expect_token(&mut self, expected: TokenType) -> Result<(), ParseError> {
        if self.current_token().token_type == expected {
            self.advance();
            Ok(())
        } else {
            Err(ParseError {
                message: format!("Expected {:?}, found {:?}", expected, self.current_token().token_type),
                location: self.current_token().location.clone(),
                suggestion: Some(format!("Change to {:?}", expected)),
            })
        }
    }
    
    /// Expect a specific keyword
    fn expect_keyword(&mut self, keyword: &str) -> Result<(), ParseError> {
        if let TokenType::Identifier(name) = &self.current_token().token_type {
            if name == keyword {
                self.advance();
                Ok(())
            } else {
                Err(ParseError {
                    message: format!("Expected keyword '{}', found '{}'", keyword, name),
                    location: self.current_token().location.clone(),
                    suggestion: Some(format!("Change to '{}'", keyword)),
                })
            }
        } else {
            Err(ParseError {
                message: format!("Expected keyword '{}'", keyword),
                location: self.current_token().location.clone(),
                suggestion: Some(format!("Add keyword '{}'", keyword)),
            })
        }
    }
    
    /// Skip whitespace and comments
    fn skip_whitespace_and_comments(&mut self) {
        while !self.is_at_end() {
            match self.current_token().token_type {
                TokenType::Whitespace | TokenType::Newline | TokenType::Tab => {
                    self.advance();
                }
                TokenType::SingleLineComment(_) | TokenType::MultiLineComment(_) | TokenType::GeneratorComment(_) => {
                    self.advance();
                }
                _ => break,
            }
        }
    }
    
    /// Check if we're at the end of tokens
    fn is_at_end(&self) -> bool {
        self.position >= self.tokens.len() || self.current_token().token_type == TokenType::EndOfFile
    }
    
    /// Get current token
    fn current_token(&self) -> &Token {
        if self.position < self.tokens.len() {
            &self.tokens[self.position]
        } else {
            &self.tokens.last().unwrap()
        }
    }
    
    /// Advance to next token
    fn advance(&mut self) {
        if self.position < self.tokens.len() {
            self.position += 1;
        }
    }
    
    /// Get the AST
    pub fn get_ast(&self) -> Option<&Node> {
        self.ast.as_ref()
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