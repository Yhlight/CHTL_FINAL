#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Illegal, Eof, Ident(String), String(String), Number(String),
    LBrace, RBrace, LBracket, RBracket, LParen, RParen, Semicolon, Colon, Comma,
    Arrow, // ->
}

pub struct Lexer<'a> {
    input: &'a [u8],
    position: usize,
    read_position: usize,
    ch: u8,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        let mut l = Lexer {
            input: input.as_bytes(), position: 0, read_position: 0, ch: 0,
        };
        l.read_char();
        l
    }

    fn read_char(&mut self) {
        self.ch = if self.read_position >= self.input.len() { 0 } else { self.input[self.read_position] };
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() { 0 } else { self.input[self.read_position] }
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() { self.read_char(); }
    }

    fn read_identifier(&mut self) -> String {
        let start = self.position;
        while self.ch.is_ascii_alphanumeric() || self.ch == b'_' { self.read_char(); }
        String::from_utf8_lossy(&self.input[start..self.position]).to_string()
    }

    fn read_number(&mut self) -> String {
        let start = self.position;
        while self.ch.is_ascii_digit() || self.ch == b'.' { self.read_char(); }
        String::from_utf8_lossy(&self.input[start..self.position]).to_string()
    }

    fn read_string(&mut self) -> String {
        let start = self.position + 1;
        loop {
            self.read_char();
            if self.ch == b'"' || self.ch == 0 { break; }
        }
        String::from_utf8_lossy(&self.input[start..self.position]).to_string()
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();
        let tok = match self.ch {
            b'{' => Token::LBrace, b'}' => Token::RBrace, b'[' => Token::LBracket,
            b']' => Token::RBracket, b'(' => Token::LParen, b')' => Token::RParen,
            b';' => Token::Semicolon, b':' => Token::Colon, b',' => Token::Comma,
            b'-' => {
                if self.peek_char() == b'>' {
                    self.read_char();
                    Token::Arrow
                } else { Token::Illegal }
            }
            b'"' => Token::String(self.read_string()),
            0 => Token::Eof,
            _ => {
                if self.ch.is_ascii_alphabetic() {
                    return Token::Ident(self.read_identifier());
                } else if self.ch.is_ascii_digit() {
                    return Token::Number(self.read_number());
                } else {
                    Token::Illegal
                }
            }
        };
        self.read_char();
        tok
    }
}
