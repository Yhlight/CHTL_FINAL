package lexer

import (
	"unicode"
)

// Lexer 词法分析器
type Lexer struct {
	input        string
	position     int
	readPosition int
	ch           byte
	line         int
	column       int
	lineStart    int
	keywords     map[string]TokenType
}

// NewLexer 创建新的词法分析器
func NewLexer(input string) *Lexer {
	l := &Lexer{
		input:    input,
		line:     1,
		column:   1,
		keywords: makeKeywords(),
	}
	l.readChar()
	return l
}

// makeKeywords 创建关键字映射
func makeKeywords() map[string]TokenType {
	return map[string]TokenType{
		"text":           KEYWORD_TEXT,
		"style":          KEYWORD_STYLE,
		"script":         KEYWORD_SCRIPT,
		"template":       KEYWORD_TEMPLATE,
		"custom":         KEYWORD_CUSTOM,
		"element":        KEYWORD_ELEMENT,
		"var":            KEYWORD_VAR,
		"from":           KEYWORD_FROM,
		"Origin":         KEYWORD_ORIGIN,
		"Import":         KEYWORD_IMPORT,
		"import":         KEYWORD_IMPORT,
		"Namespace":      KEYWORD_NAMESPACE,
		"Configuration":  KEYWORD_CONFIGURATION,
		"use":            KEYWORD_USE,
		"inherit":        KEYWORD_INHERIT,
		"delete":         KEYWORD_DELETE,
		"insert":         KEYWORD_INSERT,
		"function":       KEYWORD_FUNCTION,
		"let":            KEYWORD_LET,
		"const":          KEYWORD_CONST,
		"if":             KEYWORD_IF,
		"for":            KEYWORD_FOR,
		"while":          KEYWORD_WHILE,
		"load":           KEYWORD_LOAD,
		"local":          KEYWORD_LOCAL,
		"select":         KEYWORD_SELECT,
		"delegate":       KEYWORD_DELEGATE,
		"animate":        KEYWORD_ANIMATE,
		"virtual":        KEYWORD_VIRTUAL,
		"route":          KEYWORD_ROUTE,
		"after":          KEYWORD_AFTER,
		"before":         KEYWORD_BEFORE,
		"replace":        KEYWORD_REPLACE,
		"at":             KEYWORD_AT_TOP,
		"top":            KEYWORD_AT_TOP,
		"bottom":         KEYWORD_AT_BOTTOM,
		"as":             KEYWORD_AS,
		"except":         KEYWORD_EXCEPT,
		"html5":          KEYWORD_HTML5,
		"Var":            KEYWORD_VAR,
		"Element":        KEYWORD_ELEMENT,
		"Html":           KEYWORD_HTML,
		"JavaScript":     KEYWORD_JAVASCRIPT,
		"Chtl":           KEYWORD_CHTL,
		"CJmod":          KEYWORD_CJMOD,
		"Config":         KEYWORD_CONFIG,
		"vir":            KEYWORD_VIR,
		"listen":         KEYWORD_LISTEN,
		"router":         KEYWORD_ROUTER,
		"util":           KEYWORD_UTIL,
		"then":           KEYWORD_THEN,
		"change":         KEYWORD_CHANGE,
		"fileloader":     KEYWORD_FILELOADER,
		"iNeverAway":     KEYWORD_INEVERAWAY,
		"printMylove":    KEYWORD_PRINTMYLOVE,
	}
}

// NextToken 获取下一个词法单元
func (l *Lexer) NextToken() Token {
	var tok Token

	l.skipWhitespace()

	tok.Position = Position{
		Line:   l.line,
		Column: l.column,
		Offset: l.position,
	}

	switch l.ch {
	case '{':
		tok = l.newToken(LEFT_BRACE, l.ch)
	case '}':
		tok = l.newToken(RIGHT_BRACE, l.ch)
	case '[':
		tok = l.newToken(LEFT_BRACKET, l.ch)
	case ']':
		tok = l.newToken(RIGHT_BRACKET, l.ch)
	case '(':
		tok = l.newToken(LEFT_PAREN, l.ch)
	case ')':
		tok = l.newToken(RIGHT_PAREN, l.ch)
	case ';':
		tok = l.newToken(SEMICOLON, l.ch)
	case ':':
		if l.peekChar() == '=' {
			l.readChar()
			tok = l.newToken(ASSIGN, "=")
		} else {
			tok = l.newToken(COLON, l.ch)
		}
	case ',':
		tok = l.newToken(COMMA, l.ch)
	case '.':
		tok = l.newToken(DOT, l.ch)
	case '=':
		if l.peekChar() == '=' {
			l.readChar()
			tok = l.newToken(EQUAL, "==")
		} else {
			tok = l.newToken(EQUALS, l.ch)
		}
	case '?':
		tok = l.newToken(QUESTION, l.ch)
	case '|':
		if l.peekChar() == '|' {
			l.readChar()
			tok = l.newToken(OR, "||")
		} else {
			tok = l.newToken(PIPE, l.ch)
		}
	case '&':
		if l.peekChar() == '&' {
			l.readChar()
			tok = l.newToken(AND, "&&")
		} else {
			tok = l.newToken(AMPERSAND, l.ch)
		}
	case '#':
		// 检查是否为颜色值
		if l.peekChar() != 0 && (isDigit(l.peekChar()) || isLetter(l.peekChar())) {
			// 读取颜色值
			position := l.position
			l.readChar() // 读取 #
			for isDigit(l.ch) || isLetter(l.ch) {
				l.readChar()
			}
			return l.newToken(IDENTIFIER, l.input[position:l.position])
		}
		tok = l.newToken(HASH, l.ch)
	case '@':
		tok = l.newToken(AT, l.ch)
	case '$':
		tok = l.newToken(DOLLAR, l.ch)
	case '_':
		tok = l.newToken(UNDERSCORE, l.ch)
	case '+':
		tok = l.newToken(PLUS, l.ch)
	case '-':
		if l.peekChar() == '>' {
			l.readChar()
			tok = l.newToken(ARROW, "->")
		} else {
			tok = l.newToken(MINUS, l.ch)
		}
	case '*':
		if l.peekChar() == '*' {
			l.readChar()
			tok = l.newToken(POWER, "**")
		} else {
			tok = l.newToken(MULTIPLY, l.ch)
		}
	case '/':
		if l.peekChar() == '/' {
			tok = l.readLineComment()
		} else if l.peekChar() == '*' {
			tok = l.readBlockComment()
		} else {
			tok = l.newToken(DIVIDE, l.ch)
		}
	case '%':
		tok = l.newToken(MODULO, l.ch)
	case '<':
		if l.peekChar() == '=' {
			l.readChar()
			tok = l.newToken(LESS_EQUAL, "<=")
		} else {
			tok = l.newToken(LESS, l.ch)
		}
	case '>':
		if l.peekChar() == '=' {
			l.readChar()
			tok = l.newToken(GREATER_EQUAL, ">=")
		} else {
			tok = l.newToken(GREATER, l.ch)
		}
	case '!':
		if l.peekChar() == '=' {
			l.readChar()
			tok = l.newToken(NOT_EQUAL, "!=")
		} else {
			tok = l.newToken(NOT, l.ch)
		}
	case '"':
		tok = l.readString('"')
	case '\'':
		tok = l.readString('\'')
	case 0:
		tok = l.newToken(EOF, "")
	default:
		if isLetter(l.ch) {
			tok.Literal = l.readIdentifier()
			tok.Type = l.lookupIdent(tok.Literal)
			return tok
		} else if isDigit(l.ch) {
			tok = l.readNumber()
			return tok
		} else {
			tok = l.newToken(ILLEGAL, l.ch)
		}
	}

	l.readChar()
	return tok
}

// readChar 读取下一个字符
func (l *Lexer) readChar() {
	if l.readPosition >= len(l.input) {
		l.ch = 0
	} else {
		l.ch = l.input[l.readPosition]
	}
	l.position = l.readPosition
	l.readPosition++
	if l.ch == '\n' {
		l.line++
		l.column = 1
		l.lineStart = l.position
	} else {
		l.column++
	}
}

// peekChar 预览下一个字符
func (l *Lexer) peekChar() byte {
	if l.readPosition >= len(l.input) {
		return 0
	}
	return l.input[l.readPosition]
}

// readIdentifier 读取标识符
func (l *Lexer) readIdentifier() string {
	position := l.position
	for isLetter(l.ch) || isDigit(l.ch) || l.ch == '_' || l.ch == '-' {
		l.readChar()
	}
	return l.input[position:l.position]
}

// readNumber 读取数字
func (l *Lexer) readNumber() Token {
	position := l.position
	for isDigit(l.ch) || l.ch == '.' {
		l.readChar()
	}
	
	// 检查单位
	if l.ch == '%' {
		l.readChar()
	} else if l.ch == 'p' && l.peekChar() == 'x' {
		l.readChar() // 读取 'p'
		l.readChar() // 读取 'x'
	} else if l.ch == 'e' && l.peekChar() == 'm' {
		l.readChar() // 读取 'e'
		l.readChar() // 读取 'm'
	} else if l.ch == 'r' && l.peekChar() == 'e' && l.readPosition+1 < len(l.input) && l.input[l.readPosition+1] == 'm' {
		l.readChar() // 读取 'r'
		l.readChar() // 读取 'e'
		l.readChar() // 读取 'm'
	}
	
	// 创建token时使用原始位置
	token := l.newToken(NUMBER, l.input[position:l.position])
	token.Position = Position{Line: l.line, Column: position - l.lineStart + 1}
	return token
}

// readString 读取字符串
func (l *Lexer) readString(quote byte) Token {
	position := l.position + 1
	for {
		l.readChar()
		if l.ch == quote || l.ch == 0 {
			break
		}
	}
	return l.newToken(STRING, l.input[position:l.position])
}

// readLineComment 读取行注释
func (l *Lexer) readLineComment() Token {
	position := l.position
	for l.ch != '\n' && l.ch != 0 {
		l.readChar()
	}
	return l.newToken(COMMENT, l.input[position:l.position])
}

// readBlockComment 读取块注释
func (l *Lexer) readBlockComment() Token {
	position := l.position
	for {
		l.readChar()
		if l.ch == '*' && l.peekChar() == '/' {
			l.readChar()
			l.readChar()
			break
		}
		if l.ch == 0 {
			break
		}
	}
	return l.newToken(COMMENT, l.input[position:l.position])
}

// skipWhitespace 跳过空白字符
func (l *Lexer) skipWhitespace() {
	for l.ch == ' ' || l.ch == '\t' || l.ch == '\n' || l.ch == '\r' {
		l.readChar()
	}
}

// lookupIdent 查找标识符类型
func (l *Lexer) lookupIdent(ident string) TokenType {
	if tok, ok := l.keywords[ident]; ok {
		return tok
	}
	return IDENTIFIER
}

// newToken 创建新词法单元
func (l *Lexer) newToken(tokenType TokenType, ch interface{}) Token {
	var literal string
	switch v := ch.(type) {
	case byte:
		literal = string(v)
	case string:
		literal = v
	}
	return Token{
		Type:     tokenType,
		Literal:  literal,
		Position: Position{Line: l.line, Column: l.column, Offset: l.position},
	}
}

// isLetter 检查是否为字母
func isLetter(ch byte) bool {
	return unicode.IsLetter(rune(ch)) || ch == '_'
}

// isDigit 检查是否为数字
func isDigit(ch byte) bool {
	return '0' <= ch && ch <= '9'
}