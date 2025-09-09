//! Lexical analysis for CHTL

use crate::error::{ChtlError, Result};
use std::collections::HashMap;

/// Token types in CHTL
#[derive(Debug, Clone, PartialEq)]
pub enum TokenType {
    // Literals
    Identifier(String),
    String(String),
    Number(String),
    
    // Operators
    Colon,           // :
    Equal,           // =
    Semicolon,       // ;
    Comma,           // ,
    Dot,             // .
    Slash,           // /
    Backslash,       // \
    
    // Brackets
    LeftParen,       // (
    RightParen,      // )
    LeftBracket,     // [
    RightBracket,    // ]
    LeftBrace,       // {
    RightBrace,      // }
    LeftAngle,       // <
    RightAngle,      // >
    
    // Keywords
    Text,            // text
    Style,           // style
    Script,          // script
    Template,        // [Template]
    Custom,          // [Custom]
    Origin,          // [Origin]
    Import,          // [Import]
    Namespace,       // [Namespace]
    Configuration,   // [Configuration]
    Info,            // [Info]
    Export,          // [Export]
    Name,            // [Name]
    
    // Style keywords
    AtStyle,         // @Style
    AtElement,       // @Element
    AtVar,           // @Var
    AtHtml,          // @Html
    AtJavaScript,    // @JavaScript
    AtChtl,          // @Chtl
    AtCJmod,         // @CJmod
    AtConfig,        // @Config
    
    // Special keywords
    Inherit,         // inherit
    Delete,          // delete
    Insert,          // insert
    After,           // after
    Before,          // before
    Replace,         // replace
    AtTop,           // at top
    AtBottom,        // at bottom
    From,            // from
    As,              // as
    Except,          // except
    Use,             // use
    Html5,           // html5
    
    // Comments
    LineComment,     // //
    BlockComment,    // /* */
    GeneratorComment, // --
    
    // Whitespace
    Whitespace,
    Newline,
    
    // End of file
    Eof,
}

/// A token in the CHTL source code
#[derive(Debug, Clone)]
pub struct Token {
    pub token_type: TokenType,
    pub position: usize,
    pub line: usize,
    pub column: usize,
    pub length: usize,
}

impl Token {
    pub fn new(token_type: TokenType, position: usize, line: usize, column: usize, length: usize) -> Self {
        Self {
            token_type,
            position,
            line,
            column,
            length,
        }
    }
}

/// CHTL lexer
pub struct Lexer {
    source: String,
    position: usize,
    line: usize,
    column: usize,
    keywords: HashMap<String, TokenType>,
}

impl Lexer {
    /// Create a new lexer for the given source code
    pub fn new(source: String) -> Self {
        let mut keywords = HashMap::new();
        
        // Add keywords
        keywords.insert("text".to_string(), TokenType::Text);
        keywords.insert("style".to_string(), TokenType::Style);
        keywords.insert("script".to_string(), TokenType::Script);
        keywords.insert("inherit".to_string(), TokenType::Inherit);
        keywords.insert("delete".to_string(), TokenType::Delete);
        keywords.insert("insert".to_string(), TokenType::Insert);
        keywords.insert("after".to_string(), TokenType::After);
        keywords.insert("before".to_string(), TokenType::Before);
        keywords.insert("replace".to_string(), TokenType::Replace);
        keywords.insert("at top".to_string(), TokenType::AtTop);
        keywords.insert("at bottom".to_string(), TokenType::AtBottom);
        keywords.insert("from".to_string(), TokenType::From);
        keywords.insert("as".to_string(), TokenType::As);
        keywords.insert("except".to_string(), TokenType::Except);
        keywords.insert("use".to_string(), TokenType::Use);
        keywords.insert("html5".to_string(), TokenType::Html5);
        
        // Add @ keywords
        keywords.insert("@Style".to_string(), TokenType::AtStyle);
        keywords.insert("@Element".to_string(), TokenType::AtElement);
        keywords.insert("@Var".to_string(), TokenType::AtVar);
        keywords.insert("@Html".to_string(), TokenType::AtHtml);
        keywords.insert("@JavaScript".to_string(), TokenType::AtJavaScript);
        keywords.insert("@Chtl".to_string(), TokenType::AtChtl);
        keywords.insert("@CJmod".to_string(), TokenType::AtCJmod);
        keywords.insert("@Config".to_string(), TokenType::AtConfig);
        
        Self {
            source,
            position: 0,
            line: 1,
            column: 1,
            keywords,
        }
    }
    
    /// Tokenize the source code
    pub fn tokenize(&mut self) -> Result<Vec<Token>> {
        let mut tokens = Vec::new();
        
        while self.position < self.source.len() {
            let ch = self.current_char();
            
            match ch {
                // Whitespace
                ' ' | '\t' => {
                    let start_pos = self.position;
                    while self.position < self.source.len() 
                        && matches!(self.current_char(), ' ' | '\t') {
                        self.advance();
                    }
                    tokens.push(Token::new(
                        TokenType::Whitespace,
                        start_pos,
                        self.line,
                        self.column,
                        self.position - start_pos,
                    ));
                }
                
                // Newline
                '\n' => {
                    let pos = self.position;
                    self.advance();
                    self.line += 1;
                    self.column = 1;
                    tokens.push(Token::new(TokenType::Newline, pos, self.line - 1, self.column, 1));
                }
                
                // Comments
                '/' if self.peek() == Some('/') => {
                    self.lex_line_comment(&mut tokens)?;
                }
                '/' if self.peek() == Some('*') => {
                    self.lex_block_comment(&mut tokens)?;
                }
                '-' if self.peek() == Some('-') => {
                    self.lex_generator_comment(&mut tokens)?;
                }
                
                // String literals
                '"' => {
                    self.lex_string(&mut tokens, '"')?;
                }
                '\'' => {
                    self.lex_string(&mut tokens, '\'')?;
                }
                
                // Single character tokens
                ':' => {
                    self.add_token(TokenType::Colon, &mut tokens);
                    self.advance();
                }
                '=' => {
                    self.add_token(TokenType::Equal, &mut tokens);
                    self.advance();
                }
                ';' => {
                    self.add_token(TokenType::Semicolon, &mut tokens);
                    self.advance();
                }
                ',' => {
                    self.add_token(TokenType::Comma, &mut tokens);
                    self.advance();
                }
                '.' => {
                    self.add_token(TokenType::Dot, &mut tokens);
                    self.advance();
                }
                '/' => {
                    self.add_token(TokenType::Slash, &mut tokens);
                    self.advance();
                }
                '\\' => {
                    self.add_token(TokenType::Backslash, &mut tokens);
                    self.advance();
                }
                '(' => {
                    self.add_token(TokenType::LeftParen, &mut tokens);
                    self.advance();
                }
                ')' => {
                    self.add_token(TokenType::RightParen, &mut tokens);
                    self.advance();
                }
                '[' => {
                    self.lex_bracket_token(&mut tokens)?;
                }
                ']' => {
                    self.add_token(TokenType::RightBracket, &mut tokens);
                    self.advance();
                }
                '{' => {
                    self.add_token(TokenType::LeftBrace, &mut tokens);
                    self.advance();
                }
                '}' => {
                    self.add_token(TokenType::RightBrace, &mut tokens);
                    self.advance();
                }
                '<' => {
                    self.add_token(TokenType::LeftAngle, &mut tokens);
                    self.advance();
                }
                '>' => {
                    self.add_token(TokenType::RightAngle, &mut tokens);
                    self.advance();
                }
                
                // @ symbol for keywords
                '@' => {
                    self.lex_at_keyword(&mut tokens)?;
                }
                
                // Identifiers and numbers
                c if c.is_alphabetic() || c == '_' => {
                    self.lex_identifier(&mut tokens)?;
                }
                c if c.is_numeric() => {
                    self.lex_number(&mut tokens)?;
                }
                
                _ => {
                    return Err(ChtlError::lexical(
                        format!("Unexpected character: '{}'", ch),
                        self.position,
                    ));
                }
            }
        }
        
        tokens.push(Token::new(TokenType::Eof, self.position, self.line, self.column, 0));
        Ok(tokens)
    }
    
    fn current_char(&self) -> char {
        self.source.chars().nth(self.position).unwrap_or('\0')
    }
    
    fn peek(&self) -> Option<char> {
        self.source.chars().nth(self.position + 1)
    }
    
    fn advance(&mut self) {
        if self.position < self.source.len() {
            self.position += 1;
            self.column += 1;
        }
    }
    
    fn add_token(&mut self, token_type: TokenType, tokens: &mut Vec<Token>) {
        let pos = self.position;
        let line = self.line;
        let col = self.column;
        tokens.push(Token::new(token_type, pos, line, col, 1));
    }
    
    fn lex_line_comment(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        // Skip //
        self.advance();
        self.advance();
        
        // Read until newline or EOF
        while self.position < self.source.len() && self.current_char() != '\n' {
            self.advance();
        }
        
        let content = &self.source[start_pos + 2..self.position];
        tokens.push(Token::new(
            TokenType::LineComment,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_block_comment(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        // Skip /*
        self.advance();
        self.advance();
        
        // Read until */
        while self.position < self.source.len() - 1 {
            if self.current_char() == '*' && self.peek() == Some('/') {
                self.advance();
                self.advance();
                break;
            }
            if self.current_char() == '\n' {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            self.advance();
        }
        
        if self.position >= self.source.len() {
            return Err(ChtlError::lexical(
                "Unterminated block comment".to_string(),
                start_pos,
            ));
        }
        
        tokens.push(Token::new(
            TokenType::BlockComment,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_generator_comment(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        // Skip --
        self.advance();
        self.advance();
        
        // Read until newline or EOF
        while self.position < self.source.len() && self.current_char() != '\n' {
            self.advance();
        }
        
        tokens.push(Token::new(
            TokenType::GeneratorComment,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_string(&mut self, tokens: &mut Vec<Token>, quote: char) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        // Skip opening quote
        self.advance();
        
        // Read until closing quote
        while self.position < self.source.len() && self.current_char() != quote {
            if self.current_char() == '\\' {
                self.advance(); // Skip escape character
            }
            if self.current_char() == '\n' {
                self.line += 1;
                self.column = 1;
            } else {
                self.column += 1;
            }
            self.advance();
        }
        
        if self.position >= self.source.len() {
            return Err(ChtlError::lexical(
                format!("Unterminated string literal starting with '{}'", quote),
                start_pos,
            ));
        }
        
        // Skip closing quote
        self.advance();
        
        let content = &self.source[start_pos + 1..self.position - 1];
        tokens.push(Token::new(
            TokenType::String(content.to_string()),
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_bracket_token(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        // Skip [
        self.advance();
        
        // Read identifier until ]
        let mut identifier = String::new();
        while self.position < self.source.len() && self.current_char() != ']' {
            identifier.push(self.current_char());
            self.advance();
        }
        
        if self.position >= self.source.len() {
            return Err(ChtlError::lexical(
                "Unterminated bracket token".to_string(),
                start_pos,
            ));
        }
        
        // Skip ]
        self.advance();
        
        let token_type = match identifier.as_str() {
            "Template" => TokenType::Template,
            "Custom" => TokenType::Custom,
            "Origin" => TokenType::Origin,
            "Import" => TokenType::Import,
            "Namespace" => TokenType::Namespace,
            "Configuration" => TokenType::Configuration,
            "Info" => TokenType::Info,
            "Export" => TokenType::Export,
            "Name" => TokenType::Name,
            _ => return Err(ChtlError::lexical(
                format!("Unknown bracket token: [{}]", identifier),
                start_pos,
            )),
        };
        
        tokens.push(Token::new(
            token_type,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_identifier(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        let mut identifier = String::new();
        while self.position < self.source.len() {
            let ch = self.current_char();
            if ch.is_alphanumeric() || ch == '_' || ch == '-' {
                identifier.push(ch);
                self.advance();
            } else {
                break;
            }
        }
        
        let token_type = if let Some(&keyword_type) = self.keywords.get(&identifier) {
            keyword_type
        } else {
            TokenType::Identifier(identifier)
        };
        
        tokens.push(Token::new(
            token_type,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_number(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        let mut number = String::new();
        while self.position < self.source.len() {
            let ch = self.current_char();
            if ch.is_numeric() || ch == '.' {
                number.push(ch);
                self.advance();
            } else {
                break;
            }
        }
        
        tokens.push(Token::new(
            TokenType::Number(number),
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
    
    fn lex_at_keyword(&mut self, tokens: &mut Vec<Token>) -> Result<()> {
        let start_pos = self.position;
        let start_line = self.line;
        let start_col = self.column;
        
        self.advance(); // consume @
        
        let mut keyword = String::from("@");
        while self.position < self.source.len() {
            let ch = self.current_char();
            if ch.is_alphanumeric() || ch == '_' {
                keyword.push(ch);
                self.advance();
            } else {
                break;
            }
        }
        
        let token_type = if let Some(&keyword_type) = self.keywords.get(&keyword) {
            keyword_type
        } else {
            TokenType::Identifier(keyword)
        };
        
        tokens.push(Token::new(
            token_type,
            start_pos,
            start_line,
            start_col,
            self.position - start_pos,
        ));
        
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    
    #[test]
    fn test_lexer_basic() {
        let source = r#"div {
    text: "Hello World";
}"#;
        let mut lexer = Lexer::new(source.to_string());
        let tokens = lexer.tokenize().unwrap();
        
        assert!(!tokens.is_empty());
        assert_eq!(tokens[0].token_type, TokenType::Identifier("div".to_string()));
    }
    
    #[test]
    fn test_lexer_comments() {
        let source = r#"// This is a comment
/* This is a block comment */
-- This is a generator comment"#;
        let mut lexer = Lexer::new(source.to_string());
        let tokens = lexer.tokenize().unwrap();
        
        assert_eq!(tokens[0].token_type, TokenType::LineComment);
        assert_eq!(tokens[2].token_type, TokenType::BlockComment);
        assert_eq!(tokens[4].token_type, TokenType::GeneratorComment);
    }
}