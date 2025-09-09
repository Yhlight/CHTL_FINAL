use crate::chtl::lexer::{Lexer, Token};
use crate::chtl::node::{Program, Node, ElementNode, TextNode, OriginNode};
use std::collections::HashMap;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    pub errors: Vec<String>,
    global_styles: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer, current_token: Token::Illegal, peek_token: Token::Illegal,
            errors: Vec::new(), global_styles: Vec::new(),
        };
        p.next_token(); p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if self.peek_token == token {
            self.next_token(); true
        } else {
            self.peek_error(&token); false
        }
    }

    fn peek_error(&mut self, token: &Token) {
        self.errors.push(format!("expected next token to be {:?}, got {:?} instead", token, self.peek_token));
    }

    pub fn parse_program(&mut self) -> Program {
        let mut children = Vec::new();
        while self.current_token != Token::Eof {
            if let Some(node) = self.parse_node() {
                children.push(node);
            }
            self.next_token();
        }
        Program { children, global_styles: self.global_styles.clone() }
    }

    fn parse_node(&mut self) -> Option<Node> {
        match &self.current_token {
            Token::LBracket => self.parse_special_block(),
            Token::Ident(name) => {
                match name.as_str() {
                    "text" => self.parse_text_node().map(Node::Text),
                    _ => self.parse_element_node(),
                }
            }
            _ => None,
        }
    }

    fn parse_special_block(&mut self) -> Option<Node> {
        self.next_token(); // Consume '['
        let name = match &self.current_token { Token::Ident(s) => s.clone(), _ => return None };
        if !self.expect_peek(Token::RBracket) { return None; }
        match name.as_str() {
            "Origin" => self.parse_origin_node(),
            _ => { self.errors.push(format!("unknown special block type: {}", name)); None }
        }
    }

    fn parse_origin_node(&mut self) -> Option<Node> {
        if !self.expect_peek(Token::At) { return None; }
        self.next_token();
        let origin_type = match &self.current_token { Token::Ident(s) => s.clone(), _ => return None };
        if !self.expect_peek(Token::LBrace) { return None; }
        let content = self.read_raw_content_until_matching_brace();
        Some(Node::Origin(OriginNode { origin_type, content }))
    }

    fn read_raw_content_until_matching_brace(&mut self) -> String {
        let start_pos = self.lexer.get_position() + 1;
        let mut brace_level = 1;
        while brace_level > 0 && self.peek_token != Token::Eof {
            self.next_token();
            match self.current_token {
                Token::LBrace => brace_level += 1,
                Token::RBrace => brace_level -= 1,
                _ => {}
            }
        }
        let end_pos = self.lexer.get_position();
        let content = self.lexer.get_input_slice(start_pos, end_pos).unwrap_or("").to_string();
        self.next_token();
        content
    }

    fn parse_element_node(&mut self) -> Option<Node> {
        let tag_name = match &self.current_token { Token::Ident(name) => name.clone(), _ => return None };
        if !self.expect_peek(Token::LBrace) { return None; }
        self.next_token();

        let mut attributes = HashMap::new();
        let mut children = Vec::new();

        while self.current_token != Token::RBrace && self.current_token != Token::Eof {
            match &self.current_token {
                Token::Ident(name) => {
                    if self.peek_token == Token::Colon {
                        self.parse_attribute(&mut attributes);
                    } else if self.peek_token == Token::LBrace {
                         match name.as_str() {
                            "style" => self.parse_style_block(&mut attributes),
                            _ => { // This must be a child element or a text block
                                if let Some(node) = self.parse_node() {
                                    children.push(node);
                                }
                            }
                        }
                    }
                },
                Token::LBracket => {
                     if let Some(node) = self.parse_special_block() {
                        children.push(node);
                     }
                }
                _ => { /* Ignore other tokens for now */ }
            }
            self.next_token();
        }

        Some(Node::Element(ElementNode { tag_name, attributes, children }))
    }

    fn parse_attribute(&mut self, attributes: &mut HashMap<String, String>) {
        let key = match &self.current_token { Token::Ident(s) => s.clone(), _ => return };
        self.next_token(); self.next_token();
        let value = match &self.current_token {
            Token::String(s) | Token::Ident(s) => s.clone(),
            _ => return,
        };
        if self.peek_token == Token::Semicolon { self.next_token(); }
        attributes.insert(key, value);
    }

    fn parse_style_block(&mut self, attributes: &mut HashMap<String, String>) {
        self.next_token(); self.next_token();
        let mut inline_styles = String::new();
        while self.current_token != Token::RBrace {
            if let Token::Ident(key) = &self.current_token {
                if self.peek_token == Token::Colon {
                    let prop_key = key.clone();
                    self.next_token(); self.next_token();
                    if let Token::Ident(val) = &self.current_token {
                        inline_styles.push_str(&format!("{}: {}; ", prop_key, val));
                    }
                }
            }
            self.next_token();
        }
        attributes.entry("style".to_string()).and_modify(|e| e.push_str(&inline_styles)).or_insert(inline_styles);
    }

    // CORRECTED a bug here. Should return Option<TextNode>
    fn parse_text_node(&mut self) -> Option<TextNode> {
        self.next_token();
        if !self.expect_peek(Token::LBrace) { return None; }
        self.next_token();
        let value = match &self.current_token {
            Token::String(s) | Token::Ident(s) => s.clone(),
            _ => return None
        };
        if !self.expect_peek(Token::RBrace) { return None; }
        Some(TextNode { value })
    }
}

#[cfg(test)]
mod tests {
    // ... tests ...
}
