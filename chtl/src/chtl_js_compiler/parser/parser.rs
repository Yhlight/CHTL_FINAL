use crate::chtl_js_compiler::lexer::{Lexer, Token};
use crate::chtl_js_compiler::node::{CHTLJSNode, EnhancedSelectorNode};

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            current_token: Token::Illegal("".to_string()),
            peek_token: Token::Illegal("".to_string()),
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Vec<CHTLJSNode> {
        let mut program = Vec::new();
        while self.current_token != Token::EndOfFile {
            if let Some(node) = self.parse_statement() {
                program.push(node);
            }
            self.next_token();
        }
        program
    }

    fn parse_statement(&mut self) -> Option<CHTLJSNode> {
        match self.current_token {
            Token::SelectorContent(_) => self.parse_enhanced_selector(),
            _ => None,
        }
    }

    fn parse_enhanced_selector(&mut self) -> Option<CHTLJSNode> {
        if let Token::SelectorContent(selector) = self.current_token.clone() {
            Some(CHTLJSNode::EnhancedSelector(EnhancedSelectorNode {
                selector,
            }))
        } else {
            // This should not be reached if called from parse_statement correctly
            None
        }
    }
}
