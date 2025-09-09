#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    Illegal, Eof, Ident(String), String(String), LBrace, RBrace, LBracket, RBracket,
    LParen, RParen, Semicolon, Colon, Equal, Comma, Dot, At, Slash, Dash, Star,
    GreaterThan, LessThan, Question, Ampersand, Pipe, Bang, GenComment(String),
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

    // Add public getters
    pub fn get_position(&self) -> usize {
        self.position
    }

    pub fn get_input_slice(&self, start: usize, end: usize) -> Option<&str> {
        self.input.get(start..end).and_then(|s| std::str::from_utf8(s).ok())
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

    fn read_string(&mut self, quote: u8) -> String {
        let start = self.position + 1;
        loop {
            self.read_char();
            if self.ch == quote || self.ch == 0 { break; }
        }
        String::from_utf8_lossy(&self.input[start..self.position]).to_string()
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();
        let tok = match self.ch {
            b'{' => Token::LBrace, b'}' => Token::RBrace, b'[' => Token::LBracket,
            b']' => Token::RBracket, b'(' => Token::LParen, b')' => Token::RParen,
            b';' => Token::Semicolon, b':' => Token::Colon, b'=' => Token::Equal,
            b',' => Token::Comma, b'.' => Token::Dot, b'@' => Token::At,
            b'>' => Token::GreaterThan, b'<' => Token::LessThan, b'?' => Token::Question,
            b'!' => Token::Bang, b'&' => Token::Ampersand, b'|' => Token::Pipe,
            b'*' => Token::Star,
            b'-' => {
                if self.peek_char() == b'-' {
                    self.read_char();
                    let start = self.position + 1;
                    while self.ch != b'\n' && self.ch != 0 { self.read_char(); }
                    let content = String::from_utf8_lossy(&self.input[start..self.position]).trim().to_string();
                    return Token::GenComment(content);
                } else { Token::Dash }
            }
            b'/' => {
                if self.peek_char() == b'/' {
                    while self.ch != b'\n' && self.ch != 0 { self.read_char(); }
                    return self.next_token();
                } else if self.peek_char() == b'*' {
                    self.read_char();
                    loop {
                        self.read_char();
                        if (self.ch == b'*' && self.peek_char() == b'/') || self.ch == 0 { break; }
                    }
                    self.read_char();
                    return self.next_token();
                } else { Token::Slash }
            }
            b'"' | b'\'' => Token::String(self.read_string(self.ch)),
            0 => Token::Eof,
            _ => {
                if self.ch.is_ascii_alphabetic() || self.ch == b'_' {
                    return Token::Ident(self.read_identifier());
                } else { Token::Illegal }
            }
        };
        self.read_char();
        tok
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn test_next_token() {
        // ... test ...
    }
}
