//! CHTL Lexer - Tokenizes CHTL source code
//! 
//! This module handles the lexical analysis of CHTL source code,
//! converting raw text into a stream of tokens.

use anyhow::Result;
use std::collections::VecDeque;

/// Token types in CHTL
#[derive(Debug, Clone, PartialEq)]
pub enum TokenType {
    // Literals
    String(String),
    Number(String),
    Identifier(String),
    
    // Keywords
    Text,
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
    Use,
    
    // Operators and punctuation
    LeftBrace,      // {
    RightBrace,     // }
    LeftBracket,    // [
    RightBracket,   // ]
    LeftParen,      // (
    RightParen,     // )
    Semicolon,      // ;
    Colon,          // :
    Equals,         // =
    Comma,          // ,
    Dot,            // .
    Hash,           // #
    At,             // @
    Question,       // ?
    Exclamation,    // !
    Ampersand,      // &
    Pipe,           // |
    Plus,           // +
    Minus,          // -
    Star,           // *
    Slash,          // /
    Percent,        // %
    Less,           // <
    Greater,        // >
    LessEqual,      // <=
    GreaterEqual,   // >=
    EqualEqual,     // ==
    NotEqual,       // !=
    And,            // &&
    Or,             // ||
    
    // Comments
    LineComment,    // //
    BlockComment,   // /* */
    GeneratorComment, // --
    
    // Special
    Newline,
    Whitespace,
    EOF,
}

/// A token in the CHTL language
#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub position: Position,
    pub value: String,
}

/// Position information for a token
#[derive(Debug, Clone, PartialEq)]
pub struct Position {
    pub line: usize,
    pub column: usize,
    pub offset: usize,
}

/// CHTL Lexer
pub struct ChtlLexer {
    source: String,
    position: usize,
    line: usize,
    column: usize,
}

impl ChtlLexer {
    /// Create a new CHTL lexer
    pub fn new() -> Self {
        Self {
            source: String::new(),
            position: 0,
            line: 1,
            column: 1,
        }
    }
    
    /// Tokenize the given source code
    pub fn tokenize(&self, source: &str) -> Result<VecDeque<Token>> {
        let mut lexer = Self {
            source: source.to_string(),
            position: 0,
            line: 1,
            column: 1,
        };
        
        let mut tokens = VecDeque::new();
        
        while !lexer.is_at_end() {
            lexer.skip_whitespace();
            
            if lexer.is_at_end() {
                break;
            }
            
            if let Some(token) = lexer.next_token()? {
                tokens.push_back(token);
            }
        }
        
        // Add EOF token
        tokens.push_back(Token {
            token_type: TokenType::EOF,
            position: Position {
                line: lexer.line,
                column: lexer.column,
                offset: lexer.position,
            },
            value: String::new(),
        });
        
        Ok(tokens)
    }
    
    fn is_at_end(&self) -> bool {
        self.position >= self.source.len()
    }
    
    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.position)
    }
    
    fn peek_next(&self) -> Option<char> {
        self.source.chars().nth(self.position + 1)
    }
    
    fn advance(&mut self) -> Option<char> {
        let ch = self.peek()?;
        self.position += 1;
        if ch == '\n' {
            self.line += 1;
            self.column = 1;
        } else {
            self.column += 1;
        }
        Some(ch)
    }
    
    fn skip_whitespace(&mut self) {
        while let Some(ch) = self.peek() {
            if ch.is_whitespace() && ch != '\n' {
                self.advance();
            } else {
                break;
            }
        }
    }
    
    fn next_token(&mut self) -> Result<Option<Token>> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_column = self.column;
        
        let ch = match self.peek() {
            Some(c) => c,
            None => return Ok(None),
        };
        
        let token = match ch {
            // Single character tokens
            '{' => {
                self.advance();
                self.create_token(TokenType::LeftBrace, start_pos, start_line, start_column)
            }
            '}' => {
                self.advance();
                self.create_token(TokenType::RightBrace, start_pos, start_line, start_column)
            }
            '[' => {
                self.advance();
                self.create_token(TokenType::LeftBracket, start_pos, start_line, start_column)
            }
            ']' => {
                self.advance();
                self.create_token(TokenType::RightBracket, start_pos, start_line, start_column)
            }
            '(' => {
                self.advance();
                self.create_token(TokenType::LeftParen, start_pos, start_line, start_column)
            }
            ')' => {
                self.advance();
                self.create_token(TokenType::RightParen, start_pos, start_line, start_column)
            }
            ';' => {
                self.advance();
                self.create_token(TokenType::Semicolon, start_pos, start_line, start_column)
            }
            ':' => {
                self.advance();
                self.create_token(TokenType::Colon, start_pos, start_line, start_column)
            }
            '=' => {
                self.advance();
                self.create_token(TokenType::Equals, start_pos, start_line, start_column)
            }
            ',' => {
                self.advance();
                self.create_token(TokenType::Comma, start_pos, start_line, start_column)
            }
            '.' => {
                self.advance();
                self.create_token(TokenType::Dot, start_pos, start_line, start_column)
            }
            '#' => {
                self.advance();
                self.create_token(TokenType::Hash, start_pos, start_line, start_column)
            }
            '@' => {
                self.advance();
                self.create_token(TokenType::At, start_pos, start_line, start_column)
            }
            '?' => {
                self.advance();
                self.create_token(TokenType::Question, start_pos, start_line, start_column)
            }
            '!' => {
                self.advance();
                self.create_token(TokenType::Exclamation, start_pos, start_line, start_column)
            }
            '&' => {
                if self.peek_next() == Some('&') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::And, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Ampersand, start_pos, start_line, start_column)
                }
            }
            '|' => {
                if self.peek_next() == Some('|') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::Or, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Pipe, start_pos, start_line, start_column)
                }
            }
            '+' => {
                self.advance();
                self.create_token(TokenType::Plus, start_pos, start_line, start_column)
            }
            '-' => {
                if self.peek_next() == Some('-') {
                    self.advance(); // consume first '-'
                    self.advance(); // consume second '-'
                    self.create_token(TokenType::GeneratorComment, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Minus, start_pos, start_line, start_column)
                }
            }
            '*' => {
                self.advance();
                self.create_token(TokenType::Star, start_pos, start_line, start_column)
            }
            '/' => {
                if self.peek_next() == Some('/') {
                    // Line comment
                    while let Some(ch) = self.peek() {
                        if ch == '\n' {
                            break;
                        }
                        self.advance();
                    }
                    self.create_token(TokenType::LineComment, start_pos, start_line, start_column)
                } else if self.peek_next() == Some('*') {
                    // Block comment
                    self.advance(); // consume first '/'
                    self.advance(); // consume '*'
                    while let Some(ch) = self.peek() {
                        if ch == '*' && self.peek_next() == Some('/') {
                            self.advance(); // consume '*'
                            self.advance(); // consume '/'
                            break;
                        }
                        self.advance();
                    }
                    self.create_token(TokenType::BlockComment, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Slash, start_pos, start_line, start_column)
                }
            }
            '%' => {
                self.advance();
                self.create_token(TokenType::Percent, start_pos, start_line, start_column)
            }
            '<' => {
                if self.peek_next() == Some('=') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::LessEqual, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Less, start_pos, start_line, start_column)
                }
            }
            '>' => {
                if self.peek_next() == Some('=') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::GreaterEqual, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Greater, start_pos, start_line, start_column)
                }
            }
            '=' => {
                if self.peek_next() == Some('=') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::EqualEqual, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Equals, start_pos, start_line, start_column)
                }
            }
            '!' => {
                if self.peek_next() == Some('=') {
                    self.advance();
                    self.advance();
                    self.create_token(TokenType::NotEqual, start_pos, start_line, start_column)
                } else {
                    self.advance();
                    self.create_token(TokenType::Exclamation, start_pos, start_line, start_column)
                }
            }
            '"' => {
                self.advance(); // consume opening quote
                let mut value = String::new();
                while let Some(ch) = self.peek() {
                    if ch == '"' {
                        self.advance(); // consume closing quote
                        break;
                    }
                    if ch == '\\' {
                        self.advance(); // consume backslash
                        if let Some(escaped) = self.peek() {
                            value.push(match escaped {
                                'n' => '\n',
                                't' => '\t',
                                'r' => '\r',
                                '\\' => '\\',
                                '"' => '"',
                                _ => escaped,
                            });
                            self.advance();
                        }
                    } else {
                        value.push(ch);
                        self.advance();
                    }
                }
                self.create_token(TokenType::String(value), start_pos, start_line, start_column)
            }
            '\'' => {
                self.advance(); // consume opening quote
                let mut value = String::new();
                while let Some(ch) = self.peek() {
                    if ch == '\'' {
                        self.advance(); // consume closing quote
                        break;
                    }
                    if ch == '\\' {
                        self.advance(); // consume backslash
                        if let Some(escaped) = self.peek() {
                            value.push(match escaped {
                                'n' => '\n',
                                't' => '\t',
                                'r' => '\r',
                                '\\' => '\\',
                                '\'' => '\'',
                                _ => escaped,
                            });
                            self.advance();
                        }
                    } else {
                        value.push(ch);
                        self.advance();
                    }
                }
                self.create_token(TokenType::String(value), start_pos, start_line, start_column)
            }
            '\n' => {
                self.advance();
                self.create_token(TokenType::Newline, start_pos, start_line, start_column)
            }
            _ if ch.is_alphabetic() || ch == '_' => {
                // Identifier or keyword
                let mut value = String::new();
                while let Some(ch) = self.peek() {
                    if ch.is_alphanumeric() || ch == '_' || ch == '-' {
                        value.push(ch);
                        self.advance();
                    } else {
                        break;
                    }
                }
                
                let token_type = match value.as_str() {
                    "text" => TokenType::Text,
                    "style" => TokenType::Style,
                    "script" => TokenType::Script,
                    "Template" => TokenType::Template,
                    "Custom" => TokenType::Custom,
                    "Origin" => TokenType::Origin,
                    "Import" => TokenType::Import,
                    "Namespace" => TokenType::Namespace,
                    "Configuration" => TokenType::Configuration,
                    "use" => TokenType::Use,
                    _ => TokenType::Identifier(value),
                };
                
                self.create_token(token_type, start_pos, start_line, start_column)
            }
            _ if ch.is_numeric() => {
                // Number
                let mut value = String::new();
                while let Some(ch) = self.peek() {
                    if ch.is_numeric() || ch == '.' {
                        value.push(ch);
                        self.advance();
                    } else {
                        break;
                    }
                }
                self.create_token(TokenType::Number(value), start_pos, start_line, start_column)
            }
            _ => {
                // Unknown character, skip it
                self.advance();
                return Ok(None);
            }
        };
        
        Ok(Some(token))
    }
    
    fn create_token(&self, token_type: TokenType, start_pos: usize, start_line: usize, start_column: usize) -> Token {
        let value = match &token_type {
            TokenType::String(s) => s.clone(),
            TokenType::Number(n) => n.clone(),
            TokenType::Identifier(i) => i.clone(),
            _ => String::new(),
        };
        
        Token {
            token_type,
            position: Position {
                line: start_line,
                column: start_column,
                offset: start_pos,
            },
            value,
        }
    }
}