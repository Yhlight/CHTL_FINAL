use super::token::Token;

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input,
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
            self.ch = self.input.as_bytes()[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
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
            b'{' => {
                if self.peek_char() == b'{' {
                    self.read_char(); // consume first {
                    self.read_char(); // consume second {
                    let content = self.read_selector_content();
                    return Token::SelectorContent(content);
                }
                Token::OpenBrace
            }
            b'(' => Token::OpenParen,
            b')' => Token::CloseParen,
            b',' => Token::Comma,
            b';' => Token::Semicolon,
            b':' => Token::Colon,
            b'.' => Token::Dot,
            b'-' => {
                if self.peek_char() == b'>' {
                    self.read_char();
                    return Token::Arrow;
                }
                Token::Illegal("-".to_string())
            }
            b'=' => {
                if self.peek_char() == b'>' {
                    self.read_char();
                    return Token::FatArrow;
                }
                Token::Illegal("=".to_string())
            }
            b'"' | b'\'' => return self.read_string(self.ch),
            _ => {
                if self.ch.is_ascii_alphabetic() || self.ch == b'_' {
                    let ident = self.read_identifier();
                    return Token::Identifier(ident);
                } else if self.ch == 0 {
                    return Token::EndOfFile;
                }
                Token::Illegal(self.ch.to_string())
            }
        };

        self.read_char();
        tok
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' {
            self.read_char();
        }
        self.input[position..self.position].to_string()
    }

    fn read_string(&mut self, quote_type: u8) -> Token {
        let position = self.position + 1;
        loop {
            self.read_char();
            if self.ch == quote_type || self.ch == 0 {
                break;
            }
        }
        if self.ch == 0 {
            Token::Illegal("Unterminated string".to_string())
        } else {
            // consume closing quote
            self.read_char();
            Token::StringLiteral(self.input[position..self.position - 1].to_string())
        }
    }

    fn read_selector_content(&mut self) -> String {
        let position = self.position;
        while !(self.ch == b'}' && self.peek_char() == b'}') && self.ch != 0 {
            self.read_char();
        }
        let content = self.input[position..self.position].trim().to_string();
        if self.ch != 0 {
            self.read_char(); // consume first }
            self.read_char(); // consume second }
        }
        content
    }
}
