//! This module defines the Parser, which consumes tokens from the Lexer
//! to build an Abstract Syntax Tree (AST).

use crate::ast::{
    AttributeNode, CommentNode, ElementNode, Expression, Node, Program, StyleNode, StyleProperty,
    TextNode,
};
use crate::lexer::Lexer;
use crate::token::Token;
use std::mem;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    cur_token: Token,
    peek_token: Token,
    errors: Vec<String>,
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

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn next_token(&mut self) {
        self.cur_token = mem::replace(&mut self.peek_token, self.lexer.next_token());
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program { nodes: Vec::new() };
        while self.cur_token != Token::Eof {
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
            Token::GeneratorComment(value) => Ok(Node::Comment(CommentNode {
                value: value.clone(),
            })),
            _ => Err(format!(
                "Unexpected token found for start of a node: {:?}",
                self.cur_token
            )),
        }
    }

    fn parse_element_node(&mut self) -> Result<ElementNode, String> {
        let tag_name = match &self.cur_token {
            Token::Ident(name) => name.clone(),
            _ => return Err("Expected identifier for tag name".to_string()),
        };

        self.expect_peek(Token::LBrace)?;
        self.next_token();

        let mut attributes = Vec::new();
        let mut children = Vec::new();

        while self.cur_token != Token::RBrace && self.cur_token != Token::Eof {
            match self.cur_token {
                Token::Ident(_) => {
                    if self.peek_token_is(&Token::Colon) || self.peek_token_is(&Token::Assign) {
                        attributes.push(self.parse_attribute_node()?);
                    } else {
                        children.push(self.parse_node()?);
                    }
                }
                Token::GeneratorComment(_) => {
                    children.push(self.parse_node()?);
                }
                _ => {
                    return Err(format!(
                        "Unexpected token inside element block: {:?}",
                        self.cur_token
                    ));
                }
            }
            self.next_token();
        }

        if self.cur_token != Token::RBrace {
            return Err("Expected '}' to close element block".to_string());
        }

        Ok(ElementNode {
            tag_name,
            attributes,
            children,
        })
    }

    fn parse_style_node(&mut self) -> Result<StyleNode, String> {
        self.expect_peek(Token::LBrace)?;
        self.next_token();

        let mut properties = Vec::new();

        while self.cur_token != Token::RBrace && self.cur_token != Token::Eof {
            properties.push(self.parse_style_property()?);
            self.next_token();
        }

        if self.cur_token != Token::RBrace {
            return Err("Expected '}' to close style block".to_string());
        }

        Ok(StyleNode { properties })
    }

    fn parse_style_property(&mut self) -> Result<StyleProperty, String> {
        let name = match &self.cur_token {
            Token::Ident(name) => name.clone(),
            _ => return Err("Expected identifier for style property name".to_string()),
        };

        self.expect_peek(Token::Colon)?;

        self.next_token();

        let mut value_parts = Vec::new();
        while !self.cur_token_is(&Token::Semicolon) && !self.cur_token_is(&Token::Eof) {
            value_parts.push(token_to_string(&self.cur_token));
            self.next_token();
        }

        if !self.cur_token_is(&Token::Semicolon) {
             return Err("Expected ';' after style property value".to_string());
        }

        Ok(StyleProperty {
            name,
            value: value_parts.join(" "),
        })
    }

    fn parse_text_node(&mut self) -> Result<TextNode, String> {
        self.expect_peek(Token::LBrace)?;
        self.next_token();

        let value = match &self.cur_token {
            Token::String(s) => s.clone(),
            Token::Ident(s) => s.clone(),
            _ => return Err("Expected string or identifier for text node content".to_string()),
        };
        self.next_token();

        if self.cur_token != Token::RBrace {
            return Err("Expected '}' to close text block".to_string());
        }

        Ok(TextNode { value })
    }

    fn parse_attribute_node(&mut self) -> Result<AttributeNode, String> {
        let name = match &self.cur_token {
            Token::Ident(name) => name.clone(),
            _ => return Err("Expected identifier for attribute name".to_string()),
        };

        self.next_token();
        if !self.cur_token_is(&Token::Colon) && !self.cur_token_is(&Token::Assign) {
            return Err("Expected ':' or '=' after attribute name".to_string());
        }

        self.next_token();
        let value = match &self.cur_token {
            Token::Ident(val) => Expression::Ident(val.clone()),
            Token::String(val) => Expression::StringLiteral(val.clone()),
            _ => return Err("Expected identifier or string for attribute value".to_string()),
        };

        self.expect_peek(Token::Semicolon)?;

        Ok(AttributeNode { name, value })
    }

    fn cur_token_is(&self, t: &Token) -> bool {
        mem::discriminant(&self.cur_token) == mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        mem::discriminant(&self.peek_token) == mem::discriminant(t)
    }

    fn expect_peek(&mut self, t: Token) -> Result<(), String> {
        if self.peek_token_is(&t) {
            self.next_token();
            Ok(())
        } else {
            Err(format!(
                "Expected next token to be {:?}, got {:?} instead",
                t, self.peek_token
            ))
        }
    }
}

fn token_to_string(token: &Token) -> String {
    match token {
        Token::Ident(s) => s.clone(),
        Token::String(s) => s.clone(),
        Token::Dot => ".".to_string(),
        _ => "".to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_element_with_styles_parsing() {
        let input = r#"
            div {
                id: "main";
                -- a generator comment
                style {
                    width: 100px;
                    height: 200px;
                }
                span {
                    text { "Hello World" }
                }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(
            parser.errors.is_empty(),
            "Parser has errors: {:?}",
            parser.errors
        );
        assert_eq!(program.nodes.len(), 1);

        let node = program.nodes.get(0).unwrap();
        if let Node::Element(el) = node {
            assert_eq!(el.tag_name, "div");
            assert_eq!(el.attributes.len(), 1);
            assert_eq!(el.children.len(), 3);

            let attr1 = &el.attributes[0];
            assert_eq!(attr1.name, "id");
            assert_eq!(attr1.value, Expression::StringLiteral("main".to_string()));

            let child1 = &el.children[0];
            assert!(matches!(child1, Node::Comment(_)));

            let child2 = &el.children[1];
            if let Node::Style(style_node) = child2 {
                assert_eq!(style_node.properties.len(), 2);
                let prop1 = &style_node.properties[0];
                assert_eq!(prop1.name, "width");
                assert_eq!(prop1.value, "100px");
            } else {
                panic!("Expected style node");
            }

            let child3 = &el.children[2];
            assert!(matches!(child3, Node::Element(_)));
        } else {
            panic!("Expected top-level node to be an element");
        }
    }
}
