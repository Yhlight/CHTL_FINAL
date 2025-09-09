//! This module defines the Parser, which consumes tokens from the Lexer
//! to build an Abstract Syntax Tree (AST).

use crate::ast::{
    AttributeNode, CssRuleNode, ElementNode, Expression, Node, Program, StyleNode,
    StyleProperty, TextNode,
};
use crate::lexer::Lexer;
use crate::token::Token;
use std::mem;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    cur_token: Token,
    peek_token: Token,
    pub errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            cur_token: Token::Eof,
            peek_token: Token::Eof,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.cur_token = mem::replace(&mut self.peek_token, self.lexer.next_token());
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program { nodes: Vec::new() };
        while !self.cur_token_is(&Token::Eof) {
            match self.parse_node() {
                Ok(node) => program.nodes.push(node),
                Err(e) => self.errors.push(e),
            }
            self.next_token();
        }
        program
    }

    fn parse_node(&mut self) -> Result<Node, String> {
        match &self.cur_token {
            Token::Ident(tag) if tag == "text" => self.parse_text_node().map(Node::Text),
            Token::Ident(tag) if tag == "style" => self.parse_style_node().map(Node::Style),
            Token::Ident(_) => self.parse_element_node().map(Node::Element),
            _ => Err(format!("Cannot parse node from token {:?}", self.cur_token)),
        }
    }

    fn parse_element_node(&mut self) -> Result<ElementNode, String> {
        let tag_name = self.parse_identifier_as_string()?;
        self.expect_peek(&Token::LBrace)?;
        self.next_token();
        let mut attributes = vec![];
        let mut children = vec![];
        while !self.cur_token_is(&Token::RBrace) && !self.cur_token_is(&Token::Eof) {
            if let Token::Ident(_) = &self.cur_token {
                if self.peek_token_is(&Token::Colon) || self.peek_token_is(&Token::Assign) {
                    attributes.push(self.parse_attribute_node()?);
                } else {
                    children.push(self.parse_node()?);
                }
            } else {
                children.push(self.parse_node()?);
            }
            self.next_token();
        }
        self.expect_cur(&Token::RBrace)?;
        Ok(ElementNode { tag_name, attributes, children })
    }

    fn parse_style_node(&mut self) -> Result<StyleNode, String> {
        self.expect_peek(&Token::LBrace)?;
        self.next_token();
        let mut inline_properties = vec![];
        let mut rules = vec![];
        while !self.cur_token_is(&Token::RBrace) && !self.cur_token_is(&Token::Eof) {
            match &self.cur_token {
                Token::Dot | Token::Hash | Token::Ampersand => rules.push(self.parse_css_rule()?),
                Token::Ident(_) => {
                    if self.peek_token_is(&Token::Colon) {
                        inline_properties.push(self.parse_style_property()?);
                    } else {
                        rules.push(self.parse_css_rule()?);
                    }
                }
                _ => return Err(format!("Unexpected token in style block: {:?}", self.cur_token)),
            }
            self.next_token();
        }
        self.expect_cur(&Token::RBrace)?;
        Ok(StyleNode { inline_properties, rules })
    }

    fn parse_css_rule(&mut self) -> Result<CssRuleNode, String> {
        let mut selector_parts = Vec::new();
        while !self.cur_token_is(&Token::LBrace) && !self.cur_token_is(&Token::Eof) {
            selector_parts.push(token_to_string(&self.cur_token));
            self.next_token();
        }
        let selector = selector_parts.join("");
        self.expect_cur(&Token::LBrace)?;
        self.next_token();
        let mut properties = vec![];
        while !self.cur_token_is(&Token::RBrace) && !self.cur_token_is(&Token::Eof) {
            properties.push(self.parse_style_property()?);
            self.next_token();
        }
        self.expect_cur(&Token::RBrace)?;
        Ok(CssRuleNode { selector, properties })
    }

    fn parse_style_property(&mut self) -> Result<StyleProperty, String> {
        let name = self.parse_identifier_as_string()?;
        self.expect_peek(&Token::Colon)?;
        self.next_token();
        let mut value_parts = Vec::new();
        while !self.cur_token_is(&Token::Semicolon) && !self.cur_token_is(&Token::Eof) {
            value_parts.push(token_to_string(&self.cur_token));
            self.next_token();
        }
        if !self.cur_token_is(&Token::Semicolon) {
             return Err("Expected ';' after style property value".to_string());
        }
        Ok(StyleProperty { name, value: value_parts.join("") })
    }

    fn parse_text_node(&mut self) -> Result<TextNode, String> {
        self.expect_peek(&Token::LBrace)?;
        self.next_token();
        let value = match &self.cur_token {
            Token::String(s) | Token::Ident(s) => s.clone(),
            _ => return Err("Expected string or identifier for text node".to_string()),
        };
        self.next_token();
        self.expect_cur(&Token::RBrace)?;
        Ok(TextNode { value })
    }

    fn parse_attribute_node(&mut self) -> Result<AttributeNode, String> {
        let name = self.parse_identifier_as_string()?;
        self.expect_peek_oneof(&[Token::Colon, Token::Assign])?;
        self.next_token();
        let value = match &self.cur_token {
            Token::Ident(val) => Expression::Ident(val.clone()),
            Token::String(val) => Expression::StringLiteral(val.clone()),
            _ => return Err("Expected identifier or string for attribute value".to_string()),
        };
        if self.peek_token_is(&Token::Semicolon) { self.next_token(); }
        Ok(AttributeNode { name, value })
    }

    fn parse_identifier_as_string(&self) -> Result<String, String> {
        if let Token::Ident(name) = &self.cur_token { Ok(name.clone()) }
        else { Err("Expected identifier".to_string()) }
    }

    fn cur_token_is(&self, t: &Token) -> bool { mem::discriminant(&self.cur_token) == mem::discriminant(t) }
    fn peek_token_is(&self, t: &Token) -> bool { mem::discriminant(&self.peek_token) == mem::discriminant(t) }

    fn expect_peek(&mut self, t: &Token) -> Result<(), String> {
        if self.peek_token_is(t) { self.next_token(); Ok(()) }
        else { Err(format!("Expected next token to be {:?}, got {:?} instead", t, self.peek_token)) }
    }

    fn expect_peek_oneof(&mut self, tokens: &[Token]) -> Result<(), String> {
        for t in tokens { if self.peek_token_is(t) { self.next_token(); return Ok(()); } }
        Err(format!("Expected next token to be one of {:?}, got {:?} instead", tokens, self.peek_token))
    }

    fn expect_cur(&mut self, t: &Token) -> Result<(), String> {
        if self.cur_token_is(t) { Ok(()) }
        else { Err(format!("Expected current token to be {:?}, got {:?} instead", t, self.cur_token)) }
    }
}

fn token_to_string(token: &Token) -> String {
    match token {
        Token::Ident(s) | Token::String(s) => s.clone(),
        Token::Dot => ".".to_string(), Token::Hash => "#".to_string(),
        Token::Ampersand => "&".to_string(), Token::Colon => ":".to_string(),
        _ => "".to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_style_block_parsing() {
        let input = r#"
            style {
                color: white;
                .box {
                    width: 100px;
                }
                #main {
                    height: 200px;
                }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let node = parser.parse_node().unwrap();

        assert!(parser.errors.is_empty(), "Parser has errors: {:?}", parser.errors);

        if let Node::Style(style_node) = node {
            assert_eq!(style_node.inline_properties.len(), 1);
            assert_eq!(style_node.rules.len(), 2);
            let inline_prop = &style_node.inline_properties[0];
            assert_eq!(inline_prop.name, "color");
            assert_eq!(inline_prop.value, "white");
            let rule1 = &style_node.rules[0];
            assert_eq!(rule1.selector, ".box");
            assert_eq!(rule1.properties.len(), 1);
            let rule1_prop = &rule1.properties[0];
            assert_eq!(rule1_prop.name, "width");
            assert_eq!(rule1_prop.value, "100px");
            let rule2 = &style_node.rules[1];
            assert_eq!(rule2.selector, "#main");
        } else {
            panic!("Expected a StyleNode, got {:?}", node);
        }
    }
}
