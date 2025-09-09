use crate::chtl_compiler::lexer::{Lexer, Token};
use crate::chtl_compiler::node::{Node, ElementNode, TextNode, CommentNode, Attribute, StyleNode, TemplateDefinitionNode, StyleContent};
use crate::chtl_compiler::loader::Loader;
use std::collections::HashMap;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    loader: Loader,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
    templates: HashMap<String, TemplateDefinitionNode>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            loader: Loader::new(),
            current_token: Token::Illegal("".to_string()),
            peek_token: Token::Illegal("".to_string()),
            errors: Vec::new(),
            templates: HashMap::new(),
        };
        // Read two tokens, so current_token and peek_token are both set
        p.next_token();
        p.next_token();
        p
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    pub fn templates(&self) -> &HashMap<String, TemplateDefinitionNode> {
        &self.templates
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Vec<Node> {
        let mut program: Vec<Node> = Vec::new();

        while self.current_token != Token::EndOfFile {
            if let Some(node) = self.parse_statement() {
                program.push(node);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Node> {
        match &self.current_token {
            Token::Identifier(name) => {
                if name == "text" && self.peek_token_is(&Token::OpenBrace) {
                    return self.parse_text_block();
                } else if name == "style" && self.peek_token_is(&Token::OpenBrace) {
                    return self.parse_style_block();
                }
                self.parse_element()
            }
            Token::LineComment(content) => Some(Node::Comment(CommentNode { content: content.clone(), is_generatable: false })),
            Token::BlockComment(content) => Some(Node::Comment(CommentNode { content: content.clone(), is_generatable: false })),
            Token::GeneratorComment(content) => Some(Node::Comment(CommentNode { content: content.clone(), is_generatable: true })),
            Token::TemplateKeyword => self.parse_template_definition(),
            Token::ImportKeyword => self.parse_import_statement(),
            Token::At => self.parse_template_usage(),
            _ => None,
        }
    }

    fn parse_element(&mut self) -> Option<Node> {
        let tag_name = match &self.current_token {
            Token::Identifier(name) => name.clone(),
            _ => {
                self.errors.push(format!("Expected identifier, got {:?}", self.current_token));
                return None;
            }
        };

        if !self.expect_peek(Token::OpenBrace) {
            return None;
        }
        self.next_token(); // Consume '{' to start parsing the body

        let mut attributes = Vec::new();
        let mut children = Vec::new();

        while !self.current_token_is(&Token::CloseBrace) && !self.current_token_is(&Token::EndOfFile) {
            match self.current_token {
                // It could be an attribute or a nested element. Both start with an Identifier.
                Token::Identifier(_) => {
                    if self.peek_token_is(&Token::Colon) || self.peek_token_is(&Token::Equal) {
                        if let Some(attr) = self.parse_attribute() {
                            // The `text` attribute is special and will be converted to a child node by the generator.
                            attributes.push(attr);
                        }
                    } else if self.peek_token_is(&Token::OpenBrace) {
                        if let Some(child) = self.parse_statement() {
                            children.push(child);
                        }
                    } else {
                        // This could be an unquoted literal for a `text` attribute that was omitted.
                        // For now, we'll error.
                        self.errors.push(format!("Unexpected token after identifier in element body: {:?}", self.peek_token));
                    }
                }
                // Handle comments inside an element body
                Token::LineComment(_) | Token::BlockComment(_) | Token::GeneratorComment(_) => {
                    if let Some(child) = self.parse_statement() {
                        children.push(child);
                    }
                }
                Token::At => {
                    if let Some(child) = self.parse_statement() {
                        children.push(child);
                    }
                }
                _ => {
                    self.errors.push(format!("Unexpected token inside element body: {:?}", self.current_token));
                }
            }
            self.next_token(); // Move to the next statement/attribute in the body
        }

        if !self.current_token_is(&Token::CloseBrace) {
             self.errors.push(format!("Expected '}}', got {:?}", self.current_token));
             return None;
        }

        Some(Node::Element(ElementNode {
            tag_name,
            attributes,
            children,
        }))
    }

    fn parse_attribute(&mut self) -> Option<Attribute> {
        let name = match &self.current_token {
            Token::Identifier(n) => n.clone(),
            _ => {
                // This case should not be reached if called correctly
                self.errors.push(format!("Expected identifier for attribute name, got {:?}", self.current_token));
                return None;
            }
        };

        self.next_token(); // consume attribute name, current_token is now ':' or '='
        if !self.current_token_is(&Token::Colon) && !self.current_token_is(&Token::Equal) {
            self.errors.push(format!("Expected ':' or '=' after attribute name, got {:?}", self.current_token));
            return None;
        }

        self.next_token(); // consume ':' or '=', current_token is now the value

        let value = match &self.current_token {
            Token::StringLiteral(s) => s.clone(),
            Token::Identifier(s) => s.clone(), // For unquoted literals
            _ => {
                self.errors.push(format!("Expected attribute value (string or identifier), got {:?}", self.current_token));
                return None;
            }
        };

        // Attributes are optionally terminated by a semicolon.
        if self.peek_token_is(&Token::Semicolon) {
            self.next_token(); // consume value
            // current_token is now the semicolon, the loop will advance past it.
        }

        Some(Attribute { name, value })
    }

    fn parse_text_block(&mut self) -> Option<Node> {
        // current_token is Identifier("text")
        self.next_token(); // consume "text", current_token is now "{"

        if !self.current_token_is(&Token::OpenBrace) {
            self.errors.push(format!(
                "Expected '{{' after 'text' keyword, got {:?}",
                self.current_token
            ));
            return None;
        }

        self.next_token(); // consume "{", current_token is now content

        let content = match &self.current_token {
            Token::StringLiteral(s) => s.clone(),
            Token::Identifier(s) => s.clone(), // For unquoted literals
            _ => {
                self.errors.push(format!(
                    "Expected text block content (string or identifier), got {:?}",
                    self.current_token
                ));
                return None;
            }
        };

        self.next_token(); // consume content, current_token is now "}"

        if !self.current_token_is(&Token::CloseBrace) {
            self.errors.push(format!(
                "Expected '}}' after text content, got {:?}",
                self.current_token
            ));
            return None;
        }

        Some(Node::Text(TextNode { content }))
    }

    fn parse_style_block(&mut self) -> Option<Node> {
        // current_token is Identifier("style")
        self.next_token(); // consume "style", current_token is now "{"

        if !self.current_token_is(&Token::OpenBrace) {
            self.errors.push(format!(
                "Expected '{{' after 'style' keyword, got {:?}",
                self.current_token
            ));
            return None;
        }
        self.next_token(); // consume "{", current_token is now the first property or "}"

        let mut content = Vec::new();

        while !self.current_token_is(&Token::CloseBrace) && !self.current_token_is(&Token::EndOfFile) {
            match self.current_token.clone() {
                Token::Identifier(name) => {
                    self.next_token(); // consume property name
                    if !self.current_token_is(&Token::Colon) {
                        self.errors.push(format!("Expected ':' after style property name, got {:?}", self.current_token));
                        break;
                    }
                    self.next_token(); // consume ":"

                    let value = self.parse_property_value();
                    let prop = crate::chtl_compiler::node::StyleProperty { name, value };
                    content.push(StyleContent::Property(prop));

                    // parse_property_value consumes the semicolon if it exists,
                    // so we don't need to advance the token here.
                }
                Token::At => {
                    if let Some(Node::TemplateUsage(usage)) = self.parse_template_usage() {
                        if usage.kind == crate::chtl_compiler::node::TemplateKind::Style {
                            content.push(StyleContent::TemplateUsage(usage));
                        } else {
                            self.errors.push(format!("Only @Style templates can be used inside a style block, got @{:?}", usage.kind));
                        }
                    }
                    // parse_template_usage consumes the semicolon
                    self.next_token();
                }
                _ => {
                    self.errors.push(format!("Expected identifier or '@' for style property, got {:?}", self.current_token));
                    self.next_token(); // prevent infinite loop
                }
            }
        }

        if !self.current_token_is(&Token::CloseBrace) {
            self.errors.push(format!("Expected '}}' to close style block, got {:?}", self.current_token));
            return None;
        }

        Some(Node::Style(StyleNode { content }))
    }

    fn parse_property_value(&mut self) -> crate::chtl_compiler::node::PropertyValue {
        // This is a simplified expression parser. It does not handle operator precedence.
        // It reads tokens until it hits a '?' or ';'.
        let mut condition_parts = Vec::new();
        while !self.current_token_is(&Token::Question)
              && !self.current_token_is(&Token::Semicolon)
              && !self.current_token_is(&Token::CloseBrace)
              && !self.current_token_is(&Token::Colon)
        {
            let part = match &self.current_token {
                Token::Identifier(s) => s.clone(),
                Token::StringLiteral(s) => format!("\"{}\"", s), // keep quotes for strings
                Token::GreaterThan => ">".to_string(),
                Token::LessThan => "<".to_string(),
                _ => "".to_string(),
            };
            if !part.is_empty() {
                condition_parts.push(part);
            }
            self.next_token();
        }

        let value = if !self.current_token_is(&Token::Question) {
            crate::chtl_compiler::node::PropertyValue::Literal(condition_parts.join(" "))
        } else {
            // It's a conditional expression
            self.next_token(); // consume '?'

            let true_value = self.parse_property_value();

            let false_value = if self.current_token_is(&Token::Colon) {
                self.next_token(); // consume ':'
                Some(Box::new(self.parse_property_value()))
            } else {
                None
            };

            let condition = crate::chtl_compiler::node::ConditionalExpression {
                condition: condition_parts.join(" "),
                true_value: Box::new(true_value),
                false_value,
            };

            crate::chtl_compiler::node::PropertyValue::Conditional(condition)
        };

        // A property value can be terminated by a semicolon or the closing brace of the style block.
        if self.current_token_is(&Token::Semicolon) {
            self.next_token();
        }

        value
    }

    fn parse_template_definition(&mut self) -> Option<Node> {
        // current_token is [Template]
        self.next_token(); // consume [Template], current is now @

        if !self.current_token_is(&Token::At) {
            self.errors.push(format!("Expected '@' after [Template], got {:?}", self.current_token));
            return None;
        }
        self.next_token(); // consume @, current is now type (Style, Element, etc.)

        let kind = match self.current_token {
            Token::Identifier(ref s) if s == "Style" => crate::chtl_compiler::node::TemplateKind::Style,
            Token::Identifier(ref s) if s == "Element" => crate::chtl_compiler::node::TemplateKind::Element,
            Token::Identifier(ref s) if s == "Var" => crate::chtl_compiler::node::TemplateKind::Var,
            _ => {
                self.errors.push(format!("Expected template type (Style, Element, Var), got {:?}", self.current_token));
                return None;
            }
        };
        self.next_token(); // consume type, current is now name

        let name = match &self.current_token {
            Token::Identifier(s) => s.clone(),
            _ => {
                self.errors.push(format!("Expected template name, got {:?}", self.current_token));
                return None;
            }
        };
        self.next_token(); // consume name, current is now {

        if !self.current_token_is(&Token::OpenBrace) {
            self.errors.push(format!("Expected '{{' to open template body, got {:?}", self.current_token));
            return None;
        }
        self.next_token(); // consume {

        let body = match kind {
            crate::chtl_compiler::node::TemplateKind::Style => self.parse_style_template_body(),
            crate::chtl_compiler::node::TemplateKind::Element => self.parse_element_template_body(),
            // For now, Var body parsing is not implemented
            _ => {
                self.errors.push("Parsing for this template type is not yet implemented.".to_string());
                return None;
            }
        };

        if !self.current_token_is(&Token::CloseBrace) {
            self.errors.push(format!("Expected '}}' to close template body, got {:?}", self.current_token));
            return None;
        }

        let def = TemplateDefinitionNode { name: name.clone(), kind, body };
        self.templates.insert(name, def.clone());

        Some(Node::TemplateDefinition(def))
    }

    fn parse_style_template_body(&mut self) -> crate::chtl_compiler::node::TemplateBody {
        let mut properties = Vec::new();
        // This is very similar to parse_style_block, but without creating a node.
        while !self.current_token_is(&Token::CloseBrace) && !self.current_token_is(&Token::EndOfFile) {
             if let Token::Identifier(name) = self.current_token.clone() {
                self.next_token(); // consume property name
                if !self.current_token_is(&Token::Colon) { break; } // simplified error handling
                self.next_token(); // consume ":"
                let value = match &self.current_token {
                    Token::Identifier(s) => s.clone(),
                    Token::StringLiteral(s) => s.clone(),
                    _ => break,
                };
                self.next_token(); // consume value
                if self.current_token_is(&Token::Semicolon) { self.next_token(); }
                properties.push(crate::chtl_compiler::node::StyleProperty { name, value: crate::chtl_compiler::node::PropertyValue::Literal(value) });
            } else {
                break; // Exit on unexpected token
            }
        }
        crate::chtl_compiler::node::TemplateBody::Style(properties)
    }

    fn parse_element_template_body(&mut self) -> crate::chtl_compiler::node::TemplateBody {
        let mut nodes = Vec::new();
        while !self.current_token_is(&Token::CloseBrace) && !self.current_token_is(&Token::EndOfFile) {
            if let Some(node) = self.parse_statement() {
                nodes.push(node);
            }
            self.next_token();
        }
        crate::chtl_compiler::node::TemplateBody::Element(nodes)
    }

    fn parse_template_usage(&mut self) -> Option<Node> {
        // current_token is @
        self.next_token(); // consume @, current is now type

        let kind = match self.current_token {
            Token::Identifier(ref s) if s == "Style" => crate::chtl_compiler::node::TemplateKind::Style,
            Token::Identifier(ref s) if s == "Element" => crate::chtl_compiler::node::TemplateKind::Element,
            Token::Identifier(ref s) if s == "Var" => crate::chtl_compiler::node::TemplateKind::Var,
            _ => {
                self.errors.push(format!("Expected template usage type (Style, Element, Var), got {:?}", self.current_token));
                return None;
            }
        };
        self.next_token(); // consume type, current is now name

        let name = match &self.current_token {
            Token::Identifier(s) => s.clone(),
            _ => {
                self.errors.push(format!("Expected template usage name, got {:?}", self.current_token));
                return None;
            }
        };

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Node::TemplateUsage(crate::chtl_compiler::node::TemplateUsageNode { name, kind }))
    }

    fn parse_import_statement(&mut self) -> Option<Node> {
        // current_token is [Import]
        self.next_token(); // consume [Import], current is now @

        if !self.current_token_is(&Token::At) {
            self.errors.push(format!("Expected '@' after [Import], got {:?}", self.current_token));
            return None;
        }
        self.next_token(); // consume @, current is now type

        let kind = match self.current_token {
            Token::Identifier(ref s) if s == "Chtl" => crate::chtl_compiler::node::ImportKind::Chtl,
            Token::Identifier(ref s) if s == "Html" => crate::chtl_compiler::node::ImportKind::Html,
            Token::Identifier(ref s) if s == "Style" => crate::chtl_compiler::node::ImportKind::Style,
            Token::Identifier(ref s) if s == "JavaScript" => crate::chtl_compiler::node::ImportKind::JavaScript,
            _ => {
                self.errors.push(format!("Expected import type, got {:?}", self.current_token));
                return None;
            }
        };
        self.next_token(); // consume type

        // Expect 'from' keyword
        if !self.current_token_is(&Token::Identifier("from".to_string())) {
            self.errors.push(format!("Expected 'from' keyword, got {:?}", self.current_token));
            return None;
        }
        self.next_token(); // consume 'from'

        // Expect path
        let path = match &self.current_token {
            Token::StringLiteral(s) => s.clone(),
            Token::Identifier(s) => s.clone(), // for unquoted paths
            _ => {
                self.errors.push(format!("Expected import path, got {:?}", self.current_token));
                return None;
            }
        };
        self.next_token(); // consume path

        // Expect optional 'as' keyword
        let alias = if self.current_token_is(&Token::Identifier("as".to_string())) {
            self.next_token(); // consume 'as'
            match &self.current_token {
                Token::Identifier(s) => Some(s.clone()),
                _ => {
                    self.errors.push(format!("Expected alias name, got {:?}", self.current_token));
                    None
                }
            }
        } else {
            None
        };

        // Perform the import
        if kind == crate::chtl_compiler::node::ImportKind::Chtl {
            match self.loader.load_file(&path) {
                Ok(content) => {
                    let mut new_parser = Parser::new(Lexer::new(&content));
                    new_parser.parse_program(); // This will parse and populate its own template map
                    // Merge templates
                    self.templates.extend(new_parser.templates);
                }
                Err(e) => {
                    self.errors.push(format!("Failed to load imported file '{}': {}", path, e));
                }
            }
        }

        Some(Node::Import(crate::chtl_compiler::node::ImportNode { kind, path, alias }))
    }

    fn current_token_is(&self, t: &Token) -> bool {
        // This is a bit tricky due to the String inside some tokens.
        // We use std::mem::discriminant for now.
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(&t) {
            self.next_token();
            true
        } else {
            self.peek_error(t);
            false
        }
    }

    fn peek_error(&mut self, t: Token) {
        self.errors.push(format!(
            "expected next token to be {:?}, got {:?} instead",
            t, self.peek_token
        ));
    }
}
