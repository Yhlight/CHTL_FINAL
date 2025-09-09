//! CHTL lexer (tokenizer)
//! 
//! This module provides lexical analysis for CHTL source code,
//! converting raw text into a stream of tokens for parsing.

use std::collections::HashMap;

/// CHTL lexer for tokenizing source code
#[derive(Debug, Clone)]
pub struct CHTLLexer {
    /// Source code being analyzed
    source: String,
    /// Current position in the source
    position: usize,
    /// Current line number
    line: usize,
    /// Current column position
    column: usize,
    /// Keywords
    keywords: HashMap<String, TokenType>,
}

/// Token type enumeration
#[derive(Debug, Clone, PartialEq)]
pub enum TokenType {
    // Comments
    SingleLineComment(String),
    MultiLineComment(String),
    GeneratorComment(String),
    
    // Literals
    String(String),
    Number(String),
    Identifier(String),
    
    // Keywords and operators
    LeftBracket,
    RightBracket,
    LeftBrace,
    RightBrace,
    LeftParen,
    RightParen,
    Colon,
    Semicolon,
    Comma,
    Dot,
    At,
    Ampersand,
    DoubleAmpersand,
    Pipe,
    DoublePipe,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    Question,
    
    // Template and custom keywords
    Template,
    Custom,
    Import,
    Export,
    Namespace,
    Configuration,
    Origin,
    
    // Special tokens
    Whitespace,
    Newline,
    EOF,
}

/// Token structure
#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub value: String,
    pub location: SourceLocation,
}

/// Source location information
#[derive(Debug, Clone)]
pub struct SourceLocation {
    pub file: String,
    pub line: usize,
    pub column: usize,
    pub position: usize,
}

/// Lexer error
#[derive(Debug, thiserror::Error)]
pub enum LexerError {
    #[error("Unexpected character '{0}' at line {1}, column {2}")]
    UnexpectedCharacter(char, usize, usize),
    
    #[error("Unterminated string at line {0}, column {1}")]
    UnterminatedString(usize, usize),
    
    #[error("Unterminated comment at line {0}, column {1}")]
    UnterminatedComment(usize, usize),
    
    #[error("Invalid number format at line {0}, column {1}")]
    InvalidNumber(usize, usize),
}

impl CHTLLexer {
    /// Create a new lexer with the given source code
    pub fn new(source: String) -> Self {
        let mut keywords = HashMap::new();
        keywords.insert("Template".to_string(), TokenType::Template);
        keywords.insert("Custom".to_string(), TokenType::Custom);
        keywords.insert("Import".to_string(), TokenType::Import);
        keywords.insert("Export".to_string(), TokenType::Export);
        keywords.insert("Namespace".to_string(), TokenType::Namespace);
        keywords.insert("Configuration".to_string(), TokenType::Configuration);
        keywords.insert("Origin".to_string(), TokenType::Origin);
        
        Self {
            source,
            position: 0,
            line: 1,
            column: 1,
            keywords,
        }
    }
    
    /// Tokenize the source code
    pub fn tokenize(&mut self) -> Result<Vec<Token>, LexerError> {
        let mut tokens = Vec::new();
        
        while let Some(token) = self.next_token()? {
            if !matches!(token.token_type, TokenType::Whitespace) {
                tokens.push(token);
            }
        }
        
        Ok(tokens)
    }
    
    /// Get the next token
    pub fn next_token(&mut self) -> Result<Option<Token>, LexerError> {
        self.skip_whitespace();
        
        if self.position >= self.source.len() {
            return Ok(Some(self.create_token(TokenType::EOF, "", self.position, self.line, self.column)));
        }
        
        let start_pos = self.position;
        let start_line = self.line;
        let start_column = self.column;
        
        let current_char = self.current_char().unwrap();
        
        match current_char {
            // Comments
            '/' if self.peek_char() == Some('/') => {
                self.advance(); // Skip first /
                self.advance(); // Skip second /
                let content = self.read_until_newline();
                Ok(Some(self.create_token(TokenType::SingleLineComment(content.clone()), &format!("//{}", content), start_pos, start_line, start_column)))
            }
            '/' if self.peek_char() == Some('*') => {
                self.advance(); // Skip /
                self.advance(); // Skip *
                let content = self.read_until_comment_end()?;
                Ok(Some(self.create_token(TokenType::MultiLineComment(content.clone()), &format!("/*{}*/", content), start_pos, start_line, start_column)))
            }
            '-' if self.peek_char() == Some('-') => {
                self.advance(); // Skip first -
                self.advance(); // Skip second -
                let content = self.read_until_newline();
                Ok(Some(self.create_token(TokenType::GeneratorComment(content.clone()), &format!("--{}", content), start_pos, start_line, start_column)))
            }
            
            // String literals
            '"' | '\'' => {
                let quote = current_char;
                self.advance(); // Skip opening quote
                let content = self.read_string(quote)?;
                Ok(Some(self.create_token(TokenType::String(content.clone()), &format!("{}{}{}", quote, content, quote), start_pos, start_line, start_column)))
            }
            
            // Numbers
            c if c.is_ascii_digit() => {
                let number = self.read_number();
                Ok(Some(self.create_token(TokenType::Number(number.clone()), &number, start_pos, start_line, start_column)))
            }
            
            // Identifiers and keywords
            c if c.is_alphabetic() || c == '_' => {
                let identifier = self.read_identifier();
                let token_type = if let Some(keyword_type) = self.keywords.get(&identifier) {
                    keyword_type.clone()
                } else {
                    TokenType::Identifier(identifier.clone())
                };
                Ok(Some(self.create_token(token_type, &identifier, start_pos, start_line, start_column)))
            }
            
            // Single character tokens
            '[' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::LeftBracket, "[", start_pos, start_line, start_column)))
            }
            ']' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::RightBracket, "]", start_pos, start_line, start_column)))
            }
            '{' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::LeftBrace, "{", start_pos, start_line, start_column)))
            }
            '}' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::RightBrace, "}", start_pos, start_line, start_column)))
            }
            '(' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::LeftParen, "(", start_pos, start_line, start_column)))
            }
            ')' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::RightParen, ")", start_pos, start_line, start_column)))
            }
            ':' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Colon, ":", start_pos, start_line, start_column)))
            }
            ';' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Semicolon, ";", start_pos, start_line, start_column)))
            }
            ',' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Comma, ",", start_pos, start_line, start_column)))
            }
            '.' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Dot, ".", start_pos, start_line, start_column)))
            }
            '@' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::At, "@", start_pos, start_line, start_column)))
            }
            '&' if self.peek_char() == Some('&') => {
                self.advance();
                self.advance();
                Ok(Some(self.create_token(TokenType::DoubleAmpersand, "&&", start_pos, start_line, start_column)))
            }
            '&' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Ampersand, "&", start_pos, start_line, start_column)))
            }
            '|' if self.peek_char() == Some('|') => {
                self.advance();
                self.advance();
                Ok(Some(self.create_token(TokenType::DoublePipe, "||", start_pos, start_line, start_column)))
            }
            '|' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Pipe, "|", start_pos, start_line, start_column)))
            }
            '=' if self.peek_char() == Some('=') => {
                self.advance();
                self.advance();
                Ok(Some(self.create_token(TokenType::Equal, "==", start_pos, start_line, start_column)))
            }
            '!' if self.peek_char() == Some('=') => {
                self.advance();
                self.advance();
                Ok(Some(self.create_token(TokenType::NotEqual, "!=", start_pos, start_line, start_column)))
            }
            '<' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::LessThan, "<", start_pos, start_line, start_column)))
            }
            '>' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::GreaterThan, ">", start_pos, start_line, start_column)))
            }
            '?' => {
                self.advance();
                Ok(Some(self.create_token(TokenType::Question, "?", start_pos, start_line, start_column)))
            }
            
            _ => Err(LexerError::UnexpectedCharacter(current_char, self.line, self.column))
        }
    }
    
    /// Get current character
    fn current_char(&self) -> Option<char> {
        self.source.chars().nth(self.position)
    }
    
    /// Peek at next character
    fn peek_char(&self) -> Option<char> {
        self.source.chars().nth(self.position + 1)
    }
    
    /// Advance to next character
    fn advance(&mut self) {
        if let Some(ch) = self.current_char() {
            self.position += 1;
            if ch == '\n' {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
        }
    }
    
    /// Skip whitespace characters
    fn skip_whitespace(&mut self) {
        while let Some(ch) = self.current_char() {
            if ch.is_whitespace() {
                self.advance();
            } else {
                break;
            }
        }
    }
    
    /// Read until newline
    fn read_until_newline(&mut self) -> String {
        let mut content = String::new();
        while let Some(ch) = self.current_char() {
            if ch == '\n' {
                break;
            }
            content.push(ch);
            self.advance();
        }
        content
    }
    
    /// Read until comment end
    fn read_until_comment_end(&mut self) -> Result<String, LexerError> {
        let mut content = String::new();
        while let Some(ch) = self.current_char() {
            if ch == '*' && self.peek_char() == Some('/') {
                self.advance(); // Skip *
                self.advance(); // Skip /
                return Ok(content);
            }
            content.push(ch);
            self.advance();
        }
        Err(LexerError::UnterminatedComment(self.line, self.column))
    }
    
    /// Read string literal
    fn read_string(&mut self, quote: char) -> Result<String, LexerError> {
        let mut content = String::new();
        while let Some(ch) = self.current_char() {
            if ch == quote {
                self.advance(); // Skip closing quote
                return Ok(content);
            }
            if ch == '\\' {
                self.advance();
                if let Some(escaped) = self.current_char() {
                    content.push(match escaped {
                        'n' => '\n',
                        't' => '\t',
                        'r' => '\r',
                        '\\' => '\\',
                        '\'' => '\'',
                        '"' => '"',
                        _ => escaped,
                    });
                    self.advance();
                }
            } else {
                content.push(ch);
                self.advance();
            }
        }
        Err(LexerError::UnterminatedString(self.line, self.column))
    }
    
    /// Read number
    fn read_number(&mut self) -> String {
        let mut number = String::new();
        while let Some(ch) = self.current_char() {
            if ch.is_ascii_digit() || ch == '.' {
                number.push(ch);
                self.advance();
            } else {
                break;
            }
        }
        number
    }
    
    /// Read identifier
    fn read_identifier(&mut self) -> String {
        let mut identifier = String::new();
        while let Some(ch) = self.current_char() {
            if ch.is_alphanumeric() || ch == '_' {
                identifier.push(ch);
                self.advance();
            } else {
                break;
            }
        }
        identifier
    }
    
    /// Create a token
    fn create_token(&self, token_type: TokenType, value: &str, _start_pos: usize, start_line: usize, start_column: usize) -> Token {
        Token {
            token_type,
            value: value.to_string(),
            location: SourceLocation {
                file: String::new(), // TODO: Add file name support
                line: start_line,
                column: start_column,
                position: self.position,
            },
        }
    }
}