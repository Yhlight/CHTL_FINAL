package lexer

type Lexer struct {
	input        string
	position     int
	readPosition int
	ch           byte
}

func New(input string) *Lexer {
	l := &Lexer{input: input}
	l.readChar()
	return l
}

func (l *Lexer) GetInput() string {
	return l.input
}

func (l *Lexer) readChar() {
	if l.readPosition >= len(l.input) {
		l.ch = 0
	} else {
		l.ch = l.input[l.readPosition]
	}
	l.position = l.readPosition
	l.readPosition++
}

func (l *Lexer) peekChar() byte {
	if l.readPosition >= len(l.input) {
		return 0
	}
	return l.input[l.readPosition]
}

func (l *Lexer) NextToken() Token {
	var tok Token
	startPos := l.position

	l.skipWhitespace()
	startPos = l.position // Update position after skipping whitespace

	switch l.ch {
	case '=': tok = l.newToken(ASSIGN, l.ch)
	case ':': tok = l.newToken(COLON, l.ch)
	case ';': tok = l.newToken(SEMICOLON, l.ch)
	case '+': tok = l.newToken(PLUS, l.ch)
	case '-':
		if l.peekChar() == '-' {
			l.readChar(); l.readChar()
			tok.Type = GEN_COMMENT
			tok.Literal = l.readLineComment()
			tok.Position = startPos
			return tok
		}
		tok = l.newToken(MINUS, l.ch)
	case '*': tok = l.newToken(ASTERISK, l.ch)
	case '/':
		if l.peekChar() == '/' {
			l.readChar(); l.readChar()
			tok.Type = COMMENT
			tok.Literal = l.readLineComment()
			tok.Position = startPos
			return tok
		} else if l.peekChar() == '*' {
			l.readChar(); l.readChar()
			tok.Type = COMMENT
			tok.Literal = l.readBlockComment()
			tok.Position = startPos
			return tok
		}
		tok = l.newToken(SLASH, l.ch)
	case '{': tok = l.newToken(LBRACE, l.ch)
	case '}': tok = l.newToken(RBRACE, l.ch)
	case '[': tok = l.newToken(LBRACKET, l.ch)
	case ']': tok = l.newToken(RBRACKET, l.ch)
	case '@': tok = l.newToken(AT, l.ch)
	case '.': tok = l.newToken(DOT, l.ch)
	case '#': tok = l.newToken(HASH, l.ch)
	case '&': tok = l.newToken(AMPERSAND, l.ch)
	case '"', '\'':
		tok.Type = STRING
		tok.Literal = l.readString(l.ch)
		tok.Position = startPos
		l.readChar()
		return tok
	case 0:
		tok.Literal = ""
		tok.Type = EOF
		tok.Position = startPos
	default:
		if isLetter(l.ch) {
			tok.Literal = l.readIdentifier()
			tok.Type = LookupIdent(tok.Literal)
			tok.Position = startPos
			return tok
		} else if isDigit(l.ch) {
			tok.Type = NUMBER
			tok.Literal = l.readNumber()
			tok.Position = startPos
			return tok
		} else {
			tok = l.newToken(ILLEGAL, l.ch)
		}
	}

	l.readChar()
	return tok
}

func (l *Lexer) skipWhitespace() { for l.ch == ' ' || l.ch == '\t' || l.ch == '\n' || l.ch == '\r' { l.readChar() } }
func (l *Lexer) readIdentifier() string { position := l.position; for isIdentifierChar(l.ch) { l.readChar() }; return l.input[position:l.position] }
func (l *Lexer) readString(quote byte) string { position := l.position + 1; for { l.readChar(); if l.ch == quote || l.ch == 0 { break } }; return l.input[position:l.position] }
func (l *Lexer) readLineComment() string { position := l.position; for l.ch != '\n' && l.ch != 0 { l.readChar() }; return l.input[position:l.position] }
func (l *Lexer) readBlockComment() string { position := l.position; for !(l.ch == '*' && l.peekChar() == '/') && l.ch != 0 { l.readChar() }; if l.ch != 0 { l.readChar(); l.readChar() }; return l.input[position : l.position-2] }
func (l *Lexer) readNumber() string {
	position := l.position
	for isDigit(l.ch) {
		l.readChar()
	}
	if l.ch == '.' && isDigit(l.peekChar()) {
		l.readChar()
		for isDigit(l.ch) {
			l.readChar()
		}
	}
	return l.input[position:l.position]
}

func isLetter(ch byte) bool { return 'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch == '_' }
func isDigit(ch byte) bool { return '0' <= ch && ch <= '9' }
func isIdentifierChar(ch byte) bool { return isLetter(ch) || isDigit(ch) || ch == '-' }
func (l *Lexer) newToken(tokenType TokenType, ch byte) Token {
	return Token{Type: tokenType, Literal: string(ch), Position: l.position}
}
