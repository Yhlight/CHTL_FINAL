use crate::chtl_js::lexer::{Lexer, Token};
use crate::chtl_js::node::{Node, FunctionCallNode, Value};
use std::collections::HashMap;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    pub errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer, current_token: Token::Illegal, peek_token: Token::Illegal, errors: Vec::new(),
        };
        p.next_token(); p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Vec<Node> {
        let mut nodes = Vec::new();
        while self.current_token != Token::Eof {
            if let Some(node) = self.parse_node() { nodes.push(node); }
            self.next_token();
        }
        nodes
    }

    fn parse_node(&mut self) -> Option<Node> {
        match self.current_token {
            Token::Ident(_) => self.parse_function_call().map(Node::FunctionCall),
            _ => None,
        }
    }

    fn parse_function_call(&mut self) -> Option<FunctionCallNode> {
        let name = match &self.current_token { Token::Ident(s) => s.clone(), _ => return None };
        if self.peek_token != Token::LBrace { return None; }
        self.next_token();
        let arguments = self.parse_arguments_block()?;
        Some(FunctionCallNode { name, arguments })
    }

    fn parse_arguments_block(&mut self) -> Option<HashMap<String, Value>> {
        let mut args = HashMap::new();
        self.next_token();
        while self.current_token != Token::RBrace && self.current_token != Token::Eof {
            let key = match &self.current_token { Token::Ident(s) => s.clone(), _ => return None };
            if self.peek_token != Token::Colon { return None; }
            self.next_token(); self.next_token();
            if let Some(value) = self.parse_value() {
                 args.insert(key, value);
            }
            if self.peek_token == Token::Comma { self.next_token(); }
            self.next_token();
        }
        Some(args)
    }

    fn parse_value(&mut self) -> Option<Value> {
        match &self.current_token {
            Token::String(s) => Some(Value::String(s.clone())),
            Token::Number(n) => n.parse::<f64>().ok().map(Value::Number),
            Token::Ident(i) => Some(Value::Identifier(i.clone())),
            Token::LBrace => self.parse_object_value(),
            Token::LBracket => self.parse_array_value(),
            _ => None,
        }
    }

    fn parse_object_value(&mut self) -> Option<Value> {
        self.parse_arguments_block().map(Value::Object)
    }

    fn parse_array_value(&mut self) -> Option<Value> {
        let mut values = Vec::new();
        self.next_token();
        while self.current_token != Token::RBracket && self.current_token != Token::Eof {
            if let Some(value) = self.parse_value() {
                values.push(value);
            }
            if self.peek_token == Token::Comma { self.next_token(); }
            self.next_token();
        }
        Some(Value::Array(values))
    }
}
