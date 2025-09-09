//! This module defines the Lexer, which is responsible for turning CHTL source code
//! into a stream of tokens.

use crate::token::Token;

pub struct Lexer<'a> {
    input: &'a [u8],
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input: input.as_bytes(),
            position: 0,
            read_position: 0,
            ch: 0,
        };
        l.read_char();
        l
    }

    fn read_char(&mut self) {
        if self.read_position >= self.input.len() {
            self.ch = 0;
        } else {
            self.ch = self.input[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input[self.read_position]
        }
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            b'=' => { self.read_char(); Token::Assign }
            b';' => { self.read_char(); Token::Semicolon }
            b'(' => { self.read_char(); Token::LParen }
            b')' => { self.read_char(); Token::RParen }
            b',' => { self.read_char(); Token::Comma }
            b'{' => { self.read_char(); Token::LBrace }
            b'}' => { self.read_char(); Token::RBrace }
            b'[' => { self.read_char(); Token::LBracket }
            b']' => { self.read_char(); Token::RBracket }
            b':' => { self.read_char(); Token::Colon }
            b'!' => { self.read_char(); Token::Bang }
            b'+' => { self.read_char(); Token::Plus }
            b'*' => { self.read_char(); Token::Asterisk }
            b'@' => { self.read_char(); Token::At }
            b'#' => { self.read_char(); Token::Hash }
            b'.' => { self.read_char(); Token::Dot }
            b'?' => { self.read_char(); Token::Question }
            b'>' => { self.read_char(); Token::Gt }
            b'<' => { self.read_char(); Token::Lt }
            b'&' if self.peek_char() == b'&' => { self.read_char(); self.read_char(); Token::LogicalAnd }
            b'|' if self.peek_char() == b'|' => { self.read_char(); self.read_char(); Token::LogicalOr }
            b'-' if self.peek_char() == b'>' => { self.read_char(); self.read_char(); Token::Arrow }
            b'-' if self.peek_char() == b'-' => { self.read_char(); self.read_char(); Token::GeneratorComment(self.read_comment_line()) }
            b'-' => { self.read_char(); Token::Minus }
            b'/' if self.peek_char() == b'/' => { self.read_comment_line(); self.next_token() }
            b'/' if self.peek_char() == b'*' => { self.read_multiline_comment(); self.next_token() }
            b'/' => { self.read_char(); Token::Slash }
            b'&' => { self.read_char(); Token::Ampersand }
            b'"' | b'\'' => {
                let quote = self.ch;
                let content = self.read_string(quote);
                self.read_char();
                Token::String(content)
            }
            0 => Token::Eof,
            _ => {
                if self.ch.is_ascii_digit() {
                    return Token::Number(self.read_number());
                }
                if is_identifier_start(self.ch) {
                    return Token::Ident(self.read_identifier());
                }
                let ch = self.ch;
                self.read_char();
                Token::Illegal(String::from_utf8_lossy(&[ch]).to_string())
            }
        };
        tok
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while is_identifier_char(self.ch) {
            self.read_char();
        }
        String::from_utf8_lossy(&self.input[position..self.position]).to_string()
    }

    fn read_number(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_digit() {
            self.read_char();
        }
        if self.ch == b'.' {
            self.read_char();
            while self.ch.is_ascii_digit() {
                self.read_char();
            }
        }
        String::from_utf8_lossy(&self.input[position..self.position]).to_string()
    }

    fn read_string(&mut self, quote: u8) -> String {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == quote || self.ch == 0 {
                break;
            }
        }
        String::from_utf8_lossy(&self.input[position..self.position]).to_string()
    }

    fn read_comment_line(&mut self) -> String {
        let position = self.position;
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
        String::from_utf8_lossy(&self.input[position..self.position]).trim().to_string()
    }

    fn read_multiline_comment(&mut self) {
        self.read_char();
        self.read_char();
        loop {
            if self.ch == b'*' && self.peek_char() == b'/' {
                self.read_char();
                self.read_char();
                break;
            }
            if self.ch == 0 {
                break;
            }
            self.read_char();
        }
    }
}

fn is_identifier_start(ch: u8) -> bool {
    ch.is_ascii_alphabetic() || ch == b'_'
}

fn is_identifier_char(ch: u8) -> bool {
    ch.is_ascii_alphanumeric() || ch == b'_' || ch == b'-'
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::token::Token;

    #[test]
    fn test_next_token_simple() {
        let input = "div { id: box; }";
        let tests = vec![
            Token::Ident("div".to_string()),
            Token::LBrace,
            Token::Ident("id".to_string()),
            Token::Colon,
            Token::Ident("box".to_string()),
            Token::Semicolon,
            Token::RBrace,
            Token::Eof,
        ];
        let mut l = Lexer::new(input);
        for expected_token in tests {
            assert_eq!(l.next_token(), expected_token);
        }
    }

    #[test]
    fn test_css_value_as_identifier() {
        let input = "width: 100px;";
        let tests = vec![
            Token::Ident("width".to_string()),
            Token::Colon,
            Token::Number("100".to_string()),
            Token::Ident("px".to_string()),
            Token::Semicolon,
            Token::Eof,
        ];
        let mut l = Lexer::new(input);
        for expected_token in tests {
            assert_eq!(l.next_token(), expected_token);
        }
    }

    #[test]
    fn test_comments() {
        let input = r#"
            // This is a single line comment
            div {
                -- This is a generator comment
                /* This is a
                   multiline comment */
                color: red;
            }
        "#;
        let tests = vec![
            Token::Ident("div".to_string()),
            Token::LBrace,
            Token::GeneratorComment("This is a generator comment".to_string()),
            Token::Ident("color".to_string()),
            Token::Colon,
            Token::Ident("red".to_string()),
            Token::Semicolon,
            Token::RBrace,
            Token::Eof,
        ];
        let mut l = Lexer::new(input);
        for expected_token in tests {
            assert_eq!(l.next_token(), expected_token);
        }
    }

    #[test]
    fn test_all_symbols() {
        let input = r#"=;(),{}[];:@#.>&&||->?"#;
        let tests = vec![
            Token::Assign,
            Token::Semicolon,
            Token::LParen,
            Token::RParen,
            Token::Comma,
            Token::LBrace,
            Token::RBrace,
            Token::LBracket,
            Token::RBracket,
            Token::Semicolon,
            Token::Colon,
            Token::At,
            Token::Hash,
            Token::Dot,
            Token::Gt,
            Token::LogicalAnd,
            Token::LogicalOr,
            Token::Arrow,
            Token::Question,
            Token::Eof,
        ];
        let mut l = Lexer::new(input);
        for expected_token in tests {
            assert_eq!(l.next_token(), expected_token, "Failed on input: {}", input);
        }
    }

    #[test]
    fn test_numbers_and_operators() {
        let input = "10 + 20.5 * 3 / 4 - 5 !";
        let tests = vec![
            Token::Number("10".to_string()),
            Token::Plus,
            Token::Number("20.5".to_string()),
            Token::Asterisk,
            Token::Number("3".to_string()),
            Token::Slash,
            Token::Number("4".to_string()),
            Token::Minus,
            Token::Number("5".to_string()),
            Token::Bang,
            Token::Eof,
        ];
        let mut l = Lexer::new(input);
        for expected_token in tests {
            assert_eq!(l.next_token(), expected_token);
        }
    }
}
