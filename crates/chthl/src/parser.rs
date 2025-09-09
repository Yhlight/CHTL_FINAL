//! This module defines the Parser, which consumes tokens from the Lexer
//! to build an Abstract Syntax Tree (AST).

use crate::ast::{
    AttributeNode, CssRuleNode, ElementNode, Expression, InfixExpression, Node,
    Program, StyleNode, StyleProperty, TextNode, PrefixExpression
};
use crate::lexer::Lexer;
use crate::token::Token;
use std::mem;

#[derive(PartialEq, PartialOrd, Clone, Copy)]
enum Precedence {
    LOWEST,
    UNIT,       // 100px
    TERNARY,    // ? :
    LOGICAL,    // && ||
    LESSGREATER, // > or <
    SUM,        // + -
    PRODUCT,    // * /
    PREFIX,     // -X or !X
}

fn token_to_precedence(token: &Token) -> Precedence {
    match token {
        Token::LogicalAnd | Token::LogicalOr => Precedence::LOGICAL,
        Token::Gt | Token::Lt => Precedence::LESSGREATER,
        Token::Plus | Token::Minus => Precedence::SUM,
        Token::Slash | Token::Asterisk => Precedence::PRODUCT,
        Token::Question => Precedence::TERNARY,
        Token::Ident(_) => Precedence::UNIT,
        _ => Precedence::LOWEST,
    }
}

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
            if self.cur_token_is(&Token::Semicolon) {
                self.next_token();
            }
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
        }
        self.expect_cur(&Token::RBrace)?;
        Ok(CssRuleNode { selector, properties })
    }

    fn parse_style_property(&mut self) -> Result<StyleProperty, String> {
        let name = self.parse_identifier_as_string()?;
        self.expect_peek(&Token::Colon)?;
        self.next_token();
        let value = self.parse_expression(Precedence::LOWEST)?;
        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }
        Ok(StyleProperty { name, value })
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
        let value = self.parse_expression(Precedence::LOWEST)?;
        if self.peek_token_is(&Token::Semicolon) { self.next_token(); }
        Ok(AttributeNode { name, value })
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Result<Expression, String> {
        let mut left_exp = self.parse_prefix()?;
        while !self.peek_token_is(&Token::Semicolon) && precedence < self.peek_precedence() {
            self.next_token();
            left_exp = self.parse_infix(left_exp)?;
        }
        Ok(left_exp)
    }

    fn parse_prefix(&mut self) -> Result<Expression, String> {
        match &self.cur_token {
            Token::Ident(_) => self.parse_identifier(),
            Token::Number(_) => self.parse_number_literal(),
            Token::String(_) => self.parse_string_literal(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            _ => Err(format!("No prefix parse function for {:?}", self.cur_token)),
        }
    }

    fn parse_prefix_expression(&mut self) -> Result<Expression, String> {
        let operator = token_to_string(&self.cur_token);
        self.next_token();
        let right = self.parse_expression(Precedence::PREFIX)?;
        Ok(Expression::Prefix(PrefixExpression{ operator, right: Box::new(right) }))
    }

    fn parse_infix(&mut self, left: Expression) -> Result<Expression, String> {
        match self.cur_token {
            Token::Ident(_) => self.parse_unit_literal(left),
            _ => self.parse_infix_expression(left),
        }
    }

    fn parse_unit_literal(&mut self, number: Expression) -> Result<Expression, String> {
        let n = if let Expression::NumberLiteral(val) = number {
            val
        } else {
            return Err("Expected number before unit".to_string());
        };
        if let Token::Ident(unit) = &self.cur_token {
            Ok(Expression::UnitLiteral(n, unit.clone()))
        } else {
            Err("Expected unit identifier after number".to_string())
        }
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Result<Expression, String> {
        let operator = token_to_string(&self.cur_token);
        let precedence = self.cur_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;
        Ok(Expression::Infix(InfixExpression {
            left: Box::new(left), operator, right: Box::new(right)
        }))
    }

    fn parse_identifier(&self) -> Result<Expression, String> {
        if let Token::Ident(name) = &self.cur_token { Ok(Expression::Ident(name.clone())) }
        else { Err("Expected identifier".to_string()) }
    }

    fn parse_identifier_as_string(&self) -> Result<String, String> {
        if let Token::Ident(name) = &self.cur_token { Ok(name.clone()) }
        else { Err("Expected identifier".to_string()) }
    }

    fn parse_number_literal(&self) -> Result<Expression, String> {
        if let Token::Number(val) = &self.cur_token {
            val.parse::<f64>().map(Expression::NumberLiteral).map_err(|e| e.to_string())
        } else { Err("Expected number literal".to_string()) }
    }

    fn parse_string_literal(&self) -> Result<Expression, String> {
        if let Token::String(val) = &self.cur_token { Ok(Expression::StringLiteral(val.clone())) }
        else { Err("Expected string literal".to_string()) }
    }

    fn peek_precedence(&self) -> Precedence { token_to_precedence(&self.peek_token) }
    fn cur_precedence(&self) -> Precedence { token_to_precedence(&self.cur_token) }
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
        Token::Ident(s) | Token::String(s) | Token::Number(s) => s.clone(),
        Token::Dot => ".".to_string(), Token::Hash => "#".to_string(),
        Token::Ampersand => "&".to_string(), Token::Colon => ":".to_string(),
        Token::Plus => "+".to_string(), Token::Minus => "-".to_string(),
        Token::Asterisk => "*".to_string(), Token::Slash => "/".to_string(),
        Token::Gt => ">".to_string(), Token::Lt => "<".to_string(),
        Token::Bang => "!".to_string(),
        _ => "".to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_expression_parsing() {
        let tests = vec![
            ("5 + 10", "(5 + 10)"),
            ("10 * 2 + 5", "((10 * 2) + 5)"),
            ("5 + 10 * 2", "(5 + (10 * 2))"),
            ("100px", "100px"),
        ];

        for (input, expected) in tests {
            let input_str = format!("width: {};", input);
            let lexer = Lexer::new(&input_str);
            let mut parser = Parser::new(lexer);
            let prop = parser.parse_style_property().unwrap();
            assert!(parser.errors.is_empty(), "Parse errors for '{}': {:?}", input, parser.errors);
            assert_eq!(expression_to_string(prop.value), expected, "Failed on input: {}", input);
        }
    }

    fn expression_to_string(exp: Expression) -> String {
        match exp {
            Expression::Ident(s) => s,
            Expression::NumberLiteral(n) => n.to_string(),
            Expression::UnitLiteral(n, u) => format!("{}{}", n, u),
            Expression::StringLiteral(s) => s,
            Expression::Infix(infix) => {
                format!("({} {} {})", expression_to_string(*infix.left), infix.operator, expression_to_string(*infix.right))
            },
            Expression::Prefix(prefix) => {
                format!("({}{})", prefix.operator, expression_to_string(*prefix.right))
            },
            _ => "unimplemented".to_string()
        }
    }
}
