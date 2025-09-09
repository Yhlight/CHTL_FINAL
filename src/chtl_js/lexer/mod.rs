//! CHTL JS Lexer
//! 
//! This module handles the lexical analysis of CHTL JS source code.

use anyhow::Result;
use std::collections::VecDeque;

/// CHTL JS Token types
#[derive(Debug, Clone, PartialEq)]
pub enum ChtlJSTokenType {
    // Basic tokens
    Identifier(String),
    String(String),
    Number(String),
    Boolean(bool),
    
    // CHTL JS specific tokens
    FileLoader,
    Script,
    Listen,
    Delegate,
    Animate,
    Router,
    Vir,
    
    // Operators
    Arrow,           // ->
    DoubleArrow,     // =>
    Dot,             // .
    Colon,           // :
    Semicolon,       // ;
    Comma,           // ,
    LeftParen,       // (
    RightParen,      // )
    LeftBrace,       // {
    RightBrace,      // }
    LeftBracket,     // [
    RightBracket,    // ]
    
    // Comparison operators
    Equal,           // ==
    NotEqual,        // !=
    Less,            // <
    Greater,         // >
    LessEqual,       // <=
    GreaterEqual,    // >=
    
    // Logical operators
    And,             // &&
    Or,              // ||
    Not,             // !
    
    // Assignment operators
    Assign,          // =
    PlusAssign,      // +=
    MinusAssign,     // -=
    MultiplyAssign,  // *=
    DivideAssign,    // /=
    
    // Arithmetic operators
    Plus,            // +
    Minus,           // -
    Multiply,        // *
    Divide,          // /
    Modulo,          // %
    
    // Special characters
    Question,        // ?
    At,              // @
    Hash,            // #
    Dollar,          // $
    Underscore,      // _
    
    // Whitespace and comments
    Newline,
    LineComment(String),
    BlockComment(String),
    
    // End of file
    Eof,
}

/// CHTL JS Token
#[derive(Debug, Clone)]
pub struct ChtlJSToken {
    pub token_type: ChtlJSTokenType,
    pub value: String,
    pub line: usize,
    pub column: usize,
}

/// CHTL JS Lexer
pub struct ChtlJSLexer {
    source: String,
    current: usize,
    line: usize,
    column: usize,
}

impl ChtlJSLexer {
    /// Create a new CHTL JS lexer
    pub fn new() -> Self {
        Self {
            source: String::new(),
            current: 0,
            line: 1,
            column: 1,
        }
    }
    
    /// Tokenize CHTL JS source code
    pub fn tokenize(&mut self, source: &str) -> Result<VecDeque<ChtlJSToken>> {
        self.source = source.to_string();
        self.current = 0;
        self.line = 1;
        self.column = 1;
        
        let mut tokens = VecDeque::new();
        
        while !self.is_at_end() {
            if let Some(token) = self.next_token()? {
                tokens.push_back(token);
            }
        }
        
        tokens.push_back(ChtlJSToken {
            token_type: ChtlJSTokenType::Eof,
            value: "".to_string(),
            line: self.line,
            column: self.column,
        });
        
        Ok(tokens)
    }
    
    fn next_token(&mut self) -> Result<Option<ChtlJSToken>> {
        self.skip_whitespace();
        
        if self.is_at_end() {
            return Ok(None);
        }
        
        let start = self.current;
        let line = self.line;
        let column = self.column;
        
        let ch = self.advance();
        
        match ch {
            // Single character tokens
            '(' => self.create_token(ChtlJSTokenType::LeftParen, start, line, column),
            ')' => self.create_token(ChtlJSTokenType::RightParen, start, line, column),
            '{' => self.create_token(ChtlJSTokenType::LeftBrace, start, line, column),
            '}' => self.create_token(ChtlJSTokenType::RightBrace, start, line, column),
            '[' => self.create_token(ChtlJSTokenType::LeftBracket, start, line, column),
            ']' => self.create_token(ChtlJSTokenType::RightBracket, start, line, column),
            ',' => self.create_token(ChtlJSTokenType::Comma, start, line, column),
            ';' => self.create_token(ChtlJSTokenType::Semicolon, start, line, column),
            ':' => self.create_token(ChtlJSTokenType::Colon, start, line, column),
            '.' => self.create_token(ChtlJSTokenType::Dot, start, line, column),
            '@' => self.create_token(ChtlJSTokenType::At, start, line, column),
            '#' => self.create_token(ChtlJSTokenType::Hash, start, line, column),
            '$' => self.create_token(ChtlJSTokenType::Dollar, start, line, column),
            '_' => self.create_token(ChtlJSTokenType::Underscore, start, line, column),
            '?' => self.create_token(ChtlJSTokenType::Question, start, line, column),
            
            // Multi-character tokens
            '=' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::Equal, start, line, column)
                } else if self.peek() == Some('>') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::DoubleArrow, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Assign, start, line, column)
                }
            }
            '!' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::NotEqual, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Not, start, line, column)
                }
            }
            '<' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::LessEqual, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Less, start, line, column)
                }
            }
            '>' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::GreaterEqual, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Greater, start, line, column)
                }
            }
            '&' => {
                if self.peek() == Some('&') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::And, start, line, column)
                } else {
                    return Err(anyhow::anyhow!("Unexpected character: {}", ch));
                }
            }
            '|' => {
                if self.peek() == Some('|') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::Or, start, line, column)
                } else {
                    return Err(anyhow::anyhow!("Unexpected character: {}", ch));
                }
            }
            '+' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::PlusAssign, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Plus, start, line, column)
                }
            }
            '-' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::MinusAssign, start, line, column)
                } else if self.peek() == Some('>') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::Arrow, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Minus, start, line, column)
                }
            }
            '*' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::MultiplyAssign, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Multiply, start, line, column)
                }
            }
            '/' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::DivideAssign, start, line, column)
                } else if self.peek() == Some('/') {
                    self.advance();
                    self.advance(); // Skip the second '/'
                    self.consume_line_comment()
                } else if self.peek() == Some('*') {
                    self.advance();
                    self.advance(); // Skip the '*'
                    self.consume_block_comment()
                } else {
                    self.create_token(ChtlJSTokenType::Divide, start, line, column)
                }
            }
            '%' => {
                if self.peek() == Some('=') {
                    self.advance();
                    self.create_token(ChtlJSTokenType::Modulo, start, line, column)
                } else {
                    self.create_token(ChtlJSTokenType::Modulo, start, line, column)
                }
            }
            
            // String literals
            '"' => self.consume_string('"'),
            '\'' => self.consume_string('\''),
            
            // Numbers
            '0'..='9' => self.consume_number(),
            
            // Identifiers and keywords
            'a'..='z' | 'A'..='Z' | '_' => self.consume_identifier(),
            
            // Newlines
            '\n' => {
                self.line += 1;
                self.column = 1;
                self.create_token(ChtlJSTokenType::Newline, start, line, column)
            }
            
            _ => Err(anyhow::anyhow!("Unexpected character: {}", ch)),
        }
    }
    
    fn create_token(&self, token_type: ChtlJSTokenType, start: usize, line: usize, column: usize) -> Result<Option<ChtlJSToken>> {
        let value = self.source[start..self.current].to_string();
        Ok(Some(ChtlJSToken {
            token_type,
            value,
            line,
            column,
        }))
    }
    
    fn consume_string(&mut self, quote: char) -> Result<Option<ChtlJSToken>> {
        let start = self.current - 1;
        let line = self.line;
        let column = self.column;
        
        while !self.is_at_end() && self.peek() != Some(quote) {
            if self.peek() == Some('\n') {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            self.advance();
        }
        
        if self.is_at_end() {
            return Err(anyhow::anyhow!("Unterminated string"));
        }
        
        self.advance(); // Consume the closing quote
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(ChtlJSToken {
            token_type: ChtlJSTokenType::String(value.clone()),
            value,
            line,
            column,
        }))
    }
    
    fn consume_number(&mut self) -> Result<Option<ChtlJSToken>> {
        let start = self.current - 1;
        let line = self.line;
        let column = self.column;
        
        while !self.is_at_end() && (self.peek().unwrap_or('\0').is_ascii_digit() || self.peek() == Some('.')) {
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(ChtlJSToken {
            token_type: ChtlJSTokenType::Number(value.clone()),
            value,
            line,
            column,
        }))
    }
    
    fn consume_identifier(&mut self) -> Result<Option<ChtlJSToken>> {
        let start = self.current - 1;
        let line = self.line;
        let column = self.column;
        
        while !self.is_at_end() && (self.peek().unwrap_or('\0').is_ascii_alphanumeric() || self.peek() == Some('_')) {
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        let token_type = match value.as_str() {
            "fileloader" => ChtlJSTokenType::FileLoader,
            "script" => ChtlJSTokenType::Script,
            "listen" => ChtlJSTokenType::Listen,
            "delegate" => ChtlJSTokenType::Delegate,
            "animate" => ChtlJSTokenType::Animate,
            "router" => ChtlJSTokenType::Router,
            "vir" => ChtlJSTokenType::Vir,
            "true" => ChtlJSTokenType::Boolean(true),
            "false" => ChtlJSTokenType::Boolean(false),
            _ => ChtlJSTokenType::Identifier(value.clone()),
        };
        
        Ok(Some(ChtlJSToken {
            token_type,
            value,
            line,
            column,
        }))
    }
    
    fn consume_line_comment(&mut self) -> Result<Option<ChtlJSToken>> {
        let start = self.current - 2; // Include the "//"
        let line = self.line;
        let column = self.column;
        
        while !self.is_at_end() && self.peek() != Some('\n') {
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(ChtlJSToken {
            token_type: ChtlJSTokenType::LineComment(value.clone()),
            value,
            line,
            column,
        }))
    }
    
    fn consume_block_comment(&mut self) -> Result<Option<ChtlJSToken>> {
        let start = self.current - 2; // Include the "/*"
        let line = self.line;
        let column = self.column;
        
        while !self.is_at_end() {
            if self.peek() == Some('*') && self.peek_next() == Some('/') {
                self.advance(); // Consume '*'
                self.advance(); // Consume '/'
                break;
            }
            if self.peek() == Some('\n') {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            self.advance();
        }
        
        let value = self.source[start..self.current].to_string();
        Ok(Some(ChtlJSToken {
            token_type: ChtlJSTokenType::BlockComment(value.clone()),
            value,
            line,
            column,
        }))
    }
    
    fn skip_whitespace(&mut self) {
        while !self.is_at_end() && self.peek().unwrap_or('\0').is_whitespace() && self.peek() != Some('\n') {
            self.advance();
        }
    }
    
    fn is_at_end(&self) -> bool {
        self.current >= self.source.len()
    }
    
    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.current)
    }
    
    fn peek_next(&self) -> Option<char> {
        self.source.chars().nth(self.current + 1)
    }
    
    fn advance(&mut self) -> char {
        if !self.is_at_end() {
            let ch = self.source.chars().nth(self.current).unwrap();
            self.current += 1;
            self.column += 1;
            ch
        } else {
            '\0'
        }
    }
}