//! This module defines the Parser, which consumes tokens from the Lexer
//! to build an Abstract Syntax Tree (AST).

use crate::ast::{AttributeNode, CommentNode, ElementNode, Expression, Node, Program, TextNode};
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
        // Prime the parser with the first two tokens
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
        self.next_token(); // consume LBrace

        let mut attributes = Vec::new();
        let mut children = Vec::new();

        while self.cur_token != Token::RBrace && self.cur_token != Token::Eof {
            match self.cur_token {
                // Lookahead to see if it's an attribute or a child node
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

    fn parse_text_node(&mut self) -> Result<TextNode, String> {
        self.expect_peek(Token::LBrace)?;
        self.next_token(); // consume LBrace

        let value = match &self.cur_token {
            Token::String(s) => s.clone(),
            Token::Ident(s) => s.clone(), // for unquoted text
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

        self.next_token(); // move to : or =
        if !self.cur_token_is(&Token::Colon) && !self.cur_token_is(&Token::Assign) {
            return Err("Expected ':' or '=' after attribute name".to_string());
        }

        self.next_token(); // move to value
        let value = match &self.cur_token {
            Token::Ident(val) => Expression::Ident(val.clone()),
            Token::String(val) => Expression::StringLiteral(val.clone()),
            _ => return Err("Expected identifier or string for attribute value".to_string()),
        };

        self.expect_peek(Token::Semicolon)?;

        Ok(AttributeNode { name, value })
    }

    // Helper functions for checking tokens
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

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_simple_element_parsing() {
        let input = r#"
            div {
                id: "main";
                class: container;
                -- a generator comment
                span {
                    text { "Hello World" }
                }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert!(parser.errors.is_empty(), "Parser has errors: {:?}", parser.errors);
        assert_eq!(program.nodes.len(), 1);

        let node = program.nodes.get(0).unwrap();
        if let Node::Element(el) = node {
            assert_eq!(el.tag_name, "div");
            assert_eq!(el.attributes.len(), 2);
            assert_eq!(el.children.len(), 2);

            let attr1 = &el.attributes[0];
            assert_eq!(attr1.name, "id");
            assert_eq!(attr1.value, Expression::StringLiteral("main".to_string()));

            let child1 = &el.children[0];
            assert!(matches!(child1, Node::Comment(_)));

            let child2 = &el.children[1];
            if let Node::Element(inner_el) = child2 {
                assert_eq!(inner_el.tag_name, "span");
                assert_eq!(inner_el.children.len(), 1);
                if let Node::Text(text_node) = &inner_el.children[0] {
                    assert_eq!(text_node.value, "Hello World");
                } else {
                    panic!("Expected text node");
                }
            } else {
                panic!("Expected element node");
            }
        } else {
            panic!("Expected top-level node to be an element");
        }
    }
}
