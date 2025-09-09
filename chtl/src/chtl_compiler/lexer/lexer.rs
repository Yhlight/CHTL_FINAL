use super::token::Token;

pub struct Lexer<'a> {
    input: &'a str,
    position: usize,      // current position in input (points to current char)
    read_position: usize, // current reading position in input (after current char)
    ch: u8,               // current char under examination
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
            self.ch = 0; // ASCII code for "NUL"
        } else {
            self.ch = self.input.as_bytes()[self.read_position];
        }
        self.position = self.read_position;
        self.read_position += 1;
    }

    fn skip_whitespace(&mut self) {
        while self.ch.is_ascii_whitespace() {
            self.read_char();
        }
    }

    fn peek_char(&self) -> u8 {
        if self.read_position >= self.input.len() {
            0
        } else {
            self.input.as_bytes()[self.read_position]
        }
    }

    pub fn next_token(&mut self) -> Token {
        self.skip_whitespace();

        let tok = match self.ch {
            b'{' => Token::OpenBrace,
            b'}' => Token::CloseBrace,
            b'(' => Token::OpenParen,
            b')' => Token::CloseParen,
            b'[' => {
                if self.input[self.position..].starts_with("[Template]") {
                    // Advance the lexer past the keyword
                    for _ in 0.."[Template]".len() {
                        self.read_char();
                    }
                    return Token::TemplateKeyword;
                } else if self.input[self.position..].starts_with("[Import]") {
                    // Advance the lexer past the keyword
                    for _ in 0.."[Import]".len() {
                        self.read_char();
                    }
                    return Token::ImportKeyword;
                }
                Token::OpenBracket
            }
            b']' => Token::CloseBracket,
            b':' => Token::Colon,
            b';' => Token::Semicolon,
            b'=' => Token::Equal,
            b'@' => Token::At,
            b'#' => Token::Hash,
            b'.' => Token::Dot,
            b',' => Token::Comma,
            b'?' => Token::Question,
            b'>' => Token::GreaterThan,
            b'<' => Token::LessThan,
            b'&' => {
                if self.peek_char() == b'&' {
                    self.read_char();
                    // This should be a distinct token, but for now we'll represent it as an identifier
                    // This needs a proper Token::And variant later.
                    return Token::Identifier("&&".to_string());
                }
                Token::Illegal("&".to_string())
            }
            b'|' => {
                if self.peek_char() == b'|' {
                    self.read_char();
                    // This should be a distinct token, but for now we'll represent it as an identifier
                    // This needs a proper Token::Or variant later.
                    return Token::Identifier("||".to_string());
                }
                Token::Illegal("|".to_string())
            }
            b'"' | b'\'' => return self.read_string(self.ch),
            b'-' => {
                if self.peek_char() == b'-' {
                    self.read_char(); // consume first '-'
                    self.read_char(); // consume second '-'
                    let content = self.read_line_comment();
                    return Token::GeneratorComment(content);
                }
                Token::Illegal(self.ch.to_string())
            }
            b'/' => {
                match self.peek_char() {
                    b'/' => {
                        self.read_char(); // consume first '/'
                        self.read_char(); // consume second '/'
                        let content = self.read_line_comment();
                        return Token::LineComment(content);
                    }
                    b'*' => {
                        self.read_char(); // consume '/'
                        self.read_char(); // consume '*'
                        let content = self.read_block_comment();
                        return Token::BlockComment(content);
                    }
                    _ => Token::Illegal(self.ch.to_string())
                }
            }
            0 => Token::EndOfFile,
            _ => {
                if self.is_identifier_start(self.ch) {
                    let ident = self.read_identifier();
                    // CHTL allows unquoted literals, which are parsed as identifiers.
                    // The parser will determine how to treat them.
                    return Token::Identifier(ident);
                } else if self.ch.is_ascii_digit() {
                    // For now, we'll treat numbers as part of an unquoted literal/identifier
                    // This might need to be revised for conditional expressions.
                    let ident = self.read_identifier();
                    return Token::Identifier(ident);
                } else {
                    return Token::Illegal(self.ch.to_string());
                }
            }
        };

        self.read_char();
        tok
    }

    fn is_identifier_start(&self, ch: u8) -> bool {
        ch.is_ascii_alphabetic() || ch == b'_'
    }

    // CHTL identifiers can contain numbers (e.g. h1) and be part of unquoted literals
    // CSS properties also use hyphens.
    fn is_identifier_char(&self, ch: u8) -> bool {
        ch.is_ascii_alphanumeric() || ch == b'_' || ch == b'-'
    }

    fn read_identifier(&mut self) -> String {
        let position = self.position;
        while self.is_identifier_char(self.ch) {
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

    fn read_line_comment(&mut self) -> String {
        let position = self.position;
        while self.ch != b'\n' && self.ch != 0 {
            self.read_char();
        }
        self.input[position..self.position].trim().to_string()
    }

    fn read_block_comment(&mut self) -> String {
        let position = self.position;
        while !(self.ch == b'*' && self.peek_char() == b'/') && self.ch != 0 {
            self.read_char();
        }
        if self.ch == 0 {
            // This should probably be an error in a real compiler
            return self.input[position..self.position].to_string();
        }
        let content = self.input[position..self.position].trim().to_string();
        self.read_char(); // consume '*'
        self.read_char(); // consume '/'
        content
    }
}
